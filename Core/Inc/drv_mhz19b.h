/*
 * drv_mhz19b.h
 *
 *  Created on: Oct 9, 2020
 *      Author: kimi
 */

#ifndef INC_DRV_MHZ19B_H_
#define INC_DRV_MHZ19B_H_

/********** Include **********/

/********** Define **********/

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void DrvMHZ19BInit(UART_HandleTypeDef* huart);
void DrvMHZ19BMain(void);
float DrvMHZ19BGetCo2(void);

#endif /* INC_DRV_MHZ19B_H_ */
