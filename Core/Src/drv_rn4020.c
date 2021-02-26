/*
 * drv_rn4020.c
 *
 *  Created on: Oct 10, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"
#include "drv_rn4020.h"

/********** Define **********/

/* 長いUUIDの文字数 */
#define LUUID_SIZE		(32)
/* 短いUUIDの文字数 */
#define SUUID_SIZE		(4)
/* 環境データキャラクタリスティック値の文字数 */
#define ENV_CHARA_VALUE_SIZE	(34)
/* キャラクタリスティックプロパティの文字数 */
#define CHARA_PROPERTY_SIZE	(2)
/* キャラクタリスティックサイズの文字数 */
#define CHARA_SIZE_SIZE		(2)
/* 受信バッファサイズ */
#define RX_BUFFER_SIZE	(512)
/* 送信バッファサイズ */
#define TX_BUFFER_SIZE	(64)
/* コマンドリトライ回数 */
#define COMMAND_RETRY	(3)
/* 初期化シーケンスのリトライ回数 */
#define INIT_SEQ_RETRY	(3)
/* コマンドの終端文字 */
#define COMMAND_RETURN_CODE	('\r')
/* 受信メッセージに終端文字 */
#define RX_RETURN_CODE		('\n')
/* ListServerServicesメッセージの最初の行の文字数 */
#define LS_FIRST_LINE_SIZE	(34)
/* ListServerServicesメッセージの本文の行の文字数 */
#define LS_BODY_LINE_SIZE	(47)
/* ListServerServicesメッセージの本文のLUUID開始位置 */
#define LS_LUUID_START		(2)
/* ListServerServicesメッセージの本文のSUUID開始位置 */
#define LS_SUUID_START		(35)


/* 受信バッファのインデックス計算マクロ */
#define RX_POS(index)	((index) % RX_BUFFER_SIZE)
/* 受信メッセージ情報生成用マクロ */
#define SET_RECEIVE_INFO(receive)	{receive, sizeof(receive)-1}
/* コマンド情報生成用マクロ */
#define SET_COMMAND_INFO(command)	{command, sizeof(command)-1}

/* BLE制御状態 */
enum BleState {
	BLE_INIT = 0,
	BLE_ADVERTISE,
	BLE_CONNECT,
	BLE_ERROR
};

/* 通信状態 */
enum ComState {
	COM_TX_WAIT = 0,
	COM_RX_WAIT,
	COM_RX_OK,
	COM_RX_ERR,
	COM_FAIL
};

/* 短いUUIDの取得状態 */
enum SUuidState {
	SUUID_NO_INIT = 0,		/* 未取得 */
	SUUID_COMPLETE			/* 取得済み */
};

/* キャラクタリスティックのリスト */
enum CharacteristicId {
	CHARA_SET_TIME = 0,
	CHARA_REQ_TIMING,
	CHARA_TEMP,
	CHARA_PRESS,
	CHARA_HUMI,
	CHARA_CO2,
	CHARACTERISTIC_NUM	/* キャラクタリスティックの数 */
};

/* 受信メッセージのリスト */
enum ReceiveIdList {
	RECEIVE_ID_AOK = 0,
	RECEIVE_ID_CMD,
	RECEIVE_ID_WV,
	RECEIVE_ID_CONNECTED,
	RECEIVE_ID_CONNECTION_END,
	RECEIVE_ID_ERR,
	RECEIVE_ID_END,
	RECEVIE_ID_SERVICE_UUID,
	RECEIVE_ID_NUM
};

/* コマンドのリスト */
enum CommandIdList {
	COMMAND_ID_SerializedName = 0,
	COMMAND_ID_FactoryDefaultAndReset,
	COMMAND_ID_FactoryDefault,
	COMMAND_ID_SetName,
	COMMAND_ID_SetFeatures,
	COMMAND_ID_SetServerServices,
	COMMAND_ID_Advertise,
	COMMAND_ID_Reboot,
	COMMAND_ID_ListServerServices,
	COMMAND_ID_WriteServerHandle,
	COMMAND_ID_SetPrivateCharacteristicUUID,
	COMMAND_ID_SetPrivateServiceUUID,
	COMMAND_ID_ClearPrivateService,
	COMMAND_ID_NUM
};

