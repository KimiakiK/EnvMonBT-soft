/*
 * drv_bme280.h
 *
 *  Created on: Oct 9, 2020
 *      Author: kimi
 */

#ifndef INC_DRV_BME280_H_
#define INC_DRV_BME280_H_

/********** Include **********/

/********** Define **********/

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void DrvBME280Init(I2C_HandleTypeDef* hi2c);
void DrvBME280Main(void);
float DrvBME280GetTemperature(void);
float DrvBME280GetHumidity(void);
float DrvBME280GetPressure(void);

#endif /* INC_DRV_BME280_H_ */
