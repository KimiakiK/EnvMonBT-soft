/*
 * drv_st7789.h
 *
 *  Created on: Oct 9, 2020
 *      Author: kimi
 */

#ifndef INC_DRV_ST7789_H_
#define INC_DRV_ST7789_H_

/********** Include **********/

#include "resource.h"

/********** Define **********/

#define DISPLAY_OFF		(0)
#define DISPLAY_ON		(1)

/********** Type **********/

typedef struct SpriteQueueType {
	uint8_t sprite_id;
	uint8_t x;
	uint8_t y;
} sprite_queue_t;

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void DrvST7789Init(SPI_HandleTypeDef* hspi);
void DrvST7789InterruptDMA(void);
void DrvST7789DrawSprite(sprite_queue_t* sprite);
void DrvST7789SetDisplay(uint8_t display);
void DrvST7789ClearDisplay(void);

#endif /* INC_DRV_ST7789_H_ */
