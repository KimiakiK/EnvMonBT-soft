/*
 * apl_draw.c
 *
 *  Created on: Nov 2, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "main.h"
#include "resource.h"
#include "drv_bme280.h"
#include "drv_mhz19b.h"
#include "drv_st7789.h"
#include "apl_logger.h"
#include "apl_dispcontrol.h"
#include "drv_rtc.h"
#include "apl_draw.h"

/********** Define **********/

/* 分・時の表示数 */
#define LOG_MINUTE_NUM	(20)
#define LOG_HOUR_NUM	(24)

#define GRAPH_Y_TOP				(77)
#define GRAPH_Y_BOTTOM			(131)
#define GRAPH_Y_INTERVAL		(90)

enum signType {
	SIGN_OFF = 0,
	SIGN_ON
};

enum fractionType {
	FRACTION_OFF = 0,
	FRACTION_ON
};

enum suppressType {
	SUPPRESS_OFF = 0,
	SUPPRESS_ON
};

enum stringSizeType {
	SIZE_L = SPRITE_num0_L,
	SIZE_M = SPRITE_num0_M,
	SIZE_S = SPRITE_num0_S
};

/********** Type **********/

typedef struct logType {
	float value;
	uint8_t time;
} log_t;

typedef struct statisticsType {
	float max;
	float ave;
	float min;
} statistics_t;

typedef struct formatType {
	uint8_t digit;
	uint8_t sign;
	uint8_t fraction;
} format_t;

/********** Constant **********/

/* ログ画面の横軸数値描画テーブル */
const sprite_queue_t logNumTable[] = {
		{SPRITE_num0_S, 1, 135},
		{SPRITE_num1_S, 50, 135},
		{SPRITE_num5_S, 60, 135},
		{SPRITE_num3_S, 110, 135},
		{SPRITE_num0_S, 120, 135},
		{SPRITE_num4_S, 170, 135},
		{SPRITE_num5_S, 180, 135},
		{SPRITE_num5_S, 219, 135},
		{SPRITE_num9_S, 229, 135},
		{SPRITE_num0_S, 1, 225},
		{SPRITE_num6_S, 60, 225},
		{SPRITE_num1_S, 115, 225},
		{SPRITE_num2_S, 125, 225},
		{SPRITE_num1_S, 175, 225},
		{SPRITE_num8_S, 185, 225},
		{SPRITE_num2_S, 219, 225},
		{SPRITE_num3_S, 229, 225}
};

/* ログ画面のアイコン描画テーブル */
const sprite_queue_t logIconTable[ENV_DATA_NUM] = {
		{SPRITE_IconTemp, 22, 5},
		{SPRITE_IconHumi, 22, 5},
		{SPRITE_IconPress, 5, 5},
		{SPRITE_IconCo2, 8, 13}
};

/* ログ画面の単位描画テーブル */
const sprite_queue_t logUnitTable[ENV_DATA_NUM] = {
		{SPRITE_UnitTemp, 160, 35},
		{SPRITE_UnitHumi, 160, 35},
		{SPRITE_UnitPress, 160, 35},
		{SPRITE_UnitCo2, 160, 35}
};

/* 統計情報描画テーブル(スプライトIDは未使用) */
const sprite_queue_t statisticsDrawTable[ENV_DATA_NUM] = {
		{0, 103, 5},
		{0, 103, 5},
		{0, 92, 5},
		{0, 108, 5}
};

/* 環境データ描画書式テーブル */
const format_t envFromatTable[ENV_DATA_NUM] = {
		{2, SIGN_ON, FRACTION_ON},
		{3, SIGN_OFF, FRACTION_ON},
		{4, SIGN_OFF, FRACTION_ON},
		{4, SIGN_OFF, FRACTION_OFF},
};

/********** Variable **********/

static uint8_t oldDispState;

static float oldTemperature;
static float oldPressure;
static float oldHumidity;
static float oldCo2;

static log_t logMinute[LOG_MINUTE_NUM];
static log_t logHour[LOG_HOUR_NUM];
static statistics_t logStatistics;

