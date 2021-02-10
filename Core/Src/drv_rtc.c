/*
 * drv_rtc.c
 *
 *  Created on: Nov 2, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "main.h"
#include "drv_rtc.h"

/********** Define **********/

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

static RTC_HandleTypeDef* hrtc_ptr;

static time_t nowTime;

/********** Function Prototype **********/

/********** Function **********/

/*=== 初期化関数 ===*/
void DrvRtcInit(RTC_HandleTypeDef* hrtc)
{
	time_t init_time = {0};

	hrtc_ptr = hrtc;

#if 0
	init_time.hour = 21;
	init_time.minute = 0;
#endif

	DrvRtcSetTime(init_time);

	/* RTCレジスタ書き込み保護解除 */
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	/* 時刻を常時読み出すためにBYPSHAD制御ビットをセット */
	RTC->CR |= 0x00000020;
	/* RTCレジスタ書き込み保護有効化 */
	RTC->WPR = 0x00;
}

/*=== 周期関数 ===*/
void DrvRtcMain(void)
{
	RTC_TimeTypeDef rtc_data;
	HAL_RTC_GetTime(hrtc_ptr, &rtc_data, RTC_FORMAT_BIN);
	nowTime.hour = rtc_data.Hours;
	nowTime.minute = rtc_data.Minutes;
	nowTime.second = rtc_data.Seconds;

#if 0
	nowTime.hour = nowTime.minute;
	nowTime.minute = nowTime.second;
#endif
}

/*=== 現在時刻取得関数 ===*/
time_t DrvRtcGetNowTime(void)
{
	return nowTime;
}

/*=== 時刻設定関数 ===*/
void DrvRtcSetTime(time_t new_time)
{
	RTC_TimeTypeDef rtc_time;

	rtc_time.Hours = new_time.hour;
	rtc_time.Minutes = new_time.minute;
	rtc_time.Seconds = new_time.second;
	rtc_time.TimeFormat = RTC_HOURFORMAT12_AM;
	rtc_time.SubSeconds = 0;
	rtc_time.SecondFraction = 0;
	rtc_time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	rtc_time.StoreOperation = RTC_STOREOPERATION_RESET;

	HAL_RTC_SetTime(hrtc_ptr, &rtc_time, RTC_FORMAT_BIN);
}
