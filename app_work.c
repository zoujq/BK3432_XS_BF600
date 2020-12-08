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
/*
 * DEFINES
 ****************************************************************************************
 */

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
 }

 //FFF0----------------------------------------------------------------------------------
 
 



 
 