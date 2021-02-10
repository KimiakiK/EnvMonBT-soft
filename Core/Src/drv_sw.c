/*
 * drv_sw.c
 *
 *  Created on: Nov 1, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "main.h"
#include "drv_sw.h"

/********** Define **********/

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

static uint8_t swState[SW_NUM];

/********** Function Prototype **********/

/********** Function **********/

/*=== 初期化関数 ===*/
void DrvSwInit(void)
{
	uint8_t index;

	for (index=0; index<SW_NUM; index++) {
		swState[index] = SW_OFF;
	}
}

/*=== 周期関数 ===*/
void DrvSwMain(void)
{
	uint8_t pin_state;

	pin_state = HAL_GPIO_ReadPin(DISP_SW_GPIO_Port, DISP_SW_Pin);

	if (pin_state == GPIO_PIN_SET) {
		swState[SW_ID_DISPSW] = SW_ON;
	} else {
		swState[SW_ID_DISPSW] = SW_OFF;
	}
}

/*=== スイッチ状態取得関数 ===*/
uint8_t DrvSwGetSwState(uint8_t sw_id)
{
	uint8_t sw_state;

	sw_state = SW_OFF;

	if (sw_id < SW_NUM) {
		sw_state = swState[sw_id];
	}

	return sw_state;
}
