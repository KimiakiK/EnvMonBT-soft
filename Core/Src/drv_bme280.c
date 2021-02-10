/*
 * drv_bme280.c
 *
 *  Created on: Oct 9, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "main.h"
#include "drv_bme280.h"

/********** Define **********/

#define BME280_I2C_SLAVE_ADDRESS ((uint8_t)(0x76 << 1))
#define I2C_TIMEOUT (1000) /* [ms] */

#define BME280_MODE (0b11) /* normal mode */
#define BME280_T_SB (0b000) /* 0.5ms */
#define BME280_FILTER (0b100) /* filter coefficient 16 */
#define BME280_OSRS_T (0b010) /* temperature ×2 */
#define BME280_OSRS_P (0b101) /* pressure ×16 */
#define BME280_OSRS_H (0b101) /* humidity ×16 */

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

static int32_t temperature;
static uint32_t pressure;
static uint32_t humidity;

static I2C_HandleTypeDef* hi2c_ptr;

static uint16_t dig_T1;
static int16_t  dig_T2;
static int16_t  dig_T3;
static uint16_t dig_P1;
static int16_t  dig_P2;
static int16_t  dig_P3;
static int16_t  dig_P4;
static int16_t  dig_P5;
static int16_t  dig_P6;
static int16_t  dig_P7;
static int16_t  dig_P8;
static int16_t  dig_P9;
static uint8_t  dig_H1;
static int16_t  dig_H2;
static uint8_t  dig_H3;
static int16_t  dig_H4;
static int16_t  dig_H5;
static int8_t   dig_H6;

static int32_t t_fine;

/********** Function Prototype **********/

static void drvBME280ReadParameter(void);
static void drvBME280SetControlRegister(void);

/********** Function **********/

/*=== 初期化関数 ===*/
void DrvBME280Init(I2C_HandleTypeDef* hi2c)
{
	hi2c_ptr = hi2c;

	drvBME280ReadParameter();
	drvBME280SetControlRegister();
}

