/*
 * drv_rtc.h
 *
 *  Created on: Nov 2, 2020
 *      Author: kimi
 */

#ifndef INC_DRV_RTC_H_
#define INC_DRV_RTC_H_

/********** Include **********/

/********** Define **********/

/********** Type **********/

typedef struct timeType {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} time_t;

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void DrvRtcInit(RTC_HandleTypeDef* hrtc);
void DrvRtcMain(void);
time_t DrvRtcGetNowTime(void);
void DrvRtcSetTime(time_t new_time);

#endif /* INC_DRV_RTC_H_ */
