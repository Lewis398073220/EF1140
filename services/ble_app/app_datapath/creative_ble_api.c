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

void Get_Max_Payload_Size(uint8_t *data, uint32_t size)
{
	packet.cmdID = CMDID_MAX_PAYLOAD_SIZE;
	packet.payloadLen = 0x02;
	packetLen = packet.payloadLen + 4;
	*((uint16_t *)packet.payload) = 150;//设备能够处理的最大负载大小

	APP_Send_Notify((uint8_t *)(&packet), packetLen);
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

			TRACE(8,"FW_Version: %d.%d.%d%02d%02d.%02d%d%d",MAJOR,MINOR,16+YR,MONTH,DAY,HR,QUARTER,PATCH);
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

void Get_Battery_Level(uint8_t *data, uint32_t size)
{
	uint8_t OPTYPE = (uint8_t)data[4];

	switch(OPTYPE)
	{
		case OPTYPE_BATTERY_LEVEL_RANGE_QUERY:
			TRACE(1,"%s: OPTYPE_BATTERY_LEVEL_RANGE_QUERY\r\n",__func__);
			packet.cmdID = CMDID_BATTERY_LEVEL;
			packet.payloadLen = 0x04;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_BATTERY_LEVEL_RANGE_QUERY;
			packet.payload[1] = 100;//Maximum Battery Level
			packet.payload[2] = 0;//Minimum Battery Level
			packet.payload[3] = 10;//Step value
		
			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;
			
		case OPTYPE_BATTERY_LEVEL_QUERY:
			TRACE(1,"%s: OPTYPE_BATTERY_LEVEL_QUERY\r\n",__func__);
			packet.cmdID = CMDID_BATTERY_LEVEL;
			packet.payloadLen = 0x07;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_BATTERY_LEVEL_QUERY;
			packet.payload[1] = 1;//Total number of devices queried
			*((uint32_t *)(packet.payload + 2)) = GENERIC_DEVICE_DEFAULT;
			packet.payload[6] = 10 * app_battery_current_level();

			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;
			
		default:
			TRACE(2,"%s: undefined OPTYPE 0x%x!\r\n",__func__, OPTYPE);
			break;
	}
}

void Get_Battery_Status(uint8_t *data, uint32_t size)
{
	uint8_t OPTYPE = (uint8_t)data[4];
	
	switch(OPTYPE)
	{
		case OPTYPE_BATTERY_STATUS_QUERY:
			TRACE(1,"%s: OPTYPE_BATTERY_STATUS_QUERY\r\n",__func__);
			packet.cmdID = CMDID_BATTERY_STATUS;
			packet.payloadLen = 0x0A;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_BATTERY_STATUS_QUERY;
			packet.payload[1] = 1;//Total number of devices queried
			*((uint32_t *)(packet.payload + 2)) = GENERIC_DEVICE_DEFAULT;
			*((uint32_t *)(packet.payload + 6)) = app_battery_is_charging()?CHARGING:(app_battery_is_batterylow()?LOW_BATTERY:NORMAL);

			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;
			
		default:
			TRACE(2,"%s: undefined OPTYPE 0x%x!\r\n",__func__, OPTYPE);
			break;
	}
}

void Get_Bluetooth_Support_Feature(uint8_t *data, uint32_t size)
{
	packet.cmdID = CMDID_BLUETOOTH_FEATURE_CONTROL;
	packet.payloadLen = 0x04;
	packetLen = packet.payloadLen + 4;
	*((uint32_t *)packet.payload) = BATTERY_STATUS_QUERY | GRAPHIC_EQUALIZER | NOISE_CONTROL | LOW_LATENCY_MODE | OTA_FIRMWARE_UPGRADE;

	APP_Send_Notify((uint8_t *)(&packet), packetLen);
}

void Set_Get_Noise_Control(uint8_t *data, uint32_t size)
{
	uint8_t OPTYPE = (uint8_t)data[4];
	enum ANC_STATUS anc_status = app_get_anc_status();

	switch(OPTYPE)
	{
		case OPTYPE_SUPPORTED_NOISE_CONTROL_MODE_QUERY:
			TRACE(1,"%s: OPTYPE_SUPPORTED_NOISE_CONTROL_MODE_QUERY\r\n",__func__);
			packet.cmdID = CMDID_NOISE_CONTROL;
			packet.payloadLen = 0x02;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_SUPPORTED_NOISE_CONTROL_MODE_QUERY;
			packet.payload[1] = OFF | ACTIVE_NOISE_REDUCTION | AMBIENT_MODE;
			
			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;

		case OPTYPE_NOISE_CONTROL_MODE_RANGE_QUERY:
			TRACE(1,"%s: OPTYPE_NOISE_CONTROL_MODE_RANGE_QUERY\r\n",__func__);
			packet.cmdID = CMDID_NOISE_CONTROL;
			packet.payloadLen = 0x04;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_NOISE_CONTROL_MODE_RANGE_QUERY;
			if(data[5] == ACTIVE_NOISE_REDUCTION)
			{
				packet.payload[1] = ACTIVE_NOISE_REDUCTION;
				packet.payload[2] = 4;//Noise Control Min level
				packet.payload[3] = 4;//Noise Control Max level
			}
			else if(data[5] == AMBIENT_MODE)
			{
				packet.payload[1] = AMBIENT_MODE;
				packet.payload[2] = 0;//Noise Control Min level
				packet.payload[3] = 4;//Noise Control Max level
			}
			
			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;

		case OPTYPE_NOISE_CONTROL_MODE_LEVEL_QUERY:
			TRACE(1,"%s: OPTYPE_NOISE_CONTROL_MODE_LEVEL_QUERY\r\n",__func__);
			packet.cmdID = CMDID_NOISE_CONTROL;
			packet.payloadLen = 0x03;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_NOISE_CONTROL_MODE_LEVEL_QUERY;
			if(data[5] == ACTIVE_NOISE_REDUCTION)
			{
				packet.payload[1] = ACTIVE_NOISE_REDUCTION;
				packet.payload[2] = 4;//Current Noise Control Level
			}
			else if(data[5] == AMBIENT_MODE)
			{
				packet.payload[1] = AMBIENT_MODE;
				packet.payload[2] = 4;//Noise Control Min level
			}
			
			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;
			
		case OPTYPE_CURRENT_NOISE_CONTROL_MODE_QUERY:
			TRACE(1,"%s: OPTYPE_CURRENT_NOISE_CONTROL_MODE_QUERY\r\n",__func__);
			packet.cmdID = CMDID_NOISE_CONTROL;
			packet.payloadLen = 0x02;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_CURRENT_NOISE_CONTROL_MODE_QUERY;
			if(anc_status == ANC_ON) packet.payload[1] = ACTIVE_NOISE_REDUCTION;
			else if(anc_status == AMBIENT_ON) packet.payload[1] = AMBIENT_MODE;
			else packet.payload[1] = OFF;
			
			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;

		case OPTYPE_SET_ACTIVE_NOISE_CONTROL_MODE:
			TRACE(1,"%s: OPTYPE_SET_ACTIVE_NOISE_CONTROL_MODE\r\n",__func__); 
			if(data[5] == ACTIVE_NOISE_REDUCTION) BLE_noise_control_mode_set(ANC_ON, current_anc_on_mode_get(), true);
			else if(data[5] == AMBIENT_MODE) BLE_noise_control_mode_set(AMBIENT_ON, current_anc_on_mode_get(), true);
			else BLE_noise_control_mode_set(ANC_OFF, current_anc_on_mode_get(), true);

			packet.cmdID = CMDID_ACKNOWLEDGE_FROM_DEVICE;
			packet.payloadLen = 0x02;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = CMDID_NOISE_CONTROL;
			packet.payload[1] = GENERAL_SUCCESS;

			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;
			
		default:
			TRACE(2,"%s: undefined OPTYPE 0x%x!\r\n",__func__, OPTYPE);
			break;
	}
}

void Noise_Control_Change_Notify(void)
{
	enum ANC_STATUS anc_status = app_get_anc_status();

	TRACE(1,"%s: OPTYPE_CURRENT_NOISE_CONTROL_MODE_QUERY\r\n",__func__);
	packet.cmdID = CMDID_NOISE_CONTROL;
	packet.payloadLen = 0x02;
	packetLen = packet.payloadLen + 4;
	packet.payload[0] = OPTYPE_CURRENT_NOISE_CONTROL_MODE_QUERY;
	if(anc_status == ANC_ON) packet.payload[1] = ACTIVE_NOISE_REDUCTION;
	else if(anc_status == AMBIENT_ON) packet.payload[1] = AMBIENT_MODE;
	else packet.payload[1] = OFF;
	
	APP_Send_Notify((uint8_t *)(&packet), packetLen);
}

void Set_Get_Low_Latency_Mode(uint8_t *data, uint32_t size)
{
	uint8_t OPTYPE = (uint8_t)data[4];
	enum ACKNOWLEDGEMENT_STATUS_TABLE OPRESULT = GENERAL_SUCCESS;
	
	switch(OPTYPE)
	{
		case OPTYPE_LOW_LATENCY_MODE_QUERY:
			TRACE(1,"%s: OPTYPE_LOW_LATENCY_MODE_QUERY\r\n",__func__);
			packet.cmdID = CMDID_LOW_LATENCY_MODE;
			packet.payloadLen = 0x02;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = OPTYPE_LOW_LATENCY_MODE_QUERY;
			packet.payload[1] = get_app_gaming_mode()? 1 : 0;
			
			APP_Send_Notify((uint8_t *)(&packet), packetLen);
			break;

		case OPTYPE_SET_LOW_LATENCY_MODE:
			TRACE(1,"%s: OPTYPE_SET_LOW_LATENCY_MODE\r\n",__func__);
			if((uint8_t)data[5] == 1)
			{
				if(BLE_set_gaming_mode(true, true)) OPRESULT = GENERAL_SUCCESS;
				else OPRESULT = GENERAL_FAILURE;
			} 
			else if((uint8_t)data[5] == 0)
			{
				if(BLE_set_gaming_mode(false, true)) OPRESULT = GENERAL_SUCCESS;
				else OPRESULT = GENERAL_FAILURE;
			}
			
			packet.cmdID = CMDID_ACKNOWLEDGE_FROM_DEVICE;
			packet.payloadLen = 0x04;
			packetLen = packet.payloadLen + 4;
			packet.payload[0] = CMDID_LOW_LATENCY_MODE;
			packet.payload[1] = OPRESULT;
			packet.payload[2] = OPTYPE_SET_LOW_LATENCY_MODE;
			packet.payload[3] = get_app_gaming_mode()? 1 : 0;

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
		case CMDID_MAX_PAYLOAD_SIZE:
			TRACE(1,"%s: CMDID_MAX_PAYLOAD_SIZE\r\n",__func__);
			Get_Max_Payload_Size(data, size);
			return true;
			
		case CMDID_DEVICE_INFORMATION:
			TRACE(1,"%s: CMDID_DEVICE_INFORMATION\r\n",__func__);
			Get_Device_Information(data, size);
			return true;

		case CMDID_BATTERY_LEVEL:
			TRACE(1,"%s: CMDID_BATTERY_LEVEL\r\n",__func__);
			Get_Battery_Level(data, size);
			return true;

		case CMDID_BATTERY_STATUS:
			TRACE(1,"%s: CMDID_BATTERY_STATUS\r\n",__func__);
			Get_Battery_Status(data, size);
			return true;

		case CMDID_BLUETOOTH_FEATURE_CONTROL:
			TRACE(1,"%s: CMDID_BLUETOOTH_FEATURE_CONTROL\r\n",__func__);
			Get_Bluetooth_Support_Feature(data, size);
			return true;

		case CMDID_NOISE_CONTROL:
			TRACE(1,"%s: CMDID_NOISE_CONTROL\r\n",__func__);
			Set_Get_Noise_Control(data, size);
			return true;
			
		case CMDID_LOW_LATENCY_MODE:
			TRACE(1,"%s: CMDID_LOW_LATENCY_MODE\r\n",__func__);
			Set_Get_Low_Latency_Mode(data, size);
			return true;
			
		default:
			TRACE(1,"%s: undefined CMD ID!\r\n",__func__);
			break;
	}
	
	return false;
}

