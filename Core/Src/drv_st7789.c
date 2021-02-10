/*
 * drv_st7789.c
 *
 *  Created on: Oct 9, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "main.h"
#define RESOURCE
#include "resource.h"
#include "drv_st7789.h"

/********** Define **********/

#define SPI_TIMEOUT (1000) /* [ms] */

#define TFT_WIDTH  (240)
#define TFT_HEIGHT (240)

#define DC_COMMAND (GPIO_PIN_RESET)
#define DC_DATA    (GPIO_PIN_SET)
#define BLK_OFF    (GPIO_PIN_SET)
#define BLK_ON     (GPIO_PIN_RESET)
#define RES_OFF    (GPIO_PIN_SET)
#define RST_ON     (GPIO_PIN_RESET)

#define SPRITE_QUEUE_SIZE_MAX	(1000)

#define QUEUE_OK	(0)
#define QUEUE_NG	(1)

enum AsyncTxStateType {
	ASYNC_TX_IDLE = 0,
	ASYNC_TX_CASET,
	ASYNC_TX_RASET,
	ASYNC_TX_RAMWR
};

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

static SPI_HandleTypeDef* hspi_ptr;

static sprite_queue_t sprite_queue[SPRITE_QUEUE_SIZE_MAX];
static uint16_t sprite_queue_head;
static uint16_t sprite_queue_size;

static uint8_t async_tx_state;

static uint8_t caset_data[4];
static uint8_t raset_data[4];

/********** Function Prototype **********/

static void drvST7789InitSequence(void);
static void drvST7789SendCmmand(uint8_t command);
static void drvST7789SendData(uint8_t data);
static void drvST7789SendDataArray(const uint8_t* data, uint16_t length);
static void drvST7789SendCASET(uint8_t pos_x, uint8_t width);
static void drvST7789SendRASET(uint8_t pos_y, uint8_t height);
static void drvST7789WriteMemory(const uint8_t* data, uint16_t length);
static void drvST7789SpriteEnqueue(sprite_queue_t* enqueue_data);
static void drvST7789SpriteDequeue(void);
static void drvSt7789SetNewTx(void);

/********** Function **********/

/*=== 初期化関数 ===*/
void DrvST7789Init(SPI_HandleTypeDef* hspi)
{
	/* SPIハンドラ保持 */
	hspi_ptr = hspi;

	/* キュー初期化 */
	sprite_queue_head = 0;
	sprite_queue_size = 0;

	/* 非同期通信状態初期化 */
	async_tx_state = ASYNC_TX_IDLE;

	/* ST7789初期化 */
	drvST7789InitSequence();

	/* 画面黒塗りつぶし */
	DrvST7789ClearDisplay();
}

/*=== DMA転送完了割り込み処理関数 ===*/
void DrvST7789InterruptDMA(void)
{
	HAL_SPI_StateTypeDef spi_status;

	/* DMA送信の状態取得 */
	spi_status = HAL_SPI_GetState(hspi_ptr);

	if (spi_status == HAL_SPI_STATE_READY){
		/* 送信が完了している場合は次の送信を行う */
		switch(async_tx_state) {
		case ASYNC_TX_IDLE:
			/* 処理無し */
			break;
		case ASYNC_TX_CASET:	/* Column Address Set 完了 */
			/* Row Address Set送信 */
			async_tx_state = ASYNC_TX_RASET;
			drvST7789SendRASET(sprite_queue[sprite_queue_head].y, SpriteList[sprite_queue[sprite_queue_head].sprite_id].h);
			break;
		case ASYNC_TX_RASET:	/* Row Address Set 完了 */
			/* Memory Write送信 */
			async_tx_state = ASYNC_TX_RAMWR;
			drvST7789WriteMemory((unsigned char *)SpriteList[sprite_queue[sprite_queue_head].sprite_id].image, SpriteList[sprite_queue[sprite_queue_head].sprite_id].w * SpriteList[sprite_queue[sprite_queue_head].sprite_id].h * 2);
			break;
		case ASYNC_TX_RAMWR:	/* Memory Write 完了 */
			/* 次のデータがある場合は連続送信、なければ送信停止 */
			drvST7789SpriteDequeue();
			async_tx_state = ASYNC_TX_IDLE;
			drvSt7789SetNewTx();
			break;
		default:
			/* 状態異常時は初期値に戻す */
			async_tx_state = ASYNC_TX_IDLE;
			break;
		}
	}
}

/*=== スプライト描画関数 ===*/
void DrvST7789DrawSprite(sprite_queue_t* sprite)
{
	drvST7789SpriteEnqueue(sprite);
}

/*=== 表示ONOFF設定関数 ===*/
void DrvST7789SetDisplay(uint8_t display)
{
	sprite_queue_t sprite;

	if (display == DISPLAY_ON) {
		sprite.sprite_id = SPRITE_SPECIAL_DISPALY_ON;
	} else {
		sprite.sprite_id = SPRITE_SPECIAL_DISPALY_OFF;
	}

	drvST7789SpriteEnqueue(&sprite);
}

/*=== 画面クリア関数 ===*/
void DrvST7789ClearDisplay(void)
{
	sprite_queue_t clear_data;
	uint16_t index;

	clear_data.sprite_id = SPRITE_BLACK;
	clear_data.x = 0;

	for (index=0; index<240; index++) {
		clear_data.y = index;
		drvST7789SpriteEnqueue(&clear_data);
	}
}