static time_t oldTime;

/********** Function Prototype **********/

static void aplDrawBackgroundMain(void);
static void aplDrawBackgroundLog(uint8_t env_id);

static void aplDrawCyclicMain(void);
static void aplDrawCyclicLog(uint8_t env_id);

static void aplDrawValue(float value ,uint8_t digit, uint8_t size, uint8_t pos_x, uint8_t pos_y, uint8_t interval, uint8_t sign, uint8_t fraction, uint8_t suppress);
static void aplDrawClock(void);

static void aplDrawCalcStatistics(void);
static uint8_t aplDrawCalcYpos(float value);

/********** Function **********/

/*=== 初期化関数 ===*/
void AplDrawInit(void)
{
	oldDispState = DISP_STATE_INIT;
}

/*=== 周期関数 ===*/
void AplDrawMain(void)
{
	uint8_t current_disp_state;

	current_disp_state = AplDispcontrolGetDisplayState();

	if (current_disp_state != oldDispState) {
		/* 画面遷移時はまず画面をクリア */
		DrvST7789ClearDisplay();

		/* 画面毎の固定スプライト表示 */
		switch (current_disp_state) {
		case DISP_STATE_MAIN:
			aplDrawBackgroundMain();
			break;
		case DISP_STATE_TEMP:
		case DISP_STATE_HUMI:
		case DISP_STATE_PRESS:
		case DISP_STATE_CO2:
			aplDrawBackgroundLog(current_disp_state - DISP_STATE_TEMP);
			break;
		default:
			/* 処理無し */
			break;
		}
	}

	/* 毎周期の表示処理 */
	switch (current_disp_state) {
	case DISP_STATE_MAIN:
		aplDrawCyclicMain();
		break;
	case DISP_STATE_TEMP:
	case DISP_STATE_HUMI:
	case DISP_STATE_PRESS:
	case DISP_STATE_CO2:
		aplDrawCyclicLog(current_disp_state - DISP_STATE_TEMP);
		aplDrawClock();
		break;
	default:
		/* 処理無し */
		break;
	}

	oldDispState = current_disp_state;
}

/*=== メイン画面背景描画関数 ===*/
static void aplDrawBackgroundMain(void)
{
	sprite_queue_t sprite;
	uint8_t index;

	for (index=SPRITE_IconTemp; index<=SPRITE_UnitCo2; index++) {
		sprite.sprite_id = index;
		sprite.x = SpriteList[index].x;
		sprite.y = SpriteList[index].y;
		DrvST7789DrawSprite(&sprite);
	}

	/* 過去値初期化 */
	oldTemperature = ENV_NODATA;
	oldPressure = ENV_NODATA;
	oldHumidity = ENV_NODATA;
	oldCo2 = ENV_NODATA;
}

/*=== ログ画面背景描画関数 ===*/
static void aplDrawBackgroundLog(uint8_t env_id)
{
	sprite_queue_t sprite;
	uint8_t index;

	/* 固定スプライト表示 */
	for (index=SPRITE_Max; index<=SPRITE_Hour; index++) {
		sprite.sprite_id = index;
		sprite.x = SpriteList[index].x;
		sprite.y = SpriteList[index].y;
		DrvST7789DrawSprite(&sprite);
	}

	/* 横軸数値表示 */
	for (index=0; index<(sizeof(logNumTable) / sizeof(sprite_queue_t)); index++){
		DrvST7789DrawSprite((sprite_queue_t*)&logNumTable[index]);
	}

	/* アイコン・単位表示 */
	DrvST7789DrawSprite((sprite_queue_t*)&logIconTable[env_id]);
	DrvST7789DrawSprite((sprite_queue_t*)&logUnitTable[env_id]);

	/* 過去値初期化 */
	oldTime.minute = 0xFF;
	/* 最初に描画を起こすため */
	logMinute[0].time = 0xFF;
}