/* 初期化シーケンス */
enum InitSeq {
	INIT_SEQ_ListServerServices = 0,
	INIT_SEQ_FactoryDefaultAndReset,
	INIT_SEQ_FactoryDefault,
	INIT_SEQ_SerializedName,
	INIT_SEQ_SetName,
	INIT_SEQ_SetServerServices,
	INIT_SEQ_SetFeatures,
	INIT_SEQ_ClearPrivateService,
	INIT_SEQ_SetPrivateServiceUUID,
	INIT_SEQ_SetPrivateCharacteristicUUID,
	INIT_SEQ_Reboot,
	INIT_SEQ_SetCharacteristicValue,
	INIT_SEQ_Advertise,
	INIT_SEQ_Finish
};

/********** Type **********/

typedef enum BleState	ble_state_t;
typedef enum ComState	com_state_t;
typedef enum SUuidState	suuid_state_t;
typedef enum CharacteristicId chara_id_t;
typedef enum InitSeq	init_seq_t;
typedef enum ReceiveIdList	receive_id_t;
typedef enum CommandIdList	command_id_t;

typedef struct CharacteristicInfo {
	uint8_t uuid[LUUID_SIZE];
	uint8_t property[CHARA_PROPERTY_SIZE];
	uint8_t size[CHARA_SIZE_SIZE];
} chara_info_t;

typedef struct ReceiveInfo {
	const uint8_t* receive;
	uint8_t size;
} receive_info_t;

typedef struct CommandInfo {
	const uint8_t* command;
	uint8_t size;
} command_info_t;

/********** Constant **********/

const uint8_t uuid_service[LUUID_SIZE] = "456E764D6F6E42543231302020202020";	/* EnvMonBT210      */
const chara_info_t characteristic_info[CHARACTERISTIC_NUM] = {
	{"456E764D6F6E425454696D6520202020", "08", "07"},	/* CHARA_SET_TIME */	/* EnvMonBTTime    ,書き込み, 7byte */
	{"456E764D6F6E42545265717565737420", "08", "01"},	/* CHARA_REQ_TIMING */	/* EnvMonBTRequest ,書き込み, 1byte */
	{"456E764D6F6E425454656D7020202020", "02", "11"},	/* CHARA_TEMP */		/* EnvMonBTTemp    ,読み出し,17byte */
	{"456E764D6F6E42545072657373757265", "02", "11"},	/* CHARA_PRESS */		/* EnvMonBTPressure,読み出し,17byte */
	{"456E764D6F6E425448756D6964697479", "02", "11"},	/* CHARA_HUMI */		/* EnvMonBTHumidity,読み出し,17byte */
	{"456E764D6F6E4254434F322020202020", "02", "11"},	/* CHARA_CO2 */			/* EnvMonBTCO2     ,読み出し,17byte */
};

const uint8_t receive_AOK[] = "AOK\r\n";						/* RECEIVE_ID_AOK */
const uint8_t receive_CMD[] = "CMD\r\n";						/* RECEIVE_ID_CMD */
const uint8_t receive_WV[] = "WV,";								/* RECEIVE_ID_WV */
const uint8_t receive_Connected[] = "Connected\r\n";			/* RECEIVE_ID_CONNECTED */
const uint8_t receive_Connection_End[] = "Connection End\r\n";	/* RECEIVE_ID_CONNECTION_END */
const uint8_t receive_ERR[] = "ERR\r\n";						/* RECEIVE_ID_ERR */
const uint8_t receive_END[] = "END\r\n";						/* RECEIVE_ID_END */

const receive_info_t receive_info[RECEIVE_ID_NUM] = {
	SET_RECEIVE_INFO(receive_AOK),
	SET_RECEIVE_INFO(receive_CMD),
	SET_RECEIVE_INFO(receive_WV),
	SET_RECEIVE_INFO(receive_Connected),
	SET_RECEIVE_INFO(receive_Connection_End),
	SET_RECEIVE_INFO(receive_ERR),
	SET_RECEIVE_INFO(receive_END),
	{uuid_service, LUUID_SIZE}
};

const uint8_t command_separator[] = ",";

