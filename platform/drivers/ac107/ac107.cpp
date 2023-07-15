#include "plat_types.h"
#include "stdlib.h"
#include "cmsis_os.h"
#include "hal_trace.h" 
#include "hwtimer_list.h"
#include "hal_timer.h" 
#include "hal_iomux.h" 
#include "hal_gpio.h" 
#include "hal_i2c.h"
#include "string.h"
#include "btapp.h"
#include "app_status_ind.h"
#include "hal_codec.h"
#include "apps.h"

#include "ac107.h"

#if defined(__AC107_ADC__)

#define AC107_DEBUG_EN
 #ifdef AC107_DEBUG_EN
#define AC107_DEBUG(n,str, ...)	 	TRACE(n,str, ##__VA_ARGS__)
#else
#define AC107_DEBUG(n,str, ...)  	TRACE_DUMMY(n,str, ##__VA_ARGS__)
#endif

#define AC107_SLAVER_ADDR     0x36
#define wf_delay 2

//#define AC107_RST_PIN     	HAL_IOMUX_PIN_P0_1
//#define AC107_I2C_ID 		HAL_I2C_ID_1

#define i2c_gpio
#ifdef i2c_gpio
struct HAL_GPIO_I2C_CONFIG_T cfg_gpio_i2c_cmt1={
	HAL_GPIO_PIN_P2_2,HAL_GPIO_PIN_P2_3,100000
};
#else
static struct HAL_I2C_CONFIG_T _codec_i2c_cfg1;
#endif


void ac107_i2c_init(void)
{
//static bool init_flag=0;
//if(init_flag==0){
#ifdef i2c_gpio
	hal_gpio_i2c_open(&cfg_gpio_i2c_cmt1);
#else
	hal_iomux_set_i2c1();
	
	_codec_i2c_cfg1.mode = HAL_I2C_API_MODE_SIMPLE;
	_codec_i2c_cfg1.use_dma	= 0;
	_codec_i2c_cfg1.use_sync = 1;
	_codec_i2c_cfg1.speed = 100000;
	_codec_i2c_cfg1.as_master = 1;
	hal_i2c_open(HAL_I2C_ID_1, &_codec_i2c_cfg1);
#endif
//	init_flag=1;
//}
}

void ac107_delay_ms(uint16_t Nms)
{
    hal_sys_timer_delay(MS_TO_TICKS(Nms));
}


static uint32_t ac107_write(uint8_t uchDeviceId, const uint8_t uchWriteBytesArr[], uint16_t usWriteLen)
{
#ifdef i2c_gpio	
	return(hal_gpio_i2c_simple_send(uchDeviceId, uchWriteBytesArr, usWriteLen));
#else
	return (hal_i2c_simple_send(HAL_I2C_ID_1, uchDeviceId, uchWriteBytesArr, usWriteLen));
#endif
}

static uint32_t ac107_read(uint8_t uchDeviceId, const uint8_t uchCmddBytesArr[], uint16_t usCmddLen, uint8_t uchReadBytesArr[], uint16_t usMaxReadLen)
{
#ifdef i2c_gpio	
	return (hal_gpio_i2c_simple_recv(uchDeviceId, uchCmddBytesArr, usCmddLen, uchReadBytesArr, usMaxReadLen));	
#else
	return (hal_i2c_simple_recv(HAL_I2C_ID_1, uchDeviceId, uchCmddBytesArr, usCmddLen, uchReadBytesArr, usMaxReadLen));	
#endif
}

/*
static void ac107_rst_init(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin;
	
	pin.pin = AC107_RST_PIN;
	pin.function = HAL_IOMUX_FUNC_GPIO;
	pin.pull_sel = HAL_IOMUX_PIN_PULLUP_ENALBE;
	pin.volt = HAL_IOMUX_PIN_VOLTAGE_VIO;
	hal_iomux_init(&pin, 1);
	hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)AC107_RST_PIN, HAL_GPIO_DIR_OUT, 0);
}

static void hal_set_ac107_rst_high(void)
{
    hal_gpio_pin_set((enum HAL_GPIO_PIN_T)AC107_RST_PIN);
}

static void hal_set_ac107_rst_low(void)
{
    hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)AC107_RST_PIN);
}
*/

void ac107_hw_close(void)
{
#if defined(__USE_LDO_CTL__)
	hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)cfg_hw_ldo_ctl.pin);
#endif

#if defined(__ADC_3V3_CTR__) 
		hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)cfg_hw_adc_3_3v_control.pin);
#endif
}

void ac107_hw_open(void)
{
    //ac107_i2c_init();
#if defined(__USE_LDO_CTL__)
	hal_gpio_pin_set((enum HAL_GPIO_PIN_T)cfg_hw_ldo_ctl.pin);
#endif	

#if defined(__ADC_3V3_CTR__) 
	hal_gpio_pin_set((enum HAL_GPIO_PIN_T)cfg_hw_adc_3_3v_control.pin);
#endif
}


