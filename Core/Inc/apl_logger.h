/*
 * apl_logger.h
 *
 *  Created on: Nov 2, 2020
 *      Author: kimi
 */

#ifndef INC_APL_LOGGER_H_
#define INC_APL_LOGGER_H_

/********** Include **********/

#include <math.h>
#include <float.h>

/********** Define **********/

#define ENV_NODATA		(NAN)

enum envDataType {
	ENV_TEMP = 0,
	ENV_HUMI,
	ENV_PRESS,
	ENV_CO2,
	ENV_DATA_NUM
};

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void AplLoggerInit(void);
void AplLoggerMain(void);
float AplLoggerGetMinuteEnv(uint8_t env_id, uint8_t index, uint8_t* time);
float AplLoggerGetHourEnv(uint8_t env_id, uint8_t index, uint8_t* time);

#endif /* INC_APL_LOGGER_H_ */