const uint8_t command_SerializedName[] = "S-,EnvMonBT";
const uint8_t command_FactoryDefaultAndReset[] = "SF,1\rR,1";
const uint8_t command_FactoryDefault[] = "SF,1";
const uint8_t command_SetName[] = "SN,EnvMonBT";
const uint8_t command_SetFeatures[] = "SR,00000000";
const uint8_t command_SetServerServices[] = "SS,00000001";
const uint8_t command_Advertise[] = "A";
const uint8_t command_Reboot[] = "R,1";
const uint8_t command_ListServerServices[] = "LS";
const uint8_t command_WriteServerHandle[] = "SHW,";
const uint8_t command_SetPrivateCharacteristicUUID[] = "PC,";
const uint8_t command_SetPrivateServiceUUID[] = "PS,";
const uint8_t command_ClearPrivateService[] = "PZ";

const command_info_t command_info[COMMAND_ID_NUM] = {
	SET_COMMAND_INFO(command_SerializedName),
	SET_COMMAND_INFO(command_FactoryDefaultAndReset),
	SET_COMMAND_INFO(command_FactoryDefault),
	SET_COMMAND_INFO(command_SetName),
	SET_COMMAND_INFO(command_SetFeatures),
	SET_COMMAND_INFO(command_SetServerServices),
	SET_COMMAND_INFO(command_Advertise),
	SET_COMMAND_INFO(command_Reboot),
	SET_COMMAND_INFO(command_ListServerServices),
	SET_COMMAND_INFO(command_WriteServerHandle),
	SET_COMMAND_INFO(command_SetPrivateCharacteristicUUID),
	SET_COMMAND_INFO(command_SetPrivateServiceUUID),
	SET_COMMAND_INFO(command_ClearPrivateService)
};

const uint8_t env_chara_init[ENV_CHARA_VALUE_SIZE] = "0000000000000000000000000000000000";

/********** Variable **********/

static UART_HandleTypeDef* huart_ptr;
static uint8_t rx_buffer[RX_BUFFER_SIZE];
static uint8_t tx_buffer[TX_BUFFER_SIZE];
static uint8_t suuid_list[CHARACTERISTIC_NUM][SUUID_SIZE];
static uint16_t rx_buffer_index;
static uint16_t rx_buffer_head;
static ble_state_t ble_state;
static com_state_t com_state;
static receive_id_t command_response_id;
static uint8_t com_state_error_count;
static suuid_state_t suuid_state;
static init_seq_t init_seq;
static uint8_t init_seq_retry_count;
static uint8_t characteristic_index;

/********** Function Prototype **********/

static void receive(void);
static uint16_t checkCarriageReturn(uint16_t rx_size);
static uint16_t checkReceiveMessage(uint16_t line_size);
static uint16_t checkListServerServicesMessage(void);
static chara_id_t searchCharacteristic(uint16_t start_index);
static receive_id_t searchRecieveMessage(void);
static bool_t matchReceiveMessage(uint16_t start_index, const uint8_t* text, uint8_t text_size);
static void transitionBleState(void);
static void processBleState(void);
static void transitionInitSequence(void);
static void processInitSequence(void);
static void sendCommand(command_id_t command_id);
static uint8_t setCharacteristicInitValue(void);
static uint8_t copyTxBuffer(const uint8_t* data, uint8_t size, uint8_t offset);

/********** Function **********/

/*=== 初期化関数 ===*/
void DrvRN4020Init(UART_HandleTypeDef* huart)
{
	huart_ptr = huart;

	rx_buffer_index = 0;

	ble_state = BLE_INIT;
	com_state = COM_TX_WAIT;
	command_response_id = RECEIVE_ID_NUM;
	com_state_error_count = 0;
	suuid_state = SUUID_NO_INIT;
	init_seq = INIT_SEQ_ListServerServices;
	init_seq_retry_count = 0;
	characteristic_index = 0;


	HAL_UART_Receive_DMA(huart_ptr, rx_buffer, RX_BUFFER_SIZE);

}

/*=== 周期関数 ===*/
void DrvRN4020Main(void)
{
	receive();
	transitionBleState();
	processBleState();
}