/*=== 初期化シーケンス関数 ===*/
static void drvST7789InitSequence(void)
{
	HAL_GPIO_WritePin(TFT_RES_GPIO_Port, TFT_RES_Pin, RES_OFF);
	HAL_Delay(400);
	drvST7789SendCmmand(0x01); /* SWRESET (01h): Software Reset */
	HAL_Delay(120);
	drvST7789SendCmmand(0x11); /* SLPOUT (11h): Sleep Out */
	HAL_Delay(5);
	drvST7789SendCmmand(0x3A); /* COLMOD (3Ah): Interface Pixel Format */
	drvST7789SendData(0x55);   /* 65K of RGB interface, 16bit/pixel */
	drvST7789SendCmmand(0x36); /* MADCTL (36h): Memory Data Access Control */
	drvST7789SendData(0x00);   /* Page Address Order: Top to Bottom, Column Address Order: Left to Right */
	// drvST7789SendCmmand(0x20); /* INVOFF (20h): Display Inversion Off (0x0000:WHITE, 0xFFFF:BLACK */
	drvST7789SendCmmand(0x21); /* INVON (21h): Display Inversion On (0x0000:BLACK, 0xFFFF:WHITE) */
	// drvST7789SendCmmand(0x13); /* NORON (13h): Normal Display Mode On */
	drvST7789SendCmmand(0x29); /* DISPON (29h): Display On */
}

/*=== コマンド同期送信関数 ===*/
static void drvST7789SendCmmand(uint8_t command)
{
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, DC_COMMAND);
	HAL_SPI_Transmit(hspi_ptr, &command, 1, SPI_TIMEOUT);
}

/*=== データ同期送信関数 ===*/
static void drvST7789SendData(uint8_t data)
{
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, DC_DATA);
	HAL_SPI_Transmit(hspi_ptr, &data, 1, SPI_TIMEOUT);
}

/*=== 配列データ非同期送信関数 ===*/
static void drvST7789SendDataArray(const uint8_t* data, uint16_t length)
{
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, DC_DATA);
	HAL_SPI_Transmit_DMA(hspi_ptr, (unsigned char *)data, length);
}

/*=== Column Address Set送信関数 ===*/
static void drvST7789SendCASET(uint8_t pos_x, uint8_t width)
{
	drvST7789SendCmmand(0x2A); /* CASET (2Ah): Column Address Set */
	caset_data[0] = 0x00;
	caset_data[1] = pos_x;
	caset_data[2] = 0x00;
	caset_data[3] = pos_x + width - 1;
	drvST7789SendDataArray((const uint8_t*)caset_data, 4);
}

/*=== Row Address Set送信関数 ===*/
static void drvST7789SendRASET(uint8_t pos_y, uint8_t height)
{
	drvST7789SendCmmand(0x2B); /* RASET (2Bh): Row Address Set */
	raset_data[0] = 0x00;
	raset_data[1] = pos_y;
	raset_data[2] = 0x00;
	raset_data[3] = pos_y + height - 1;
	drvST7789SendDataArray((const uint8_t*)raset_data, 4);
}

/*=== Memory Write送信関数 ===*/
static void drvST7789WriteMemory(const uint8_t* data, uint16_t length)
{
	drvST7789SendCmmand(0x2C); /* RAMWR (2Ch): Memory Write */
	drvST7789SendDataArray(data, length);
}

/*=== スプライトエンキュー関数 ===*/
static void drvST7789SpriteEnqueue(sprite_queue_t* enqueue_data)
{
	uint16_t queue_end;

	if (sprite_queue_size < SPRITE_QUEUE_SIZE_MAX) {
		__disable_irq();
		queue_end = (sprite_queue_head + sprite_queue_size) % SPRITE_QUEUE_SIZE_MAX;
		sprite_queue[queue_end].sprite_id = enqueue_data->sprite_id;
		sprite_queue[queue_end].x = enqueue_data->x;
		sprite_queue[queue_end].y = enqueue_data->y;
		sprite_queue_size++;
		__enable_irq();
	}

	if (async_tx_state == ASYNC_TX_IDLE) {
		drvSt7789SetNewTx();
	}
}

/*=== スプライトデキュー関数 ===*/
static void drvST7789SpriteDequeue(void)
{
	if (sprite_queue_size != 0) {
		sprite_queue_head = (sprite_queue_head + 1) % SPRITE_QUEUE_SIZE_MAX;
		sprite_queue_size--;
	}
}

/*=== 新規送信設定関数 ===*/
static void drvSt7789SetNewTx(void)
{
	if (sprite_queue_size != 0) {
		if (sprite_queue[sprite_queue_head].sprite_id == SPRITE_SPECIAL_DISPALY_OFF) {
			/* 画面非表示 */
			HAL_GPIO_WritePin(TFT_BLK_GPIO_Port, TFT_BLK_Pin, BLK_ON);
			drvST7789SendCmmand(0x28); /* DISPOFF (28h): Display Off */
			drvST7789SpriteDequeue();
			drvSt7789SetNewTx();
		} else if (sprite_queue[sprite_queue_head].sprite_id == SPRITE_SPECIAL_DISPALY_ON) {
			/* 画面表示 */
			drvST7789SendCmmand(0x29); /* DISPON (29h): Display On */
			HAL_GPIO_WritePin(TFT_BLK_GPIO_Port, TFT_BLK_Pin, BLK_OFF);
			drvST7789SpriteDequeue();
			drvSt7789SetNewTx();
		} else {
			/* スプライトの送信開始 */
			async_tx_state = ASYNC_TX_CASET;
			drvST7789SendCASET(sprite_queue[sprite_queue_head].x, SpriteList[sprite_queue[sprite_queue_head].sprite_id].w);
		}
	}
}
