/*
 * apl_logger.c
 *
 *  Created on: Nov 2, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "math.h"

#include "main.h"
#include "drv_bme280.h"
#include "drv_mhz19b.h"
#include "drv_rtc.h"
#include "apl_logger.h"

/********** Define **********/

/* 分単位のログ実行間隔 */
#define LOG_INTERVAL_MINUTE		(3)
/* 時単位のログ実行間隔 */
#define LOG_INTERVAL_HOUR		(1)
/* 分単位のログ数 */
#define MINITE_LOG_NUM			(20)
/* 時単位のログ数 */
#define HOUR_LOG_NUM			(24*5)

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/* 平均値計算用 */
static float envAverage[ENV_DATA_NUM];
/* 分毎の平均値格納 */
static float envMinute[MINITE_LOG_NUM][ENV_DATA_NUM] __attribute__((section(".log"), used));
/* 時毎の平均値格納 */
static float envHour[HOUR_LOG_NUM][ENV_DATA_NUM] __attribute__((section(".log"), used));

static uint8_t envMinuteHead;
static uint8_t envHourHead;

/* 前回処理時刻 */
static time_t logTime;

/********** Function Prototype **********/

static void aplLoggerInputNewEnv(void);
static void aplLoggerEnvLogging(void);

/********** Function **********/

/*=== 初期化関数 ===*/
void AplLoggerInit(void)
{
	uint8_t env_id;
	uint8_t index;

	envMinuteHead = 0;
	envHourHead = 0;

	for (env_id=0; env_id<ENV_DATA_NUM; env_id++) {
		envAverage[env_id] = ENV_NODATA;
	}
	for (index=0; index<MINITE_LOG_NUM; index++) {
		for (env_id=0; env_id<ENV_DATA_NUM; env_id++) {
			envMinute[index][env_id] = ENV_NODATA;
		}

	}
	for (index=0; index<HOUR_LOG_NUM; index++) {
		for (env_id=0; env_id<ENV_DATA_NUM; env_id++) {
			envHour[index][env_id] = ENV_NODATA;
		}
	}
}

/*=== 周期関数 ===*/
void AplLoggerMain(void)
{
	/* 最新環境データ入力 */
	aplLoggerInputNewEnv();
	/* 環境データログ */
	aplLoggerEnvLogging();
}

/*=== 分単位環境データ取得関数 ===*/
float AplLoggerGetMinuteEnv(uint8_t env_id, uint8_t index, uint8_t* time)
{
	float env_value;

	/* 指定位置のデータ取得 */
	env_value = envMinute[(envMinuteHead + MINITE_LOG_NUM - index) % MINITE_LOG_NUM][env_id];
	/* 指定時間の一つ前の値を返す */
	*time = ((logTime.minute / LOG_INTERVAL_MINUTE) + MINITE_LOG_NUM - index - 1) % MINITE_LOG_NUM;

	return env_value;
}

/*=== 時単位環境データ取得関数 ===*/
float AplLoggerGetHourEnv(uint8_t env_id, uint8_t index, uint8_t* time)
{
	float env_value;

	/* 指定位置のデータ取得 */
	env_value = envHour[(envHourHead + HOUR_LOG_NUM - index) % HOUR_LOG_NUM][env_id];
	/* 指定時間の一つ前の値を返す */
	*time = (logTime.hour + 24 - index - 1) % 24;

	return env_value;
}

/*=== 最新環境データ入力関数 ===*/
static void aplLoggerInputNewEnv(void)
{
	float new_env[ENV_DATA_NUM];
	uint8_t env_id;

	new_env[ENV_TEMP] = DrvBME280GetTemperature();
	new_env[ENV_HUMI] = DrvBME280GetHumidity();
	new_env[ENV_PRESS] = DrvBME280GetPressure();
	new_env[ENV_CO2] = DrvMHZ19BGetCo2();

	for (env_id=0; env_id<ENV_DATA_NUM; env_id++) {
		if (isnanf(envAverage[env_id])) {
			envAverage[env_id] = new_env[env_id];
		} else {
			envAverage[env_id] = (envAverage[env_id] + new_env[env_id]) / 2.0f;
		}
	}
}

/*=== 環境データログ関数 ===*/
static void aplLoggerEnvLogging(void)
{
	time_t now_time;
	float sum;
	uint8_t env_id;
	uint8_t loop;
	uint8_t valid_data_num;

	/* 現在時刻取得 */
	now_time = DrvRtcGetNowTime();

	if (now_time.minute != logTime.minute) {
		if ((now_time.minute % LOG_INTERVAL_MINUTE) == 0) {
			/* 分の値が変化してログ間隔になったとき、新しい環境データをログ */
			envMinuteHead = (envMinuteHead + 1) % MINITE_LOG_NUM;
			for (env_id=0; env_id<ENV_DATA_NUM; env_id++) {
				envMinute[envMinuteHead][env_id] = envAverage[env_id];
				envAverage[env_id] = ENV_NODATA;
			}
		}
		logTime.minute = now_time.minute;
	}

	if (now_time.hour != logTime.hour) {
		if ((now_time.hour % LOG_INTERVAL_HOUR) == 0) {
			/* 時の値が変化してログ間隔になったとき、新しい環境データをログ */
			envHourHead = (envHourHead + 1) % HOUR_LOG_NUM;
			for (env_id=0; env_id<ENV_DATA_NUM; env_id++) {
				/* 分毎の環境データを平均してログ */
				sum = 0.0f;
				valid_data_num = 0;
				for (loop=0; loop<MINITE_LOG_NUM; loop++) {
					if (!isnanf(envMinute[loop][env_id])) {
						valid_data_num++;
						sum += envMinute[loop][env_id];
					}
				}
				envHour[envHourHead][env_id] = sum / valid_data_num;
			}
		}
		logTime.hour = now_time.hour;
	}
}
