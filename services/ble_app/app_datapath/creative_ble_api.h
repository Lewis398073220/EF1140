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

#define Start_ID   0x6A

void APP_protocol_port(uint8_t port);
void APP_Send_Notify(uint8_t *data, uint32_t size);
bool APP_Api_Entry(uint8_t *data, uint32_t size);
bool APP_Functions_Call(uint8_t *data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __PHILIPS_BLE_API_H__

