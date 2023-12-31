/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
////////////////////////////////////////////////////////////////
//

#ifndef __RESOURCES_H__
#define __RESOURCES_H__

typedef enum {
    AUD_ID_INVALID  = -1,
    AUD_ID_POWER_ON = 0x0,
    AUD_ID_POWER_OFF,
    AUD_ID_LANGUAGE_SWITCH,

    AUD_ID_NUM_0,
    AUD_ID_NUM_1,
    AUD_ID_NUM_2,
    AUD_ID_NUM_3,
    AUD_ID_NUM_4,
    AUD_ID_NUM_5,
    AUD_ID_NUM_6,
    AUD_ID_NUM_7,
    AUD_ID_NUM_8,
    AUD_ID_NUM_9,

    AUD_ID_BT_PAIR_ENABLE,
    AUD_ID_BT_PAIRING,
    AUD_ID_BT_PAIRING_SUC,
    AUD_ID_BT_PAIRING_FAIL,
    AUD_ID_BT_CALL_REFUSE,
    AUD_ID_BT_CALL_OVER,
    AUD_ID_BT_CALL_ANSWER,
    AUD_ID_BT_CALL_HUNG_UP,
    AUD_ID_BT_CALL_INCOMING_CALL,
    AUD_ID_BT_CALL_INCOMING_NUMBER,
    AUD_ID_BT_CHARGE_PLEASE,
    AUD_ID_BT_CHARGE_FINISH,
    AUD_ID_BT_CLEAR_SUCCESS,
    AUD_ID_BT_CLEAR_FAIL,
    AUD_ID_BT_CONNECTED,
    AUD_ID_BT_DIS_CONNECT,
    AUD_ID_BT_WARNING,
    AUDIO_ID_BT_ALEXA_START,
    AUDIO_ID_BT_ALEXA_STOP,
    AUDIO_ID_BT_GSOUND_MIC_OPEN,
    AUDIO_ID_BT_GSOUND_MIC_CLOSE,
    AUDIO_ID_BT_GSOUND_NC,
    AUDIO_ID_BT_MUTE,
    AUD_ID_RING_WARNING,
#if 1//def __INTERACTION__
    AUD_ID_BT_FINDME,
#endif
/** add by pang **/
    AUD_ID_BT_SHORT_1,
    AUD_ID_BT_VOL_MAX,
	AUD_ID_BT_FACTORY_RESET,
	AUD_ID_BT_ANC_ON,	
	AUD_ID_BT_ANC_OFF,
    AUD_ID_BT_MONITOR_ON,	
	AUD_ID_BT_LOST_OF_RANGE,
	AUD_ID_BT_GAMING_ON,
	AUD_ID_BT_GAMING_OFF,

	AUD_ID_BEEP_21,
	AUD_ID_BEEP_22,
	AUD_ID_SINGLE_BEEP,
	AUD_ID_DOUBLE_BEEP,
	AUD_ID_POWEROFF_LOWBATTERY,
	AUD_ID_DEMO_MODE,
	AUD_ID_CALLING_MUTE,
	AUD_ID_CALLING_UNMUTE,
/** end add **/
    MAX_RECORD_NUM,
    AUD_ID_ENUM_BOTTOM = 0x1001,
} AUD_ID_ENUM;

enum ENUM_RESOURCE_ID
{
    RES_ENGLISH_ID = 0xFF00,
    RES_CHINESE_ID = 0xFF01,

    MAX_RES_LANGUAGE_ID
};

typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;

void init_audio_resource(void* gResource);

UINT8* aud_get_reouce(AUD_ID_ENUM id, UINT32* leng, UINT16* type);

const char *aud_id2str(UINT16 aud_id);

extern UINT8 BIN_FILE[];

#endif//__RESOURCES_H__

