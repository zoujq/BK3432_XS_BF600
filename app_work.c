/**
 ****************************************************************************************
 *
 * @file app.c
 *
 * @brief Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration

#include <string.h>
//#include "rwapp_config.h"
#include "app_task.h"                // Application task Definition
#include "app.h"                     // Application Definition
#include "gap.h"                     // GAP Definition
#include "gapm_task.h"               // GAP Manager Task API
#include "gapc_task.h"               // GAP Controller Task API

#include "co_bt.h"                   // Common BT Definition
#include "co_math.h"                 // Common Maths Definition
#include "ke_timer.h"
#include "app_fff0.h"                 // Application security Definition
#include "app_ffa0.h"                 // Application security Definition
#include "app_ff50.h"              // fff0 Module Definition
#include "app_ff60.h"              // fff0 Module Definition
#include "app_ff70.h"              // fff0 Module Definition
#include "app_ff80.h"              // fff0 Module Definition
#include "app_ff90.h"              // fff0 Module Definition

#include "app_dis.h"                 // Device Information Service Application Definitions
#include "app_batt.h"                // Battery Application Definitions
#include "app_oads.h"                 // Application oads Definition
#if (NVDS_SUPPORT)
#include "nvds.h"                    // NVDS Definitions
#endif
#include "rf.h"
#include "uart.h"
#include "adc.h"
#include "gpio.h"
#include "wdt.h"
#include "rtc.h"
#include "prf_utils.h"

#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "atts.h"
#include "co_utils.h"
#include "ff50s.h"
#include "ff50s_task.h"
#include "uart.h"
#include "ff60s.h"
#include "ff60s_task.h"

#include "ff80s.h"
#include "ff80s_task.h"

#include "ff90s.h"
#include "ff90s_task.h"

#include "fee0s.h"
#include "fee0s_task.h"

#include "bass.h"
#include "bass_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */
 void set_ff50_ff52_0x2a9e_rd(uint8_t* buf);
 void set_ff50_ff51_0x2a9d_ind(uint8_t* buf);
 void set_ff60_ff62_0x2a9c_rd(uint8_t* buf);
 void set_ff60_ff61_0x2a9b_ind(uint8_t* buf);
 void set_bass_batt_0x2a19_rd(uint8_t batt);
 //uart----------------------------------------------------------------------------------
 /**
  * [xs_uart_send_data description]
  * @Author   zoujq                    904087449@qq.com
  * @DateTime 2020-12-07T13:36:30+0800
  * @param    buf                      [description]
  * @param    len                      [description]
  */
 void xs_uart_send_data(uint8_t *buf, uint8_t len)
 {
 	UART_PRINTF("uart_tx:");
 	for(uint8_t i=0; i<len; i++)
	{
		UART_PRINTF("%x ", buf[i]);
	}
	UART_PRINTF("\r\n");
 	uart_send_buff(buf,len);
 }
 /**
  * [xs_uart_received_isr description]
  * @Author   zoujq                    904087449@qq.com
  * @DateTime 2020-12-07T13:36:08+0800
  * @param    buf                      [description]
  * @param    len                      [description]
  */
 void xs_uart_received_isr(uint8_t *buf, uint8_t len)
 {
 	UART_PRINTF("uart_rx:");
 	for(uint8_t i=0; i<len; i++)
	{
		UART_PRINTF("%x ", buf[i]);
	}
	UART_PRINTF("\r\n");
	// xs_uart_send_data("55667788",8);
	
	if(buf[0]=='1')
	{
		extern void app_fff1_send_lvl(uint8_t* buf, uint8_t len);

		app_fff1_send_lvl("122",3);

	}
	else  if(buf[0]=='2')
	{

		set_ff50_ff52_0x2a9e_rd("6565");
		set_ff50_ff51_0x2a9d_ind("123456789123456");
	}
	else if(buf[0]=='6')
	{

		set_ff60_ff62_0x2a9c_rd("6565");
		set_ff60_ff61_0x2a9b_ind("123456789123456");
	}
	else if(buf[0]='8')
	{
		set_bass_batt_0x2a19_rd(58);
	}
 }

 //Weight Scale----------------------------------------------------------------------------------
 
 //Weight Scale Feature  0x2a9e
void set_ff50_ff52_0x2a9e_rd(uint8_t* buf)
 {
 	struct ff50s_env_tag* ff50s_env = PRF_ENV_GET(FF50S, ff50s);
 	memcpy(ff50s_env->ff52_value,buf,FF50_FF52_DATA_LEN);

 }
void set_ff50_ff51_0x2a9d_ind(uint8_t* buf)
{
	extern void app_ff51_send_lvl(uint8_t* buf, uint8_t len);

	app_ff51_send_lvl(buf,FF50_FF51_DATA_LEN);
}

