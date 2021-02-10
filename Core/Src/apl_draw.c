/*
 * apl_draw.c
 *
 *  Created on: Nov 2, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "main.h"
#include "resource.h"
#include "drv_bme280.h"
#include "drv_mhz19b.h"
#include "drv_st7789.h"
#include "apl_logger.h"
#include "apl_dispcontrol.h"
#if 0
#include "drv_rtc.h"
#endif
#include "apl_draw.h"

/********** Define **********/

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

static uint8_t oldDispState;

static float oldTemperature;
static float oldPressure;
static float oldHumidity;
static float oldCo2;

/********** Function Prototype **********/

static void aplDrawBackgroundMain(void);

static void aplDrawCyclicMainTemperature(void);
static void aplDrawCyclicMainPressure(void);
static void aplDrawCyclicMainHumidity(void);
static void aplDrawCyclicMainCo2(void);

/********** Function **********/

/*=== 初期化関数 ===*/
void AplDrawInit(void)
{
	oldDispState = DISP_STATE_INIT;
}

/*=== 周期関数 ===*/
void AplDrawMain(void)
{
	uint8_t current_disp_state;

	current_disp_state = AplDispcontrolGetDisplayState();

	if (current_disp_state != oldDispState) {
		/* 画面遷移時はまず画面をクリア */
		DrvST7789ClearDisplay();

		/* 画面毎の固定スプライト表示 */
		switch (current_disp_state) {
		case DISP_STATE_MAIN:
			aplDrawBackgroundMain();
			break;
		default:
			/* 処理無し */
			break;
		}
	}

	/* 毎周期の表示処理 */
	switch (current_disp_state) {
	case DISP_STATE_MAIN:
		aplDrawCyclicMainTemperature();
		aplDrawCyclicMainPressure();
		aplDrawCyclicMainHumidity();
		aplDrawCyclicMainCo2();
		break;
	default:
		/* 処理無し */
		break;
	}

	oldDispState = current_disp_state;
}

static void aplDrawBackgroundMain(void)
{
	sprite_queue_t sprite;
	uint8_t index;

	for (index=SPRITE_IconTemp; index<=SPRITE_DotPress; index++) {
		sprite.sprite_id = index;
		sprite.x = SpriteList[index].x;
		sprite.y = SpriteList[index].y;
		DrvST7789DrawSprite(&sprite);
	}

	/* 過去値初期化 */
	oldTemperature = ENV_NODATA;
	oldPressure = ENV_NODATA;
	oldHumidity = ENV_NODATA;
	oldCo2 = ENV_NODATA;
}

static void aplDrawCyclicMainTemperature(void)
{
	float temperature;
	uint8_t index;
	uint8_t minus_flag;
	sprite_queue_t sprite;

	sprite.y = 17;

	temperature = DrvBME280GetTemperature();

	if (temperature != oldTemperature) {
		oldTemperature = temperature;

		/* マイナス判定 */
		if (temperature < 0.0f) {
			temperature *= -1.0f;
			minus_flag = 1;
		}

		/* 2桁の時の符号表示 */
		if (temperature >= 10.0f && minus_flag == 1){
			sprite.sprite_id = SPRITE_minus;
		} else {
			sprite.sprite_id = SPRITE_numBlack;
		}
		sprite.x = 85;
		DrvST7789DrawSprite(&sprite);

		/* 2桁目表示 */
		index = temperature / 10.0f;
		if (index == 0) {
			if (minus_flag == 1) {
				sprite.sprite_id = SPRITE_minus;
			} else {
				sprite.sprite_id = SPRITE_numBlack;
			}
		} else {
			sprite.sprite_id = index + SPRITE_num0;
		}
		sprite.x = 108;
		DrvST7789DrawSprite(&sprite);

		/* 一1桁目表示 */
		index = ((uint8_t)temperature) % 10;
		sprite.sprite_id = index + SPRITE_num0;
		sprite.x = 131;
		DrvST7789DrawSprite(&sprite);

		/* 小数点以下第1位表示 */
		index = ((uint8_t)(temperature * 10.0f)) % 10;
		sprite.sprite_id = index + SPRITE_num0;
		sprite.x = 161;
		DrvST7789DrawSprite(&sprite);
	}
}

