/*
 * drv_rn4020.c
 *
 *  Created on: Oct 10, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "main.h"
#include "drv_rn4020.h"

/********** Define **********/

#define UART_TIMEOUT (1000) /* [ms] */
#define RX_BUFFER_SIZE (1024)

/********** Type **********/

/********** Constant **********/

//const uint8_t tx_data [] = "+\rS-,EnvMonBT\rSS,A0000001\rSR,00000000\rR,1\rD\rA\r";
const uint8_t tx_data [] = "+\rSF\rS-,EnvMonBT\rSS,A0000001\rSR,00000000\rPZ\rPS,456E764D6F6E42543130302020202020\rPC,456E764D6F6E425454656D7020202020,02,04\rPC,456E764D6F6E42545072657373757265,02,04\rR,1\rD\rA\rLS\r";


const uint8_t command_Echo[] = "+\r";
const uint8_t command_FactoryDefault[] = "SF,1\r";
const uint8_t command_SerializedName[] = "S-,EnvMonBT\r";
const uint8_t command_Name[] = "SN,EnvMonBT\r";
const uint8_t command_ServerServices[] = "SS,00000001\r";
const uint8_t command_Features[] = "SR,00000000\r";
const uint8_t command_ClearPrivateService[] = "PZ\r";
const uint8_t command_PrivateServiceUUID[] = "PS,456E764D6F6E42543130302020202020\r";
const uint8_t command_privateCharacteristicUUID[] = "PC,456E764D6F6E425454656D7020202020,02,04\r";
const uint8_t command_Reboot[] = "R,1\r";
const uint8_t command_DumpConfiguration[] = "D\r";
const uint8_t command_ListServerServices[] = "LS\r";
const uint8_t command_Advertise[] = "A\r";

/********** Variable **********/

static UART_HandleTypeDef* huart_ptr;
static uint8_t rx_data[RX_BUFFER_SIZE];

/********** Function Prototype **********/

/********** Function **********/

void DrvRN4020Init(UART_HandleTypeDef* huart)
{
	huart_ptr = huart;
/*
	HAL_UART_Receive_DMA(huart_ptr, rx_data, RX_BUFFER_SIZE);

	//HAL_UART_Transmit(huart_ptr, tx_data, sizeof(tx_data)-2, UART_TIMEOUT);

	HAL_UART_Transmit(huart_ptr, command_Echo, sizeof(command_Echo)-1, UART_TIMEOUT);
	HAL_Delay(100);

	HAL_UART_Transmit(huart_ptr, command_FactoryDefault, sizeof(command_FactoryDefault)-1, UART_TIMEOUT);
	HAL_Delay(100);

	HAL_UART_Transmit(huart_ptr, command_SerializedName, sizeof(command_SerializedName)-1, UART_TIMEOUT);
	HAL_Delay(100);

	HAL_UART_Transmit(huart_ptr, command_Name, sizeof(command_Name)-1, UART_TIMEOUT);
	HAL_Delay(100);

	HAL_UART_Transmit(huart_ptr, command_ServerServices, sizeof(command_ServerServices)-1, UART_TIMEOUT);
	HAL_Delay(100);

	HAL_UART_Transmit(huart_ptr, command_Features, sizeof(command_Features)-1, UART_TIMEOUT);
	HAL_Delay(100);

	HAL_UART_Transmit(huart_ptr, command_ClearPrivateService, sizeof(command_ClearPrivateService)-1, UART_TIMEOUT);
	HAL_Delay(100);

	HAL_UART_Transmit(huart_ptr, command_PrivateServiceUUID, sizeof(command_PrivateServiceUUID)-1, UART_TIMEOUT);
	HAL_Delay(100);

	HAL_UART_Transmit(huart_ptr, command_privateCharacteristicUUID, sizeof(command_privateCharacteristicUUID)-1, UART_TIMEOUT);
	HAL_Delay(100);

	HAL_UART_Transmit(huart_ptr, command_Reboot, sizeof(command_Reboot)-1, UART_TIMEOUT);
	HAL_Delay(5000);

	HAL_UART_Transmit(huart_ptr, command_DumpConfiguration, sizeof(command_DumpConfiguration)-1, UART_TIMEOUT);
	HAL_Delay(100);

	HAL_UART_Transmit(huart_ptr, command_ListServerServices, sizeof(command_ListServerServices)-1, UART_TIMEOUT);
	HAL_Delay(100);

	HAL_UART_Transmit(huart_ptr, command_Advertise, sizeof(command_Advertise)-1, UART_TIMEOUT);
	HAL_Delay(100);

	HAL_Delay(300);
	HAL_Delay(300);
	*/
}
