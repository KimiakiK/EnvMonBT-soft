/*
 * sys_platform.c
 *
 *  Created on: Oct 9, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "main.h"
#include "drv_sw.h"
#include "drv_bme280.h"
#include "drv_mhz19b.h"
#include "drv_st7789.h"
#include "drv_rn4020.h"
#include "drv_rtc.h"
#include "apl_logger.h"
#include "apl_dispcontrol.h"
#include "apl_draw.h"
#include "apl_btcomm.h"
#include "sys_platform.h"

/********** Define **********/

#define TIMx_SR_UIF		(0x00000001)

#if 0
#define SLEEP (10000)
#define DISP_OFF (0)
#define DISP_ON  (1)
#endif

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

static TIM_HandleTypeDef* htim1_ptr;
static uint8_t count_10ms;

/********** Function Prototype **********/

/********** Function **********/

/*=== 初期化関数 ===*/
void SysPlatformInit(TIM_HandleTypeDef* htim1)
{
	count_10ms = 0;

	/* 10ms周期タイマー開始 */
	htim1_ptr = htim1;
	HAL_TIM_Base_Start(htim1_ptr);
}

/*=== メインループ関数 ===*/
void SysPlatformMain(void)
{
	DrvRN4020Main();
	
	switch (count_10ms) {
	case  0: /*   0ms */
		DrvRtcMain();			/* 現在時刻取得 */
		DrvSwMain();			/* スイッチ状態取得 */ /* 1.16us */
		AplDispcontrolMain();	/* 画面状態遷移 */
		break;
	case  1: /*  10ms */
		DrvBME280Main();		/* 気温、湿度、気圧取得 */ /* 309us*/
		break;
	case  2: /*  20ms */
		DrvMHZ19BMain();		/* CO2取得 */
		break;
	case  3: /*  30ms */
		AplDrawMain();
		break;
	case  4: /*  40ms */

		break;
	case  5: /*  50ms */

		break;
	case  6: /*  60ms */

		break;
	case  7: /*  70ms */

		break;
	case  8: /*  80ms */

		break;
	case  9: /*  90ms */
		AplLoggerMain();		/* 環境データ保存 */
		break;
	default:
		/* 処理無し */
		break;
	}

	count_10ms ++;
	if(count_10ms >= 10) {
		count_10ms = 0;
	}

	/* 10msタイマー満了待ち */
	while ((TIM1->SR & TIMx_SR_UIF) != TIMx_SR_UIF) {
		/* 処理無し */
	}
	/* タイマーフラグクリア */
	TIM1->SR = 0;
}