/*=== 受信処理関数 ===*/
static void receive(void)
{
	uint16_t rx_size;
	uint16_t line_size;
	uint16_t checked_size;

	/* 受信できたバッファ位置を計算 */
	rx_buffer_head = RX_BUFFER_SIZE - huart_ptr->hdmarx->Instance->CNDTR;
	/* 受信できたデータ長を計算 */
	if (rx_buffer_head < rx_buffer_index) {
		rx_size = RX_BUFFER_SIZE - rx_buffer_index + rx_buffer_head;
	} else {
		rx_size = rx_buffer_head - rx_buffer_index;
	}

	/* 受信メッセージが１行分受信できているか確認 */
	line_size = checkCarriageReturn(rx_size);
	
	if (line_size > 0) {
		/* 受信メッセージを確認 */
		checked_size = checkReceiveMessage(line_size);
		if (checked_size > 0) {
			/* 確認済み受信バッファ領域をクリアして、インデックスを進める */
			for (uint16_t index=0; index<checked_size; index++) {
				rx_buffer[RX_POS(rx_buffer_index + index)] = 0;
			}
			rx_buffer_index = RX_POS(rx_buffer_index + checked_size);
		}
	}
}

/*=== 受信メッセージの改行有無判定関数 ===*/
static uint16_t checkCarriageReturn(uint16_t rx_size)
{
	uint16_t line_size;

	line_size = 0;

	for (uint16_t index=0; index<rx_size; index++) {
		if (rx_buffer[RX_POS(rx_buffer_index + index)] == RX_RETURN_CODE) {
			line_size = index + 1;
			break;
		}
	}

	return line_size;
}

/*=== 受信メッセージ確認関数 ===*/
static uint16_t checkReceiveMessage(uint16_t line_size)
{
	uint16_t checked_size;
	receive_id_t receive_id;

	checked_size = 0;
	receive_id = searchRecieveMessage();

	switch (receive_id) {
	case RECEIVE_ID_AOK:
		/* コマンド送信に対して、正常値受信 */
		if (command_response_id == RECEIVE_ID_AOK && com_state == COM_RX_WAIT) {
			com_state = COM_RX_OK;
			com_state_error_count = 0;
			command_response_id = RECEIVE_ID_NUM;
		}
		checked_size = line_size;
		break;
	case RECEIVE_ID_CMD:
		/* コマンド送信に対して、正常値受信 */
		if (command_response_id == RECEIVE_ID_CMD && com_state == COM_RX_WAIT) {
			com_state = COM_RX_OK;
			com_state_error_count = 0;
			command_response_id = RECEIVE_ID_NUM;
		}
		checked_size = line_size;
		break;
	case RECEIVE_ID_WV:

		checked_size = line_size;
		break;
	case RECEIVE_ID_CONNECTED:

		checked_size = line_size;
		break;
	case RECEIVE_ID_CONNECTION_END:

		checked_size = line_size;
		break;
	case RECEIVE_ID_ERR:
		/* コマンド送信に対して、失敗を受信 */
		if (com_state == COM_RX_WAIT) {
			com_state_error_count++;
			if (com_state_error_count > COMMAND_RETRY) {
				com_state = COM_FAIL;
			} else {
				com_state = COM_RX_ERR;
			}
		}
		checked_size = line_size;
		break;
	case RECEIVE_ID_END:
		/* ListServerServicesMessageの応答でサービスIDが一致しない場合 */
		if (command_response_id == RECEVIE_ID_SERVICE_UUID && com_state == COM_RX_WAIT) {
			suuid_state = SUUID_NO_INIT;
			com_state = COM_RX_OK;
			com_state_error_count = 0;
			command_response_id = RECEIVE_ID_NUM;
		}
		checked_size = line_size;
		break;
	case RECEVIE_ID_SERVICE_UUID:
		/* ListServerServicesメッセージは複数行あるので全て受信できているか確認 */
		checked_size = checkListServerServicesMessage();
		if (command_response_id == RECEVIE_ID_SERVICE_UUID && checked_size > 0) {
			com_state = COM_RX_OK;
			com_state_error_count = 0;
			command_response_id = RECEIVE_ID_NUM;
		}
		break;
	default:
		/* 受信メッセージリストに無い文字列は読み捨てる */
		checked_size = line_size;
		break;
	}

	return checked_size;
}

