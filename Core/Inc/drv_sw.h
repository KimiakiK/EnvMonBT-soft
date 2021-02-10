/*
 * drv_sw.h
 *
 *  Created on: Nov 1, 2020
 *      Author: kimi
 */

#ifndef INC_DRV_SW_H_
#define INC_DRV_SW_H_


/********** Include **********/

/********** Define **********/

#define SW_OFF	(0)
#define SW_ON	(1)

enum swIdType {
	SW_ID_DISPSW = 0,
	SW_NUM
};

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void DrvSwInit(void);
void DrvSwMain(void);
uint8_t DrvSwGetSwState(uint8_t sw_id);

#endif /* INC_DRV_SW_H_ */
