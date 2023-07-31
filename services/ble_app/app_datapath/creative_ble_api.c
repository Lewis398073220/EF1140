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
#include "string.h"
#include "bluetooth.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "apps.h"
#include "stdbool.h"
#include "rwapp_config.h"
#include "app_datapath_server.h"
#include "app_ble_cmd_handler.h"
#include "app_ble_custom_cmd.h"
#include "factory_section.h"
#include "app_tota.h"

#include <stdio.h>

//#include "../bt_if/inc/avrcp_api.h"
#include "../bt_app/Btapp.h"
#include "../tota/app_spp_tota.h"

#include "../../../apps/userapps/app_user.h"
#include "app_bt_stream.h"
#include "app.h"
#include "app_battery.h"
#include "../../../apps/anc/inc/app_anc.h"
#include "app_bt.h"
#include "creative_ble_api.h"
#include "bt_if.h"
#include "besbt.h"

static struct PACKET_STRUCTURE packet = {
	.startID = START_ID,
};
static uint16_t packetLen = 0;
static uint8_t protocol_port=0;

void APP_protocol_port(uint8_t port)
{
	protocol_port=port;
}

void APP_Send_Notify(uint8_t *data, uint32_t size)
{
    if(protocol_port==0)
		app_datapath_server_send_data_via_notification(data,size);
	else
		app_tota_send_data_via_spp(data, size);
}

bool APP_Api_Entry(uint8_t *data, uint32_t size)
{	
	uint16_t payload_lenth = 0;
	
	if(data[0] != START_ID)
	{
		TRACE(2,"%s: Start ID error! data[0]=%x\r\n ",__func__, data[0]);
		return false;
	}

	payload_lenth = (data[3] << 8)  | data[2];
	if(payload_lenth != size - 4)
	{
		TRACE(3,"%s: Payload Length error! data[2]=%x, data[3]=%x\r\n ",__func__, data[2], data[3]);
		return false;
	}

	if(!APP_Functions_Call(data, size))
	{
		TRACE(2,"%s: CMD ID error! data[1]=%x\r\n ",__func__, data[1]);
		return false;
	}
	
	TRACE(1,"%s: Test_Creative_API OK",__func__);
	return true;
}

