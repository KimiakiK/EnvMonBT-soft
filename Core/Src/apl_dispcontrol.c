/*
 * apl_dispcontrol.c
 *
 *  Created on: Nov 2, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "main.h"
#include "drv_st7789.h"
#include "drv_sw.h"
#include "apl_dispcontrol.h"

/********** Define **********/

enum eventType {
	EVENT_OFF = 0,
	EVENT_ON
};

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

static uint8_t dispState;
static uint16_t dispTimer;

/********** Function Prototype **********/

static void aplDispcontrolDisplayOnOff(uint8_t disp_state_old);

static uint8_t aplDispcontrolEventSw(void);
static uint8_t aplDispcontrolEventTimer(void);

static void aplDispcontrolActionTimerClear(void);

/********** Function **********/

/*=== 初期化関数 ===*/
void AplDispcontrolInit(void)
{
	dispState = DISP_STATE_INIT;
	dispTimer = 0;
}

/*=== 周期関数 ===*/
void AplDispcontrolMain(void)
{
	uint8_t disp_state_old;
	uint8_t event_sw;
	uint8_t event_timer;

	disp_state_old = dispState;

	event_sw = aplDispcontrolEventSw();
	event_timer = aplDispcontrolEventTimer();

	switch (dispState) {
	case DISP_STATE_OFF:							/* 非表示状態 */
		if (event_sw == EVENT_ON) {					/* スイッチが押されたら */
			dispState = DISP_STATE_MAIN;			/* メイン表示へ遷移 */
			aplDispcontrolActionTimerClear();		/* タイマクリア */
		}
		break;
	case DISP_STATE_INIT:							/* 初期状態 */
		if (EVENT_ON == EVENT_ON) {					/* 無条件で */
			dispState = DISP_STATE_MAIN;			/* メイン表示へ遷移 */
			aplDispcontrolActionTimerClear();		/* タイマクリア */
		}
		break;
	case DISP_STATE_MAIN:							/* メイン表示状態 */
		if (event_sw == EVENT_ON) {					/* スイッチが押されたら */
			aplDispcontrolActionTimerClear();		/* タイマクリア */
		} else if (event_timer == EVENT_ON) {		/* 時間が経過したら */
			dispState = DISP_STATE_OFF;				/* 非表示へ遷移 */
		}
		break;
	case DISP_STATE_TEMP:

		break;
	case DISP_STATE_HUMI:

		break;
	case DISP_STATE_PRESS:

		break;
	case DISP_STATE_CO2:

		break;
	default:
		/* 異常時は初期値に戻す */
		dispState = DISP_STATE_INIT;
		break;
	}

	/* 表示ONOFF制御 */
	aplDispcontrolDisplayOnOff(disp_state_old);
}

/*=== 画面表示状態取得関数 ===*/
uint8_t AplDispcontrolGetDisplayState(void)
{
	return dispState;
}

/*=== 表示ONOFF制御関数 ===*/
static void aplDispcontrolDisplayOnOff(uint8_t disp_state_old)
{
	/* 表示ON判定 */
	if (((disp_state_old == DISP_STATE_OFF) && (dispState != DISP_STATE_OFF)) ||
			((disp_state_old == DISP_STATE_INIT) && (dispState != DISP_STATE_INIT))) {
		DrvST7789SetDisplay(DISPLAY_ON);
	}
	/* 表示OFF判定 */
	if ((disp_state_old != DISP_STATE_OFF) && (dispState == DISP_STATE_OFF)) {
		DrvST7789SetDisplay(DISPLAY_OFF);
	}
}

/*=== SWイベント関数 ===*/
static uint8_t aplDispcontrolEventSw(void)
{
	uint8_t event;
	uint8_t sw_state;

	sw_state = DrvSwGetSwState(SW_ID_DISPSW);

	if (sw_state == SW_ON) {
		event = EVENT_ON;
	} else {
		event = EVENT_OFF;
	}

	return event;
}

/*=== タイマイベント関数 ===*/
static uint8_t aplDispcontrolEventTimer(void)
{
	uint8_t event;

	dispTimer ++;

	if (dispTimer >= 150) {
		event = EVENT_ON;
	} else {
		event = EVENT_OFF;
	}

	return event;
}

/*=== タイマクリアアクション関数 ===*/
static void aplDispcontrolActionTimerClear(void)
{
	dispTimer = 0;
}