static void aplDrawCyclicMainPressure(void)
{
	float pressure;
	uint8_t index;
	uint8_t suppress;
	sprite_queue_t sprite;

	sprite.y = 137;

	suppress = 1;

	pressure = DrvBME280GetPressure();

	if (pressure != oldPressure) {
		oldPressure = pressure;

		/* 4桁目表示 */
		index = pressure / 1000.0f;
		if (index == 0) {
			sprite.sprite_id = SPRITE_numBlack;
		} else {
			suppress = 0;
			sprite.sprite_id = index + SPRITE_num0;
		}
		sprite.x = 62;
		DrvST7789DrawSprite(&sprite);

		/* 3桁目表示 */
		index = ((uint16_t)(pressure / 100.0f)) % 10;
		if (index == 0 && suppress == 1) {
			sprite.sprite_id = SPRITE_numBlack;
		} else {
			suppress = 0;
			sprite.sprite_id = index + SPRITE_num0;
		}
		sprite.x = 85;
		DrvST7789DrawSprite(&sprite);

		/* 2桁目表示 */
		index = ((uint16_t)(pressure / 10.0f)) % 10;
		if (index == 0 && suppress == 1) {
			sprite.sprite_id = SPRITE_numBlack;
		} else {
			sprite.sprite_id = index + SPRITE_num0;
		}
		sprite.x = 108;
		DrvST7789DrawSprite(&sprite);

		/* 1桁目表示 */
		index = ((uint16_t)pressure) % 10;
		sprite.sprite_id = index + SPRITE_num0;
		sprite.x = 131;
		DrvST7789DrawSprite(&sprite);

		/* 小数点以下第1位表示 */
		index = ((uint16_t)(pressure * 10.0f)) % 10;
		sprite.sprite_id = index + SPRITE_num0;
		sprite.x = 161;
		DrvST7789DrawSprite(&sprite);
	}
}

static void aplDrawCyclicMainHumidity(void)
{
	float humidity;
	uint8_t index;
	uint8_t suppress;
	sprite_queue_t sprite;

	sprite.y = 77;

	suppress = 1;

	humidity = DrvBME280GetHumidity();

	if (humidity != oldHumidity) {
		oldHumidity = humidity;

		/* 3桁目表示 */
		index = humidity / 100.0f;
		if (index == 0) {
			sprite.sprite_id = SPRITE_numBlack;
		} else {
			suppress = 0;
			sprite.sprite_id = index + SPRITE_num0;
		}
		sprite.x = 85;
		DrvST7789DrawSprite(&sprite);

		/* 2桁目表示 */
		index = ((uint16_t)(humidity / 10.0f)) % 10;
		if (index == 0 && suppress == 1) {
			sprite.sprite_id = SPRITE_numBlack;
		} else {
			sprite.sprite_id = index + SPRITE_num0;
		}
		sprite.x = 108;
		DrvST7789DrawSprite(&sprite);

		/* 1桁目表示 */
		index = ((uint16_t)humidity) % 10;
		sprite.sprite_id = index + SPRITE_num0;
		sprite.x = 131;
		DrvST7789DrawSprite(&sprite);

		/* 小数点以下第1位表示 */
		index = ((uint16_t)(humidity * 10.0f)) % 10;
		sprite.sprite_id = index + SPRITE_num0;
		sprite.x = 161;
		DrvST7789DrawSprite(&sprite);
	}
}

static void aplDrawCyclicMainCo2(void)
{
	float co2;
	uint8_t index;
	uint8_t suppress;
	sprite_queue_t sprite;

	sprite.y = 197;

	suppress = 1;

	co2 = DrvMHZ19BGetCo2();

#if 0
	{
		time_t time = DrvRtcGetNowTime();
		co2 = time.minute * 100 + time.second;
	}
#endif

	if (co2 != oldCo2) {
		oldCo2 = co2;

		/* 4桁目表示 */
		index = co2 / 1000.0f;
		if (index == 0) {
			sprite.sprite_id = SPRITE_numBlack;
		} else {
			suppress = 0;
			sprite.sprite_id = index + SPRITE_num0;
		}
		sprite.x = 92;
		DrvST7789DrawSprite(&sprite);

		/* 3桁目表示 */
		index = ((uint16_t)(co2 / 100.0f)) % 10;
		if (index == 0 && suppress == 1) {
			sprite.sprite_id = SPRITE_numBlack;
		} else {
			suppress = 0;
			sprite.sprite_id = index + SPRITE_num0;
		}
		sprite.x = 115;
		DrvST7789DrawSprite(&sprite);

		/* 2桁目表示 */
		index = ((uint16_t)(co2 / 10.0f)) % 10;
		if (index == 0 && suppress == 1) {
			sprite.sprite_id = SPRITE_numBlack;
		} else {
			sprite.sprite_id = index + SPRITE_num0;
		}
		sprite.x = 138;
		DrvST7789DrawSprite(&sprite);

		/* 1桁目表示 */
		index = ((uint16_t)co2) % 10;
		sprite.sprite_id = index + SPRITE_num0;
		sprite.x = 161;
		DrvST7789DrawSprite(&sprite);
	}
}