/*=== メイン画面周期描画関数 ===*/
static void aplDrawCyclicMain(void)
{
	float value;

	/* 現在気温取得 */
	value = DrvBME280GetTemperature();
	/* 気温変化時のみ描画更新 */
	if (value != oldTemperature) {
		aplDrawValue(value, envFromatTable[ENV_TEMP].digit, SIZE_L, 85, 17, 4, envFromatTable[ENV_TEMP].sign, envFromatTable[ENV_TEMP].fraction, SUPPRESS_ON);
		oldTemperature = value;
	}
	/* 現在湿度取得 */
	value = DrvBME280GetHumidity();
	/* 湿度変化時のみ描画更新 */
	if (value != oldHumidity) {
		aplDrawValue(value, envFromatTable[ENV_HUMI].digit, SIZE_L, 85, 77, 4, envFromatTable[ENV_HUMI].sign, envFromatTable[ENV_HUMI].fraction, SUPPRESS_ON);
		oldHumidity = value;
	}
	/* 現在気圧取得 */
	value = DrvBME280GetPressure();
	/* 気圧変化時のみ描画更新 */
	if (value != oldPressure) {
		aplDrawValue(value, envFromatTable[ENV_PRESS].digit, SIZE_L, 64, 137, 4, envFromatTable[ENV_PRESS].sign, envFromatTable[ENV_PRESS].fraction, SUPPRESS_ON);
		oldPressure = value;
	}
	/* 現在CO2濃度取得 */
	value = DrvMHZ19BGetCo2();
	/* CO2濃度変化時のみ描画更新 */
	if (value != oldCo2) {
		aplDrawValue(value, envFromatTable[ENV_CO2].digit, SIZE_L, 92, 197, 4, envFromatTable[ENV_CO2].sign, envFromatTable[ENV_CO2].fraction, SUPPRESS_ON);
		oldCo2 = value;
	}
}

/*=== ログ画面周期描画関数 ===*/
static void aplDrawCyclicLog(uint8_t env_id)
{
	uint8_t index;
	uint8_t log_old_time;
	sprite_queue_t sprite;

	log_old_time = logMinute[0].time;

	/* データ取得 */
	for (index=0; index<LOG_MINUTE_NUM; index++) {
		logMinute[index].value = AplLoggerGetMinuteEnv(env_id, index, &logMinute[index].time);
	}
	for (index=0; index<LOG_HOUR_NUM; index++) {
		logHour[index].value = AplLoggerGetHourEnv(env_id, index, &logHour[index].time);
	}

	/* データ更新有無判定 */
	if (logMinute[0].time != log_old_time) {
		/* 統計情報計算 */
		aplDrawCalcStatistics();
		/* 統計数値表示 */
		if (!isnanf(logStatistics.max)) {
			aplDrawValue(logStatistics.max, envFromatTable[env_id].digit, SIZE_S, statisticsDrawTable[env_id].x, statisticsDrawTable[env_id].y, 1, envFromatTable[env_id].sign, envFromatTable[env_id].fraction, SUPPRESS_ON);
		}
		if (!isnanf(logStatistics.ave)) {
			aplDrawValue(logStatistics.ave, envFromatTable[env_id].digit, SIZE_S, statisticsDrawTable[env_id].x, statisticsDrawTable[env_id].y + 18, 1, envFromatTable[env_id].sign, envFromatTable[env_id].fraction, SUPPRESS_ON);
		}
		if (!isnanf(logStatistics.min)) {
			aplDrawValue(logStatistics.min, envFromatTable[env_id].digit, SIZE_S, statisticsDrawTable[env_id].x, statisticsDrawTable[env_id].y + 36, 1, envFromatTable[env_id].sign, envFromatTable[env_id].fraction, SUPPRESS_ON);
		}
		/* 分グラフ表示 */
		for (index=0; index<LOG_MINUTE_NUM; index++) {
			/* 背景 */
			sprite.sprite_id = SPRITE_GraphMinuteBack;
			sprite.x = logMinute[index].time * 12;
			sprite.y = SpriteList[sprite.sprite_id].y;
			DrvST7789DrawSprite(&sprite);
			/* 平均値ライン */
			if (!isnanf(logStatistics.ave)) {
				sprite.sprite_id = SPRITE_GraphMinuteAverageLine;
				sprite.y = aplDrawCalcYpos(logStatistics.ave) + GRAPH_Y_TOP;
				DrvST7789DrawSprite(&sprite);
			}
			/* プロット */
			if (!isnanf(logMinute[index].value)) {
				sprite.sprite_id = SPRITE_plot_M_0 + index;
				sprite.x = sprite.x + 4;
				sprite.y = aplDrawCalcYpos(logMinute[index].value) + GRAPH_Y_TOP - 1;
				DrvST7789DrawSprite(&sprite);
			}
		}
		/* 時グラフ表示 */
		for (index=0; index<LOG_HOUR_NUM; index++) {
			/* 背景 */
			sprite.sprite_id = SPRITE_GraphHourBack;
			sprite.x = logHour[index].time * 10;
			sprite.y = SpriteList[sprite.sprite_id].y;
			DrvST7789DrawSprite(&sprite);
			/* 平均値ライン */
			if (!isnanf(logStatistics.ave)) {
				sprite.sprite_id = SPRITE_GraphHourAverageLine;
				sprite.y = aplDrawCalcYpos(logStatistics.ave) + GRAPH_Y_TOP + GRAPH_Y_INTERVAL;
				DrvST7789DrawSprite(&sprite);
			}
			/* プロット */
			if (!isnanf(logHour[index].value)) {
				sprite.sprite_id = SPRITE_plot_H_0 + index;
				sprite.x = sprite.x + 2;
				sprite.y = aplDrawCalcYpos(logHour[index].value) + GRAPH_Y_TOP - 1 + GRAPH_Y_INTERVAL;
				DrvST7789DrawSprite(&sprite);
			}
		}
	}
}