/*=== 周期関数 ===*/
void DrvBME280Main(void)
{
	uint8_t i2c_tx_data[1];
	uint8_t i2c_rx_data[8];

	i2c_tx_data[0] = 0xF7; /* press_msb Regisiter */
	HAL_I2C_Master_Transmit(hi2c_ptr, BME280_I2C_SLAVE_ADDRESS, i2c_tx_data, 1, I2C_TIMEOUT);
	HAL_I2C_Master_Receive(hi2c_ptr, BME280_I2C_SLAVE_ADDRESS, i2c_rx_data, 8, I2C_TIMEOUT);

	{
		int32_t adc_T, var1, var2;
		adc_T = (int32_t)( ((uint32_t)i2c_rx_data[3] << 12) | ((uint32_t)i2c_rx_data[4] << 4) | ((uint32_t)i2c_rx_data[5] >> 4) );
		var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
		var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
		t_fine = var1 + var2;
		temperature =(t_fine*5+128)>>8;
	}
	{
		int32_t adc_P;
		int64_t var1, var2, p;
		adc_P = (int32_t)( ((uint32_t)i2c_rx_data[0] << 12) | ((uint32_t)i2c_rx_data[1] << 4) | ((uint32_t)i2c_rx_data[2] >> 4) );
		var1 = ((int64_t)t_fine) - 128000;
		var2 = var1 * var1 * (int64_t)dig_P6;
		var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
		var2 = var2 + (((int64_t)dig_P4)<<35);
		var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
		var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
		if (var1 != 0)
		{
			p = 1048576-adc_P;
			p = (((p<<31)-var2)*3125)/var1;
			var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
			var2 = (((int64_t)dig_P8) * p) >> 19;
			pressure = ( ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4) ) / 256;
		}
	}
	{
		int32_t adc_P;
		int64_t var1, var2, p, tmp;
		adc_P = (int32_t)( ((uint32_t)i2c_rx_data[0] << 12) | ((uint32_t)i2c_rx_data[1] << 4) | ((uint32_t)i2c_rx_data[2] >> 4) );
		var1 = ((int64_t)t_fine) - 128000;
		var2 = var1 * var1 * (int64_t)dig_P6;
		var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
		var2 = var2 + (((int64_t)dig_P4)<<35);
		var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
		var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
		if (var1 != 0)
		{
			p = 1048576-adc_P;
			p = (((p<<31)-var2)*3125)/var1;
			var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
			var2 = (((int64_t)dig_P8) * p) >> 19;
			tmp = ( ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4) );
			pressure = tmp / 256;
		}
	}
	{
		int32_t adc_H, v_x1_u32r;
		adc_H = (int32_t)( ((uint32_t)i2c_rx_data[6] << 8) | ((uint32_t)i2c_rx_data[7]) );
		v_x1_u32r = (t_fine - ((int32_t)76800));
		v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) * (((v_x1_u32r *((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
		v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
		v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
		v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
		humidity = v_x1_u32r>>12;
	}
}

/*=== 気温取得関数 ===*/
float DrvBME280GetTemperature(void)
{
	/* temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC. */
	return (float)temperature / 100.0f;
}

/*=== 湿度取得関数 ===*/
float DrvBME280GetHumidity(void)
{
	/* humidity in %RH, resolution is 0.001 %RH. Output value of “46333” equals 46.333 %RH. */
	return (float)humidity / 1000.0f;
}

/*=== 気圧取得関数 ===*/
float DrvBME280GetPressure(void)
{
	/* pressure in Pa, resolution is 0.1 Pa. Output value of “963862” equals 96386.2 Pa (963.862 hPa). */
	return (float)pressure / 100.f;
}

/*=== パラメータ読み出し関数 ===*/
static void drvBME280ReadParameter(void)
{
	uint8_t i2c_tx_data[1];
	uint8_t i2c_rx_data[26];

	i2c_tx_data[0] = 0x88; /* calib00 Regisiter */
	HAL_I2C_Master_Transmit(hi2c_ptr, BME280_I2C_SLAVE_ADDRESS, i2c_tx_data, 1, I2C_TIMEOUT);
	HAL_I2C_Master_Receive(hi2c_ptr, BME280_I2C_SLAVE_ADDRESS, i2c_rx_data, 26, I2C_TIMEOUT);
	dig_T1 = (uint16_t)i2c_rx_data[1] << 8 | (uint16_t)i2c_rx_data[0];
	dig_T2 = (int16_t)((uint16_t)i2c_rx_data[3] << 8 | (uint16_t)i2c_rx_data[2]);
	dig_T3 = (int16_t)((uint16_t)i2c_rx_data[5] << 8 | (uint16_t)i2c_rx_data[4]);
	dig_P1 = (uint16_t)i2c_rx_data[7] << 8 | (uint16_t)i2c_rx_data[6];
	dig_P2 = (int16_t)((uint16_t)i2c_rx_data[9] << 8 | (uint16_t)i2c_rx_data[8]);
	dig_P3 = (int16_t)((uint16_t)i2c_rx_data[11] << 8 | (uint16_t)i2c_rx_data[10]);
	dig_P4 = (int16_t)((uint16_t)i2c_rx_data[13] << 8 | (uint16_t)i2c_rx_data[12]);
	dig_P5 = (int16_t)((uint16_t)i2c_rx_data[15] << 8 | (uint16_t)i2c_rx_data[14]);
	dig_P6 = (int16_t)((uint16_t)i2c_rx_data[17] << 8 | (uint16_t)i2c_rx_data[16]);
	dig_P7 = (int16_t)((uint16_t)i2c_rx_data[19] << 8 | (uint16_t)i2c_rx_data[18]);
	dig_P8 = (int16_t)((uint16_t)i2c_rx_data[21] << 8 | (uint16_t)i2c_rx_data[20]);
	dig_P9 = (int16_t)((uint16_t)i2c_rx_data[23] << 8 | (uint16_t)i2c_rx_data[22]);
	dig_H1 = i2c_rx_data[25];

	i2c_tx_data[0] = 0xE1; /* calib26 Regisiter */
	HAL_I2C_Master_Transmit(hi2c_ptr, BME280_I2C_SLAVE_ADDRESS, i2c_tx_data, 1, I2C_TIMEOUT);
	HAL_I2C_Master_Receive(hi2c_ptr, BME280_I2C_SLAVE_ADDRESS, i2c_rx_data, 7, I2C_TIMEOUT);
	dig_H2 = (int16_t)((uint16_t)i2c_rx_data[1] << 8 | (uint16_t)i2c_rx_data[0]);
	dig_H3 = i2c_rx_data[2];
	dig_H4 = ( (int16_t)(int8_t)i2c_rx_data[3] * 16 ) | ( (int16_t)(i2c_rx_data[4] & 0x0F) );
	dig_H5 = ( (int16_t)(int8_t)i2c_rx_data[5] * 16 ) | ( (int16_t)(i2c_rx_data[4] >> 4) );
	dig_H6 = (int8_t)i2c_rx_data[6];
}

/*=== レジスタ設定関数 ===*/
static void drvBME280SetControlRegister(void)
{
	uint8_t i2c_tx_data[6];

	i2c_tx_data[0] = 0xF5; /* config Regisiter */
	i2c_tx_data[1] = BME280_T_SB << 5 | BME280_FILTER << 2;
	i2c_tx_data[2] = 0xF2; /* ctrl_hum Regisiter */
	i2c_tx_data[3] = BME280_OSRS_H;
	i2c_tx_data[4] = 0xF4; /* ctrl_meas Regisiter */
	i2c_tx_data[5] = BME280_OSRS_T << 5 | BME280_OSRS_P << 2 | BME280_MODE;
	HAL_I2C_Master_Transmit(hi2c_ptr, BME280_I2C_SLAVE_ADDRESS, i2c_tx_data, 6, I2C_TIMEOUT);
}