void ac107_hw_init(void)
{
	uint8_t write_cmd[2] = {0};
	uint8_t read_byte=0;
	AC107_DEBUG(0,"******%s",__func__); 
/*	
	ac107_i2c_init();
	
#if defined(__USE_LDO_CTL__)
	hal_gpio_pin_set((enum HAL_GPIO_PIN_T)cfg_hw_ldo_ctl.pin);
#endif
	ac107_delay_ms(300);
*/		
	//write_cmd[0] = CHIP_AUDIO_RST;//0x00
	//ac107_read(AC107_SLAVER_ADDR, write_cmd, 1, &read_byte, 1);
	//AC107_DEBUG(2,"******%s start: 0x%2x",__func__,read_byte); //read_byte=0x4B
	//ac107_delay_ms(wf_delay);

	write_cmd[0] = CHIP_AUDIO_RST;//0x00
	write_cmd[1] = 0x12;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2); //reset to default status
	ac107_delay_ms(wf_delay);	
	
	write_cmd[0] = PWR_CTRL1;//0x01
	write_cmd[1] = 0x80;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2); //0x01=0x80: VREF Enable
	ac107_delay_ms(wf_delay);

	write_cmd[0] = PWR_CTRL2;//0x02
	write_cmd[1] = 0x11;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2); //0x11=MICBIAS1&2 disable; 0x55=MICBIAS1&2 enable
	ac107_delay_ms(wf_delay);

	write_cmd[0] = MOD_CLK_EN;//0x21
	write_cmd[1] = 0x07;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2); //0x21=0x07: Module clock enable<I2S, ADC digital, ADC analog>
	ac107_delay_ms(wf_delay);

	write_cmd[0] = MOD_RST_CTRL;//0x22
	write_cmd[1] = 0x03;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);	/*0x22=0x03: Module reset de-asserted<I2S, ADC digital>*/
	ac107_delay_ms(wf_delay);

	write_cmd[0] = ANA_ADC1_CTRL5;//0xA4
	write_cmd[1] = 0x40;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2); //enable ADC1 analog part
	ac107_delay_ms(wf_delay);

	write_cmd[0] = ANA_ADC2_CTRL5;//0xA9
	write_cmd[1] = 0x40;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2); //enable ADC2 analog part
	ac107_delay_ms(wf_delay);

	write_cmd[0] = SYSCLK_CTRL;//0x20
	write_cmd[1] = 0x01;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2); //SYSCLK Enable
	ac107_delay_ms(wf_delay);

	write_cmd[0] = I2S_CTRL;//0x30
	write_cmd[1] = 0x15;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2); //SDO enable,0x10:slave /0xD0:master
	ac107_delay_ms(wf_delay);

	write_cmd[0] = I2S_BCLK_CTRL;//0x31
	write_cmd[1] = 0x00;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);
	ac107_delay_ms(wf_delay);

	write_cmd[0] = I2S_LRCK_CTRL1;//0x32
	write_cmd[1] = 0x00;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);
	ac107_delay_ms(wf_delay);

	write_cmd[0] = I2S_LRCK_CTRL2;//0x33
	write_cmd[1] = 0x1F;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2); //config LRCK period=32*BCLK
	ac107_delay_ms(wf_delay);

	write_cmd[0] = I2S_FMT_CTRL1;//0x34
	write_cmd[1] = 0x14;//15
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);//set I2S format: I2S Justified mode
	ac107_delay_ms(wf_delay);

	write_cmd[0] = I2S_FMT_CTRL2;//0x35
	write_cmd[1] = 0x75;//75=24bit 77=32bit
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2); //set Slot width=32-bit, Sample resolution=24-bit
	ac107_delay_ms(wf_delay);

	write_cmd[0] = I2S_TX_CTRL1;//0x38
	write_cmd[1] = 0x01;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);  //set I2S TX Channel (slot) number: 2
	ac107_delay_ms(wf_delay);

	write_cmd[0] = I2S_TX_CTRL2;//0x39
	write_cmd[1] = 0x03;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);  //I2S TX Channel 1 ~ Channel 2 (slot) enable
    ac107_delay_ms(wf_delay);	
	
	write_cmd[0] = I2S_TX_CHMP_CTRL1;//0x3C
	write_cmd[1] = 0x02;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);  //I2S TX mapping
	ac107_delay_ms(wf_delay);

	write_cmd[0] = ADC_SPRC;//0x60
	write_cmd[1] = 0x08;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);//set ADC Sample Rate: 48KHz
	ac107_delay_ms(wf_delay);

	write_cmd[0] = ADC_DIG_EN;//0x61
	write_cmd[1] = 0x07;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2); /*0x61=0x07: Digital part globe enable, ADCs digital part enable*/
	ac107_delay_ms(wf_delay);

	write_cmd[0] = HPF_EN;//0x66
	write_cmd[1] = 0x03;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2); //enable HPF
	ac107_delay_ms(wf_delay);

	write_cmd[0] = ANA_ADC1_CTRL3;//0xA2
	write_cmd[1] = 0x01;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);
	ac107_delay_ms(wf_delay);
	
	write_cmd[0] = ANA_ADC2_CTRL3;//0xA7
	write_cmd[1] = 0x01;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);
	ac107_delay_ms(wf_delay);

	write_cmd[0] = ADC1_DVOL_CTRL;//0x70
	write_cmd[1] = 0xAC;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);
	ac107_delay_ms(wf_delay);

	write_cmd[0] = ADC2_DVOL_CTRL;//0x71
	write_cmd[1] = 0xAC;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);
	
	ac107_delay_ms(10);//10

	write_cmd[0] = PWR_CTRL1;//0x01
	write_cmd[1] = 0xA0 ;
	ac107_write(AC107_SLAVER_ADDR, write_cmd, 2);//VREF Fast Start-up Disable
	ac107_delay_ms(wf_delay);

	write_cmd[0] = CHIP_AUDIO_RST;//0x00
	ac107_read(AC107_SLAVER_ADDR, write_cmd, 1, &read_byte, 1);
	AC107_DEBUG(2,"******%s end: 0x%2x",__func__,read_byte); //read_byte=0x4B
}

#endif