/*=== 数値描画関数 (value:値, digit:桁, size:文字サイズ, pos_x:横位置, pos_y:縦位置, interval:文字間隔, sing:符号有無, fraction:小数点有無, suppress:ゼロサプレス有無) ===*/
static void aplDrawValue(float value ,uint8_t digit, uint8_t size, uint8_t pos_x, uint8_t pos_y, uint8_t interval, uint8_t sign, uint8_t fraction, uint8_t suppress)
{
	sprite_queue_t sprite;
	uint8_t loop;
	uint8_t num;
	uint8_t total_digit;
	uint8_t minimum_draw_digit;
	uint8_t half_interval;
	uint8_t string_width;
	uint8_t dot_width;

	/* 符号と少数点以下の桁を足し合わせた合計桁数 */
	total_digit = digit + sign + fraction;

	/* 最小表示桁数(少数点表示含む) */
	minimum_draw_digit = 1 + fraction;

	/* 小数点表示用の文字間隔を計算(通常文字間隔の半分(四捨五入)) */
	half_interval = (uint8_t)((float)interval / 2.0f + 0.5f);

	/* 文字幅, 少数点幅 */
	string_width = SpriteList[size].w;
	dot_width = SpriteList[size + (SPRITE_dot_L - SPRITE_num0_L)].w;

	/* 符号の描画が必要か判定し、数値の符号は取り除く */
	if (sign == SIGN_ON) {
		if (value < 0.0f) {
			value = fabsf(value);	/* 数値の符号は取り除く */
		} else {
			sign = SIGN_OFF;		/* 正の値は符号無し */
		}
	}

	/* 小数点数を表示する場合は数値を10倍しておく */
	if (fraction == FRACTION_ON) {
		value = value * 10.0f;
	}

	/* 縦位置は固定 */
	sprite.y = pos_y;

	/* 最小桁から表示するために横位置を移動(文字幅×桁 + 文字間隔×桁 + 少数点幅) */
	sprite.x = pos_x + (total_digit - 1) * string_width + (total_digit - 1 - fraction) * interval + (half_interval * 2 + dot_width) * fraction;

	for (loop=0; loop<total_digit; loop++) {
		/* 一桁目を算出 */
		num = (uint16_t)value % 10;
		/* 表示文字判定 */
		if (minimum_draw_digit > loop) {
			/* 最小表示桁数の間は必ず表示 */
			sprite.sprite_id = num + size;
		} else if ((num > 0) || (value >= 10.0f) || (suppress == SUPPRESS_OFF)) {
			/* 0以外の数字または残り数値が10以上またはゼロサプレスOFFは表示 */
			sprite.sprite_id = num + size;
		} else if (sign == SIGN_ON) {
			/* 符号の表示 */
			sprite.sprite_id = size + (SPRITE_minus_L - SPRITE_num0_L);
			sign = SIGN_OFF;
		} else {
			/* 表示が無い場合は黒塗り */
			sprite.sprite_id = size + (SPRITE_numBlack_L - SPRITE_num0_L);
		}
		/* 描画要求 */
		DrvST7789DrawSprite(&sprite);
		/* 横位置をずらす */
		if (fraction == FRACTION_ON) {
			/* 小数点の表示 */
			sprite.x = sprite.x - dot_width - half_interval;
			sprite.sprite_id = size + (SPRITE_dot_L - SPRITE_num0_L);
			DrvST7789DrawSprite(&sprite);
			sprite.x = sprite.x - half_interval;
			fraction = FRACTION_OFF;
		} else {
			sprite.x = sprite.x - interval;
		}
		sprite.x = sprite.x - string_width;
		/* 数値を一桁減らす */
		value = value / 10.0f;
	}
}