uint32_t FW_Version = (MAJOR << 29)|(MINOR << 23)|(YR << 19)|(MONTH << 15)|(DAY << 10)|(HR << 5)|(QUARTER << 3)|PATCH;
uint8_t SN_String[10] = {'0','0','0','0','0','0','0','0','0','\0'};
void Get_Device_Information(uint8_t *data, uint32_t size)
{
	uint8_t OPTYPE = (uint8_t)data[4];
	uint16_t i = 0;
	uint8_t BT_name[BT_LOCAL_NAME_LEN] = {0};
	uint8_t *factory_bt_name_ptr = NULL;
	uint8_t BLE_name[BT_LOCAL_NAME_LEN] = {0};
	uint8_t *factory_ble_name_ptr = NULL;
	uint16_t valid_len =0;
	
	switch(OPTYPE)
	{
		case OPTYPE_GENERAL_DEVICE_INFORMATION_QUERY:
			TRACE(1,"%s: OPTYPE_GENERAL_DEVICE_INFORMATION_QUERY\r\n",__func__);
			packet.cmdID = CMDID_DEVICE_INFORMATION;
			packet.payloadLen = 0x11;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_GENERAL_DEVICE_INFORMATION_QUERY;
			packet.payload[1] = RESERVED;
			packet.payload[2] = RESERVED;
			packet.payload[3] = HARDWARE_REVISION;
			packet.payload[4] = 1;//Number of Firmware
			*((uint16_t *)(packet.payload + 5)) = VENDOR_ID;
			for(i = 7; i < packet.payloadLen; i++)
			{
				packet.payload[i] = RESERVED;
			}
			
			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;

		case OPTYPE_FIRMWARE_VERSION_QUERY:
			TRACE(1,"%s: OPTYPE_FIRMWARE_VERSION_QUERY\r\n",__func__);
			packet.cmdID = CMDID_DEVICE_INFORMATION;
			packet.payloadLen = 0x08;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_FIRMWARE_VERSION_QUERY;
			packet.payload[1] = data[5];//Firmware Index
			*((uint32_t *)(packet.payload + 2)) = FW_Version;
			packet.payload[6] = RESERVED;
			packet.payload[7] = RESERVED;

			TRACE(8,"FW_Version: %d.%d.%d%d%d.%d%d%d",MAJOR,MINOR,16+YR,MONTH,DAY,HR,QUARTER,PATCH);
			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;

		case OPTYPE_SERIAL_NUMBER_STRING_QUERY:
			TRACE(1,"%s: OPTYPE_SERIAL_NUMBER_STRING_QUERY\r\n",__func__);
			packet.cmdID = CMDID_DEVICE_INFORMATION;
			packet.payloadLen = sizeof(SN_String) + 1;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_SERIAL_NUMBER_STRING_QUERY;
			memcpy(packet.payload + 1, SN_String, sizeof(SN_String));

			TRACE(1,"SN: %s",SN_String);
			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;

		case OPTYPE_BLE_DEVICE_NAME_QUERY:
			TRACE(1,"%s: OPTYPE_BLE_DEVICE_NAME_QUERY\r\n",__func__);
			packet.cmdID = CMDID_DEVICE_INFORMATION;

			factory_ble_name_ptr = factory_section_get_ble_name();
			if(factory_ble_name_ptr != NULL){
				valid_len = (strlen((char*)factory_ble_name_ptr)> sizeof(BLE_name))?(sizeof(BLE_name)):strlen((char*)factory_ble_name_ptr);
				memcpy(BLE_name,factory_ble_name_ptr,valid_len);
			}
			TRACE(1,"BLE_name: %s",BLE_name);
			packet.payloadLen = valid_len + 2;//包含一个字节的空字符
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_BLE_DEVICE_NAME_QUERY;
			memcpy(packet.payload + 1, BLE_name, valid_len);
			packet.payload[packet.payloadLen-1] = '\0';//填充空字符

			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;
			
		case OPTYPE_CLASSIC_BLUETOOTH_DEVICE_QUERY:
			TRACE(1,"%s: OPTYPE_CLASSIC_BLUETOOTH_DEVICE_QUERY\r\n",__func__);
			packet.cmdID = CMDID_DEVICE_INFORMATION;
		
			factory_bt_name_ptr = factory_section_get_bt_name();
			if(factory_bt_name_ptr != NULL){
				valid_len = (strlen((char*)factory_bt_name_ptr)> sizeof(BT_name))?(sizeof(BT_name)):strlen((char*)factory_bt_name_ptr);
				memcpy(BT_name,factory_bt_name_ptr,valid_len);
			}
			TRACE(1,"BT_name: %s",BT_name);
			packet.payloadLen = valid_len + 2;//包含一个字节的空字符
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_CLASSIC_BLUETOOTH_DEVICE_QUERY;
			memcpy(packet.payload + 1, BT_name, valid_len);
			packet.payload[packet.payloadLen-1] = '\0';//填充空字符

			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;
			
		default:
			TRACE(2,"%s: undefined OPTYPE 0x%x!\r\n",__func__, OPTYPE);
			break;
	}
}

bool APP_Functions_Call(uint8_t *data, uint32_t size)
{
	uint8_t CMDID = (uint8_t)data[1];
		   
	switch(CMDID)
	{
		case CMDID_DEVICE_INFORMATION:
			TRACE(1,"%s: CMDID_DEVICE_INFORMATION\r\n",__func__);
			Get_Device_Information(data, size);
			return true;
			
		default:
			TRACE(1,"%s: undefined CMD ID!\r\n",__func__);
			break;
	}
	
	return false;
}

