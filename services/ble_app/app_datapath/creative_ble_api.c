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
	
	if(data[0] != Start_ID)
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

bool APP_Functions_Call(uint8_t *data, uint32_t size)
{
	uint8_t opcode = (uint8_t)data[1];
		   
	switch(opcode)
	{
		default:
			TRACE(1,"%s: undefined CMD ID!\r\n",__func__);
			break;
	}
	
	return false;
}