/*=== 時計描画関数 ===*/
static void aplDrawClock(void)
{
	time_t now_time;

	/* 現在時刻取得 */
	now_time = DrvRtcGetNowTime();
	/* 時刻の変化時のみ描画更新 */
	if (now_time.minute != oldTime.minute) {
		aplDrawValue(now_time.hour, 2, SIZE_M, 173, 2, 1, SIGN_OFF, FRACTION_OFF, SUPPRESS_ON);
		aplDrawValue(now_time.minute, 2, SIZE_M, 209, 2, 1, SIGN_OFF, FRACTION_OFF, SUPPRESS_OFF);
		oldTime = now_time;
	}
}

/*=== 統計情報計算関数 ===*/
static void aplDrawCalcStatistics(void)
{
	uint8_t index;
	float sum;
	uint8_t sum_num;

	logStatistics.max = -FLT_MAX;
	logStatistics.min = FLT_MAX;
	sum = 0.0f;
	sum_num = 0;

	for (index=0; index<LOG_MINUTE_NUM; index++) {
		if (!isnanf(logMinute[index].value)) {
			if (logMinute[index].value > logStatistics.max) {
				logStatistics.max = logMinute[index].value;
			}
			if (logMinute[index].value < logStatistics.min) {
				logStatistics.min = logMinute[index].value;
			}
		}
	}
	for (index=0; index<LOG_HOUR_NUM; index++) {
		if (!isnanf(logHour[index].value)) {
			if (logHour[index].value > logStatistics.max) {
				logStatistics.max = logHour[index].value;
			}
			if (logHour[index].value < logStatistics.min) {
				logStatistics.min = logHour[index].value;
			}
			sum += logHour[index].value;
			sum_num++;
		}
	}

	if (logStatistics.max == -FLT_MAX) {
		logStatistics.max = ENV_NODATA;
	}

	if (logStatistics.min == FLT_MAX) {
		logStatistics.min = ENV_NODATA;
	}

	if (sum_num >= 2) {
		logStatistics.ave = sum / sum_num;
	} else {
		logStatistics.ave = ENV_NODATA;
	}
}

static uint8_t aplDrawCalcYpos(float value)
{
	float ratio;
	uint8_t pos_y;

	ratio = (value - logStatistics.min) / (logStatistics.max - logStatistics.min);
	pos_y = (1.0f - ratio) * (float)(GRAPH_Y_BOTTOM - GRAPH_Y_TOP);

	return pos_y;
}
