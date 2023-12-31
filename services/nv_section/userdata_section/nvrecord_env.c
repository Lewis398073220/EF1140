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
#if defined(NEW_NV_RECORD_ENABLED)
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "nvrecord_extension.h"
#include "nvrecord_env.h"
#include "hal_trace.h"

/** add by pang **/
#include "../../../apps/userapps/app_user.h"
/** end add **/

static struct nvrecord_env_t localSystemInfo;
static struct p_nvrecord_env_t p_localSystemInfo;//add by pang

void nvrecord_rebuild_system_env(struct nvrecord_env_t* pSystemEnv)
{
    memset((uint8_t *)pSystemEnv, 0, sizeof(struct nvrecord_env_t));
    
    pSystemEnv->media_language.language = NVRAM_ENV_MEDIA_LANGUAGE_DEFAULT;
    pSystemEnv->ibrt_mode.mode = NVRAM_ENV_TWS_MODE_DEFAULT;
    pSystemEnv->ibrt_mode.tws_connect_success = 0;
    pSystemEnv->factory_tester_status.status = NVRAM_ENV_FACTORY_TESTER_STATUS_DEFAULT;

    pSystemEnv->aiManagerInfo.voice_key_enable = false;
    pSystemEnv->aiManagerInfo.setedCurrentAi = 0;
    pSystemEnv->aiManagerInfo.aiStatusDisableFlag = 0;
    pSystemEnv->aiManagerInfo.amaAssistantEnableStatus = 1;

	/** add by pang **/
	pSystemEnv->demo_mode = 0;
	pSystemEnv->sleep_time = DEFAULT_SLEEP_TIME;
	pSystemEnv->vibrate_mode = 1;
	pSystemEnv->eq_mode = 0;
	pSystemEnv->anc_mode = NC_OFF;
	pSystemEnv->monitor_level = 20;//10
	pSystemEnv->focus_on = 0;
	pSystemEnv->sensor_enable = 1;
	
	pSystemEnv->iir_gain[0] = 0;
	pSystemEnv->iir_gain[1] = 0;
	pSystemEnv->iir_gain[2] = 0;
	pSystemEnv->iir_gain[3] = 0;
	pSystemEnv->iir_gain[4] = 0;
	pSystemEnv->iir_gain[5] = 0;

	pSystemEnv->touch_lock = 0;
	pSystemEnv->sidetone = 0;
	pSystemEnv->anc_table_value = ANC_HIGH;
	pSystemEnv->fota_flag = 0;
	pSystemEnv->multipoint = 1;
	pSystemEnv->talkmic_led = 1;
	pSystemEnv->auto_pwoff_time = DEFAULT_AUTO_PWOFF_TIME;//add by cai
	pSystemEnv->anc_toggle_mode = AncOn_AncOff_Awareness;//add by cai
	pSystemEnv->reserved1 = 0;//m by cai
	pSystemEnv->reserved2 = 0;//m by cai
	pSystemEnv->reserved3 = 0;
	/** end add **/

    localSystemInfo = *pSystemEnv;
}

int nv_record_env_get(struct nvrecord_env_t **nvrecord_env)
{
    if (NULL == nvrecord_env)
    {
        return -1;
    }

    if (NULL == nvrecord_extension_p)
    {   
        return -1;
    }

    localSystemInfo = nvrecord_extension_p->system_info;
    *nvrecord_env = &localSystemInfo;

    return 0;
}

int nv_record_env_set(struct nvrecord_env_t *nvrecord_env)
{
    if (NULL == nvrecord_extension_p)
    {   
        return -1;
    }

    uint32_t lock = nv_record_pre_write_operation();
    nvrecord_extension_p->system_info = *nvrecord_env;
    
    nv_record_update_runtime_userdata();
    nv_record_post_write_operation(lock); 
    return 0;
}

/** add by pang **/
void nvrecord_rebuild_system_env_user(struct p_nvrecord_env_t* pSystemEnv)
{
    memset((uint8_t *)pSystemEnv, 0, sizeof(struct nvrecord_env_t));
    
	pSystemEnv->anc_on_mode = ANC_HIGH_MODE;
	pSystemEnv->anc_status = ANC_OFF;

    p_localSystemInfo = *pSystemEnv;
}

int p_nv_record_env_get(struct p_nvrecord_env_t **nvrecord_env)
{
    if (NULL == nvrecord_env)
    {
        return -1;
    }

    if (NULL == nvrecord_extension_p)
    {   
        return -1;
    }

	p_localSystemInfo = nvrecord_extension_p->p_system_info;
	*nvrecord_env = &p_localSystemInfo;

    return 0;
}

int p_nv_record_env_set(struct p_nvrecord_env_t *nvrecord_env)
{
    if (NULL == nvrecord_extension_p)
    {   
        return -1;
    }

    uint32_t lock = nv_record_pre_write_operation();
    nvrecord_extension_p->p_system_info = *nvrecord_env;
    
    nv_record_update_runtime_userdata();
    nv_record_post_write_operation(lock); 
    return 0;
}

/** end add **/

void nv_record_update_ibrt_info(uint32_t newMode, bt_bdaddr_t *ibrtPeerAddr)
{
    if (NULL == nvrecord_extension_p)
    {   
        return;
    }
    
    uint32_t lock = nv_record_pre_write_operation();

    TRACE(2,"##%s,%d",__func__,newMode);
    nvrecord_extension_p->system_info.ibrt_mode.mode = newMode;
    memcpy(nvrecord_extension_p->system_info.ibrt_mode.record.bdAddr.address,
        ibrtPeerAddr->address, BTIF_BD_ADDR_SIZE);

    nv_record_update_runtime_userdata();
    nv_record_post_write_operation(lock);    
}


void nv_record_update_factory_tester_status(uint32_t status)
{
    if (NULL == nvrecord_extension_p)
    {   
        return;
    }

    uint32_t lock = nv_record_pre_write_operation();

    nvrecord_extension_p->system_info.factory_tester_status.status = status;

    nv_record_update_runtime_userdata();
    nv_record_post_write_operation(lock);   
}

int nv_record_env_init(void)
{
    nv_record_open(section_usrdata_ddbrecord);
    return 0;
}
#endif // #if defined(NEW_NV_RECORD_ENABLED)