/* ListServerServicesメッセージ確認関数 */
static uint16_t checkListServerServicesMessage(void)
{
	uint16_t message_size;
	bool_t is_end;

	message_size = 0;

	/* ListServerServicesメッセージを最後まで受信できているか確認 */
	for (uint16_t index=0; RX_POS(rx_buffer_index + index) != rx_buffer_head; index++) {
		if (rx_buffer[RX_POS(rx_buffer_index + index)] == RX_RETURN_CODE) {
			is_end = matchReceiveMessage(RX_POS(rx_buffer_index + index + 1), receive_END, sizeof(receive_END)-1);
			if (is_end == TRUE) {
				message_size = index + 1 + sizeof(receive_END) - 1;
				break;
			}
		}
	}

	if (message_size > 0) {
		uint16_t index;
		uint16_t remain_size;
		chara_id_t chara_id;
		uint8_t suuid_count;

		/* 最初の行はサービスSSIDなので読み飛ばす */
		index = RX_POS(rx_buffer_index + LS_FIRST_LINE_SIZE);
		remain_size = message_size - LS_FIRST_LINE_SIZE;

		suuid_count = 0;
		while (remain_size > LS_BODY_LINE_SIZE) {
			chara_id = searchCharacteristic(RX_POS(index + LS_LUUID_START));
			if (chara_id < CHARACTERISTIC_NUM) {
				suuid_count++;
				for (uint8_t copy_pos=0; copy_pos<SUUID_SIZE; copy_pos++) {
					suuid_list[chara_id][copy_pos] = rx_buffer[RX_POS(index + LS_SUUID_START + copy_pos)];
				}
			}

			index = RX_POS(index + LS_BODY_LINE_SIZE);
			remain_size -= LS_BODY_LINE_SIZE;
		}

		if (suuid_count == CHARACTERISTIC_NUM) {
			suuid_state = SUUID_COMPLETE;
		}
	}

	return message_size;
}

/*=== キャラクタリスティック探索関数 ===*/
static chara_id_t searchCharacteristic(uint16_t start_index)
{
	chara_id_t chara_id;
	bool_t is_match;

	for (chara_id=0; chara_id<CHARACTERISTIC_NUM; chara_id++) {
		is_match = TRUE;
		for (uint16_t index=0; index<LUUID_SIZE; index++) {
			if (rx_buffer[RX_POS(start_index + index)] != characteristic_info[chara_id].uuid[index]) {
				is_match = FALSE;
				break;
			}
		}
		if (is_match == TRUE) {
			break;
		}
	}

	return chara_id;
}

/*=== 受信メッセージ探索関数 ===*/
static receive_id_t searchRecieveMessage(void)
{
	receive_id_t receive_id;
	bool_t is_match;

	/* 受信メッセージのリストを探索 */
	for (receive_id=0; receive_id<RECEIVE_ID_NUM; receive_id++) {
		is_match = matchReceiveMessage(rx_buffer_index, receive_info[receive_id].receive, receive_info[receive_id].size);
		if (is_match == TRUE) {
			break;
		}
	}

	return receive_id;
}

/* 受信メッセージバッファと一致確認関数 */
static bool_t matchReceiveMessage(uint16_t start_index, const uint8_t* text, uint8_t text_size)
{
	bool_t is_match;

	is_match = TRUE;

	for (uint8_t index=0; index<text_size; index++) {
		if (rx_buffer[RX_POS(start_index + index)] != text[index]) {
			is_match = FALSE;
			break;
		}
	}

	return is_match;
}

/*=== BLE制御状態遷移関数 ===*/
static void transitionBleState(void)
{
	switch (ble_state) {
	case BLE_INIT:
		/* 初期化シーケンスが完了したらアドバタイズ状態へ遷移 */
		if (init_seq == INIT_SEQ_Finish) {
			ble_state = BLE_ADVERTISE;
		}
		break;
	case BLE_ADVERTISE:

		break;
	case BLE_CONNECT:

		break;
	default:
		/* 処理無し */
		break;
	}
}

/*=== BLE制御状態処理関数 ===*/
static void processBleState(void)
{
	switch (ble_state) {
	case BLE_INIT:
		/* 初期化シーケンス実行 */
		transitionInitSequence();
		processInitSequence();
		break;
	case BLE_ADVERTISE:

		break;
	case BLE_CONNECT:

		break;
	default:
		/* 処理無し */
		break;
	}
}

