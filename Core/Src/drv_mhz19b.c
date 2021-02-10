/*
 * drv_mhz19b.c
 *
 *  Created on: Oct 9, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "main.h"
#include "drv_mhz19b.h"

/********** Define **********/

#define UART_TIMEOUT (1000) /* [ms] */

#define UART_DATA_SIZE	(9)

enum CommandIdType {
	COMMAND_READ_CO2 = 0,
	COMMAND_ZERO_POINT_CALIBRATION,
	COMMAND_ABC_LOGIC_ON,
	COMMAND_ABC_LOGIC_OFF,
	COMMAND_NUM
};

/********** Type **********/

/********** Constant **********/

const uint8_t commandData[COMMAND_NUM][UART_DATA_SIZE] = {
		{0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}, /* Read CO2 concentration */
		{0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78}, /* Zero Point Calibration */
		{0xFF, 0x01, 0x79, 0xA0, 0x00, 0x00, 0x00, 0x00, 0xE6}, /* ABC logic on */
		{0xFF, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86}  /* ABC logic off */
};

/********** Variable **********/

static UART_HandleTypeDef* huart_ptr;

static uint16_t co2;

static uint8_t uartRxData[UART_DATA_SIZE];
static uint8_t uartTxData[UART_DATA_SIZE];

/********** Function Prototype **********/

void drvMHZ19BSetTxData(uint8_t command_id);

/********** Function **********/

/*=== 初期化関数 ===*/
void DrvMHZ19BInit(UART_HandleTypeDef* huart)
{
	uint8_t index;

	huart_ptr = huart;

	for (index=0; index<UART_DATA_SIZE; index++) {
		uartRxData[index] = 0x00;
		uartTxData[index] = 0x00;
	}

	DrvMHZ19BMain();
}

/*=== 周期関数 ===*/
void DrvMHZ19BMain(void)
{
	uint8_t index;

	/* DMA転送された受信データを判定 */
	if ((uartRxData[0] == 0xFF) && (uartRxData[1] == 0x86)) {
		/* CO2濃度を取得 */
		co2 = ( (uint16_t)uartRxData[2] << 8 ) | (uint16_t)uartRxData[3];
	}

	/* 次の送受信の前に一旦DMA転送を停止 */
	HAL_UART_DMAStop(huart_ptr);

	/* 受信バッファクリア */
	for (index=0; index<UART_DATA_SIZE; index++) {
		uartRxData[index] = 0x00;
	}

	/* 送信データ設定 */
	drvMHZ19BSetTxData(COMMAND_READ_CO2);

	/* 受信開始 */
	HAL_UART_Receive_DMA(huart_ptr, uartRxData, UART_DATA_SIZE);

	/* 送信開始 */
	huart_ptr->gState &= 0xFFFFFFFE; /* TX_BUSYが解除されないので強制的に解除する */
	HAL_UART_Transmit_DMA(huart_ptr, uartTxData, UART_DATA_SIZE);
}

/*=== CO2取得関数 ===*/
float DrvMHZ19BGetCo2(void)
{
	/* co2 in ppm, resolution is 1 ppm. Output value of “413” equals 413 ppm. */
	return (float)co2;
}

/*=== 送信データ設定関数 ===*/
void drvMHZ19BSetTxData(uint8_t command_id)
{
	uint8_t index;

	if (command_id < COMMAND_NUM) {
		for (index=0; index<UART_DATA_SIZE; index++) {
			uartTxData[index] = commandData[command_id][index];
		}
	}
}
