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
#ifndef __TGT_HARDWARE__
#define __TGT_HARDWARE__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_key.h"
#include "hal_aud.h"

//config hwardware codec iir.
#define EQ_HW_DAC_IIR_LIST_NUM              1
#define EQ_HW_ADC_IIR_LIST_NUM              1
#define EQ_HW_IIR_LIST_NUM                  1
#define EQ_SW_IIR_LIST_NUM                  20
#define EQ_HW_FIR_LIST_NUM                  3

#ifdef __TENCENT_VOICE__
extern const char *BT_FIRMWARE_VERSION;
#endif
#define ANC_KEY_DETECTE HAL_IOMUX_PIN_P1_0

#if defined(__ADC_3V3_CTR__)
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_adc_3_3v_control;
#endif

#if defined(__CHARGE_CURRRENT__)
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_charge_current_control;
#endif

#if defined(__LDO_3V3_CTR__)
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pio_3_3v_control;
#endif

#if defined(__USE_AMP_MUTE_CTR__)
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pio_AMP_mute_control;
#endif

#if defined(__USE_3_5JACK_CTR__)
#define PIN_3_5JACK_DETECTE HAL_IOMUX_PIN_P1_4
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pio_3p5_jack_detecter;
#endif

#if defined(ANC_LED_PIN)
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_anc_led[2];
#endif

#if defined(__CHARGE_LED_ALONE__)
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP Cfg_charge_alone_led;
#endif

#if defined(__NTC_DETECT__)
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP Cfg_ntc_volt_ctr;
#endif

#if defined(__PWM_LED_CTL__)
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pwm_led[1];
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_io_led[1];
#endif


//pwl
#ifdef __BT_DEBUG_TPORTS__
#define CFG_HW_PLW_NUM (0)
#else
#define CFG_HW_PLW_NUM (2)
#endif
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pinmux_pwl[CFG_HW_PLW_NUM];

//adckey define
#define CFG_HW_ADCKEY_NUMBER 0
#define CFG_HW_ADCKEY_BASE 0
#define CFG_HW_ADCKEY_ADC_MAXVOLT 1000
#define CFG_HW_ADCKEY_ADC_MINVOLT 0
#define CFG_HW_ADCKEY_ADC_KEYVOLT_BASE 130
extern const uint16_t CFG_HW_ADCKEY_MAP_TABLE[CFG_HW_ADCKEY_NUMBER];

#define BTA_AV_CO_SBC_MAX_BITPOOL  52

#ifdef __BT_DEBUG_TPORTS__
#define CFG_HW_GPIOKEY_NUM (0)
#else
//gpiokey define
#define CFG_HW_GPIOKEY_NUM (3)
#endif
extern const struct HAL_KEY_GPIOKEY_CFG_T cfg_hw_gpio_key_cfg[CFG_HW_GPIOKEY_NUM];

// ANC function key
#define ANC_FUNCTION_KEY                    HAL_KEY_CODE_PWR

// ANC coefficient curve number
#define ANC_COEF_NUM                        (1)

#define ANC_TALK_THROUGH

#ifdef ANC_TALK_THROUGH
#define ANC_COEF_LIST_NUM                   (ANC_COEF_NUM + 12)
#else
#define ANC_COEF_LIST_NUM                   (ANC_COEF_NUM)
#endif

#define ANC_FF_MIC_CH_L                     AUD_CHANNEL_MAP_CH0
#define ANC_FF_MIC_CH_R                     AUD_CHANNEL_MAP_CH1
#define ANC_FB_MIC_CH_L                     AUD_CHANNEL_MAP_CH2
#define ANC_FB_MIC_CH_R                     AUD_CHANNEL_MAP_CH3

#define ANC_VMIC_CFG                        (AUD_VMIC_MAP_VMIC1)

// audio codec
#define CFG_HW_AUD_INPUT_PATH_NUM           9//4 //m by cai
extern const struct AUD_IO_PATH_CFG_T cfg_audio_input_path_cfg[CFG_HW_AUD_INPUT_PATH_NUM];

#define CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)

#define CFG_HW_AUD_SIDETONE_MIC_DEV         (AUD_CHANNEL_MAP_CH4)//m by cai for use call mic as sidetone mic
#define CFG_HW_AUD_SIDETONE_GAIN_DBVAL      (-30)//m by cai

//bt config
extern const char *BT_LOCAL_NAME;
extern const char *BLE_DEFAULT_NAME;
extern uint8_t ble_addr[6];
extern uint8_t bt_addr[6];

#define CODEC_SADC_VOL (12)//12

//extern const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY];
extern const struct CODEC_DAC_VOL_T codec_dac_vol[];


//range -12~+12
#define CFG_HW_AUD_EQ_NUM_BANDS (8)
extern const int8_t cfg_aud_eq_sbc_band_settings[CFG_HW_AUD_EQ_NUM_BANDS];
#define CFG_AUD_EQ_IIR_NUM_BANDS (4)

//battery info
#define APP_BATTERY_MIN_MV  (3330)//(3462)
#define APP_BATTERY_PD_MV   (3200)//(3400)

#define APP_BATTERY_MAX_MV (4200)

extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_enable_cfg;
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_detecter_cfg;
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_indicator_cfg;

#ifdef __cplusplus
}
#endif

#endif
