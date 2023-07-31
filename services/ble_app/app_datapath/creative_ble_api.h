/***************************************************************************
 *
 * Copyright CMT
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __CREATIVE_BLE_HOP_H__
#define __CREATIVE_BLE_HOP_H__

#ifdef __cplusplus
extern "C" {
#endif

struct PACKET_STRUCTURE {
	uint8_t startID;
	uint8_t cmdID;
	uint16_t payloadLen;
	uint8_t payload[100];
};

#define RESERVED 				  0x00

#define HARDWARE_REVISION 		  0x01
#define VENDOR_ID				  0x041E
enum FIRMWARE_VERSION_FORMAT {
	MAJOR = HARDWARE_REVISION & 0x07,//低3位
	MINOR = 2 & 0x3F,//低6位    3st release
	YR = 7 & 0x0F,//低4位    2023年
	MONTH = 8 & 0x0F,//低4位   8月
	DAY = 1 & 0x1F,//低5位   1号
	HR = 13 & 0x1F,//低5位   13点
	QUARTER = 0 & 0x03,//低2位   1st Quarter
	PATCH = 0 & 0x07,//低3位   patch 0
};

#define START_ID   0x6A

enum DEVICE_INFORMATION {
	CMDID_DEVICE_INFORMATION = 0x09,
	OPTYPE_GENERAL_DEVICE_INFORMATION_QUERY = 0x00,
	OPTYPE_FIRMWARE_VERSION_QUERY = 0x01,
	OPTYPE_RESERVED1 = 0x02,
	OPTYPE_SERIAL_NUMBER_STRING_QUERY = 0x03,
	OPTYPE_RESERVED2 = 0x04,
	OPTYPE_RESERVED3 = 0x05,
	OPTYPE_RESERVED4 = 0x06,
	OPTYPE_RESERVED5 = 0x07,
	OPTYPE_BLE_DEVICE_NAME_QUERY = 0x08,
	OPTYPE_CLASSIC_BLUETOOTH_DEVICE_QUERY = 0x09,
};

void APP_protocol_port(uint8_t port);
void APP_Send_Notify(uint8_t *data, uint32_t size);
bool APP_Api_Entry(uint8_t *data, uint32_t size);
bool APP_Functions_Call(uint8_t *data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __PHILIPS_BLE_API_H__

