/*
 * apl_dispcontrol.h
 *
 *  Created on: Nov 2, 2020
 *      Author: kimi
 */

#ifndef INC_APL_DISPCONTROL_H_
#define INC_APL_DISPCONTROL_H_


/********** Include **********/

/********** Define **********/

/* 表示OFF時間 [10ms] */
#define DISP_OFF_TIME	(250)

enum dispStateType {
	DISP_STATE_OFF = 0,
	DISP_STATE_INIT,
	DISP_STATE_MAIN,
	DISP_STATE_TEMP,
	DISP_STATE_HUMI,
	DISP_STATE_PRESS,
	DISP_STATE_CO2,
	DISP_STATE_NUM
};

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void AplDispcontrolInit(void);
void AplDispcontrolMain(void);
uint8_t AplDispcontrolGetDisplayState(void);

#endif /* INC_APL_DISPCONTROL_H_ */