/*=== 初期化シーケンス遷移関数 ===*/
static void transitionInitSequence(void)
{
	switch (com_state) {
	case COM_RX_OK:
		/* 正常受信完了時 */
		switch (init_seq) {
		case INIT_SEQ_FactoryDefaultAndReset:
		case INIT_SEQ_FactoryDefault:
		case INIT_SEQ_SerializedName:
		case INIT_SEQ_SetName:
		case INIT_SEQ_SetServerServices:
		case INIT_SEQ_SetFeatures:
		case INIT_SEQ_ClearPrivateService:
		case INIT_SEQ_SetPrivateServiceUUID:
		case INIT_SEQ_Advertise:
			/* 次のシーケンスに遷移 */
			init_seq++;
			break;
		case INIT_SEQ_Reboot:
			/* 初期化シーケンスをやり直す */
			init_seq = INIT_SEQ_ListServerServices;
			break;
		case INIT_SEQ_ListServerServices:
			if (suuid_state == SUUID_COMPLETE) {
				/* SUUIDを正常取得できたらデータを初期化してBLE通信を開始 */
				init_seq = INIT_SEQ_SetCharacteristicValue;
			} else {
				/* SUUIDが取得できない場合は初期化実施 */
				init_seq = INIT_SEQ_FactoryDefaultAndReset;
				/* 初期化シーケンスのリトライ回数を超えたらエラー */
				init_seq_retry_count++;
				if (init_seq_retry_count > INIT_SEQ_RETRY) {
					ble_state = BLE_ERROR;
				}
			}
			break;
		case INIT_SEQ_SetPrivateCharacteristicUUID:
		case INIT_SEQ_SetCharacteristicValue:
			/* 次のキャラクタリスティックを対象に変更 */
			characteristic_index++;
			if (characteristic_index >= CHARACTERISTIC_NUM) {
				/* 全てのキャラクタリスティックが完了したら次のシーケンスへ遷移 */
				init_seq++;
				characteristic_index = 0;
			}
			break;
		case INIT_SEQ_Finish:
		default:
			/* 処理無し */
			break;
		}
		/* 受信データを確認したので、送信待ちへ遷移 */
		com_state = COM_TX_WAIT;
		break;
	case COM_RX_ERR:
		/* 正しく受信できなかった場合は同じシーケンスをリトライ */
		com_state = COM_TX_WAIT;
		characteristic_index = 0;
		break;
	case COM_TX_WAIT:
	case COM_RX_WAIT:
	case COM_FAIL:
	default:
		/* 処理無し */
		break;
	}
}

/*=== 初期化シーケンス処理関数 ===*/
static void processInitSequence(void)
{
	switch (com_state) {
	case COM_TX_WAIT:
	/* コマンド送信 */
		switch (init_seq) {
		case INIT_SEQ_ListServerServices:
			sendCommand(COMMAND_ID_ListServerServices);
			command_response_id = RECEVIE_ID_SERVICE_UUID;
			break;
		case INIT_SEQ_FactoryDefaultAndReset:
			sendCommand(COMMAND_ID_FactoryDefaultAndReset);
			command_response_id = RECEIVE_ID_CMD;
			break;
		case INIT_SEQ_FactoryDefault:
			sendCommand(COMMAND_ID_FactoryDefault);
			command_response_id = RECEIVE_ID_AOK;
			break;
		case INIT_SEQ_SerializedName:
			sendCommand(COMMAND_ID_SerializedName);
			command_response_id = RECEIVE_ID_AOK;
			break;
		case INIT_SEQ_SetName:
			sendCommand(COMMAND_ID_SetName);
			command_response_id = RECEIVE_ID_AOK;
			break;
		case INIT_SEQ_SetServerServices:
			sendCommand(COMMAND_ID_SetServerServices);
			command_response_id = RECEIVE_ID_AOK;
			break;
		case INIT_SEQ_SetFeatures:
			sendCommand(COMMAND_ID_SetFeatures);
			command_response_id = RECEIVE_ID_AOK;
			break;
		case INIT_SEQ_ClearPrivateService:
			sendCommand(COMMAND_ID_ClearPrivateService);
			command_response_id = RECEIVE_ID_AOK;
			break;
		case INIT_SEQ_SetPrivateServiceUUID:
			sendCommand(COMMAND_ID_SetPrivateServiceUUID);
			command_response_id = RECEIVE_ID_AOK;
			break;
		case INIT_SEQ_SetPrivateCharacteristicUUID:
			sendCommand(COMMAND_ID_SetPrivateCharacteristicUUID);
			command_response_id = RECEIVE_ID_AOK;
			break;
		case INIT_SEQ_Reboot:
			sendCommand(COMMAND_ID_Reboot);
			command_response_id = RECEIVE_ID_CMD;
			break;
		case INIT_SEQ_SetCharacteristicValue:
			if (characteristic_index < CHARA_TEMP) {
				characteristic_index = CHARA_TEMP;
			}
			sendCommand(COMMAND_ID_WriteServerHandle);
			command_response_id = RECEIVE_ID_AOK;
			break;
		case INIT_SEQ_Advertise:
			sendCommand(COMMAND_ID_Advertise);
			command_response_id = RECEIVE_ID_AOK;
			break;
		case INIT_SEQ_Finish:
		default:
			/* 処理無し */
			break;
		}
		break;
	case COM_RX_WAIT:
	case COM_RX_OK:
	case COM_RX_ERR:
	case COM_FAIL:
	default:
		/* 処理無し */
		break;
	}
}