//Body Composition------------------------------------------------------------------------------

 void set_ff60_ff62_0x2a9c_rd(uint8_t* buf)
 {
 	struct ff60s_env_tag* ff60s_env = PRF_ENV_GET(FF60S, ff60s);
 	memcpy(ff60s_env->ff62_value,buf,FF60_FF62_DATA_LEN);
 }
 void set_ff60_ff61_0x2a9b_ind(uint8_t* buf)
{
	extern void app_ff61_send_lvl(uint8_t* buf, uint8_t len);	
	app_ff61_send_lvl(buf,FF60_FF61_DATA_LEN);	
}
//User Data
void set_fee0_fee1_0x2a99_ntf(uint8_t* buf)
{
	extern void app_fee1_send_lvl(uint8_t* buf, uint8_t len);
	struct fee0s_env_tag* fee0s_env = PRF_ENV_GET(FEE0S, fee0s);
	
 	app_fee1_send_lvl(buf,FEE0_FEE1_DATA_LEN);
 	memcpy(fee0s_env->fee1_value,buf,FEE0_FEE1_DATA_LEN);
}

void fee0_fee1_0x2a99_cb(uint8_t* buf)
{
	UART_PRINTF("fee0_fee1_0x2a99_cb\r\n");
}
void set_fee0_fee2_0x2a85_rd(uint8_t* buf)
{
	struct fee0s_env_tag* fee0s_env = PRF_ENV_GET(FEE0S, fee0s);
 	memcpy(fee0s_env->fee2_value,buf,FEE0_FEE2_DATA_LEN);
}
void fee0_fee2_0x2a85_cb(uint8_t* buf)
{
	UART_PRINTF("fee0_fee2_0x2a99_cb\r\n");
}
void set_fee0_fee3_0x2a9f_ntf(uint8_t* buf)
{
	extern void app_fee3_send_lvl(uint8_t* buf, uint8_t len);
	struct fee0s_env_tag* fee0s_env = PRF_ENV_GET(FEE0S, fee0s);
	
 	app_fee1_send_lvl(buf,FEE0_FEE3_DATA_LEN);
 	memcpy(fee0s_env->fee3_value,buf,FEE0_FEE3_DATA_LEN);
}
void fee0_fee3_0x2a9f_cb(uint8_t* buf)
{
	UART_PRINTF("fee0_fee3_0x2a9f_cb\r\n");
}
void set_fee0_fee4_0x2a8c_rd(uint8_t* buf)
{
	struct fee0s_env_tag* fee0s_env = PRF_ENV_GET(FEE0S, fee0s);
 	memcpy(fee0s_env->fee4_value,buf,FEE0_FEE4_DATA_LEN);
}
void fee0_fee4_0x2a8c_cb(uint8_t* buf)
{
	UART_PRINTF("fee0_fee4_0x2a8c_cb\r\n");
}

void set_fee0_fee5_0x2a8e_rd(uint8_t* buf)
{
	struct fee0s_env_tag* fee0s_env = PRF_ENV_GET(FEE0S, fee0s);
 	memcpy(fee0s_env->fee5_value,buf,FEE0_FEE5_DATA_LEN);
}
void fee0_fee5_0x2a8e_cb(uint8_t* buf)
{
	UART_PRINTF("fee0_fee5_0x2a8e_cb\r\n");
}

void set_fee0_fee6_0x2a9a_rd(uint8_t* buf)
{
	struct fee0s_env_tag* fee0s_env = PRF_ENV_GET(FEE0S, fee0s);
 	memcpy(fee0s_env->fee6_value,buf,FEE0_FEE6_DATA_LEN);
}

//Current Time 
void set_ff80_ff83_0x2a9f_ntf(uint8_t* buf)
{
	extern void app_ff81_send_lvl(uint8_t* buf, uint8_t len);
	struct ff80s_env_tag* ff80s_env = PRF_ENV_GET(FF80S, ff80s);
	
 	app_ff81_send_lvl(buf,FF80_FF81_DATA_LEN);
 	memcpy(ff80s_env->ff81_value,buf,FF80_FF81_DATA_LEN);
}
void ff80_ff81_0x2a9f_cb(uint8_t* buf)
{
	UART_PRINTF("ff80_ff81_0x2a9f_cb\r\n");
	set_ff80_ff83_0x2a9f_ntf(buf);

}
// battery
void set_bass_batt_0x2a19_rd(uint8_t batt)
{
	struct bass_env_tag* bass_env = PRF_ENV_GET(BASS, bass);
 	bass_env->batt_lvl[0]=batt;
}