/*=== コマンド送信関数 ===*/
static void sendCommand(command_id_t command_id)
{
	uint8_t buffer_index;

	buffer_index = 0;

	switch (command_id) {
	case COMMAND_ID_SerializedName:
	case COMMAND_ID_FactoryDefaultAndReset:
	case COMMAND_ID_FactoryDefault:
	case COMMAND_ID_SetName:
	case COMMAND_ID_SetFeatures:
	case COMMAND_ID_SetServerServices:
	case COMMAND_ID_Advertise:
	case COMMAND_ID_Reboot:
	case COMMAND_ID_ListServerServices:
	case COMMAND_ID_ClearPrivateService:
		/* 可変パラメータが無いコマンドはそのまま送信 */
		buffer_index = copyTxBuffer(command_info[command_id].command, command_info[command_id].size, buffer_index);
		break;
	case COMMAND_ID_WriteServerHandle:
		if (ble_state == BLE_INIT) {
			buffer_index = setCharacteristicInitValue();
		}
		break;
	case COMMAND_ID_SetPrivateCharacteristicUUID:
		buffer_index = copyTxBuffer(command_info[command_id].command, command_info[command_id].size, buffer_index);
		buffer_index = copyTxBuffer(characteristic_info[characteristic_index].uuid, LUUID_SIZE, buffer_index);
		buffer_index = copyTxBuffer(command_separator, 1, buffer_index);
		buffer_index = copyTxBuffer(characteristic_info[characteristic_index].property, CHARA_PROPERTY_SIZE, buffer_index);
		buffer_index = copyTxBuffer(command_separator, 1, buffer_index);
		buffer_index = copyTxBuffer(characteristic_info[characteristic_index].size, CHARA_SIZE_SIZE, buffer_index);
		break;
	case COMMAND_ID_SetPrivateServiceUUID:
		buffer_index = copyTxBuffer(command_info[command_id].command, command_info[command_id].size, buffer_index);
		buffer_index = copyTxBuffer(uuid_service, LUUID_SIZE, buffer_index);
		break;
	default:
		/* 処理無し */
		break;
	}

	/* 最後にリターンコードを付加 */
	tx_buffer[buffer_index] = COMMAND_RETURN_CODE;
	buffer_index++;

	/* データ送信 */
	huart_ptr->gState &= 0xFFFFFFFE;	/* Clear Tx Busy */
	HAL_UART_Transmit_DMA(huart_ptr, tx_buffer, buffer_index);
	com_state = COM_RX_WAIT;
}

/*=== 送信バッファデータコピー関数 ===*/
static uint8_t copyTxBuffer(const uint8_t* data, uint8_t size, uint8_t offset)
{
	uint8_t index;

	for (index=0; index<size; index++) {
		tx_buffer[offset + index] = data[index];
	}

	/* 次の空き領域を返す */
	return offset + index;
}

/*=== キャラクタリスティック値の初期値設定関数 ===*/
static uint8_t setCharacteristicInitValue(void)
{
	uint8_t buffer_index;

	buffer_index = 0;

	buffer_index = copyTxBuffer(command_info[COMMAND_ID_WriteServerHandle].command, command_info[COMMAND_ID_WriteServerHandle].size, buffer_index);
	buffer_index = copyTxBuffer(suuid_list[characteristic_index], SUUID_SIZE, buffer_index);
	buffer_index = copyTxBuffer(command_separator, 1, buffer_index);
	buffer_index = copyTxBuffer(env_chara_init, ENV_CHARA_VALUE_SIZE, buffer_index);

	return buffer_index;
}
