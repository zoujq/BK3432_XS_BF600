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
#include "uart2.h"       // uart definition

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

#include "fff0s.h"
#include "fff0s_task.h"

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
void set_bass_batt_0x2a19_rd(uint8_t batt);
extern void app_fff1_send_lvl(uint8_t* buf, uint8_t len);
void xs_uart2_send_data(uint8_t *buf, uint8_t len);
unsigned short day_of_year(unsigned short y,unsigned short m,unsigned short d);
void set_fee0_fee2_0x2a85_rd(uint8_t* buf);
void set_fee0_fee3_0x2a9f_ntf(uint8_t* buf);
void set_fee0_fee4_0x2a8c_rd(uint8_t* buf);
void set_fee0_fee5_0x2a8e_rd(uint8_t* buf);
void send_user_list();

char g_user_num=0;
char g_user_list[8][14]=
{
	{0x00,0x01,0xFF,0xFF,0xFF,0xC7,0x07,0x0A,0x01,0xA5,0x01,0x03,0,0},
	{0x00,0x02,0xFF,0xFF,0xFF,0xC8,0x07,0x0A,0x01,0xA1,0x01,0x03,0,0},
	{0x00,0x03,0xFF,0xFF,0xFF,0xC9,0x07,0x0A,0x01,0xA5,0x00,0x03,0,0},
	{0x00,0x04,0xFF,0xFF,0xFF,0xCA,0x07,0x0A,0x01,0xA2,0x01,0x03,0,0},
	{0x00,0x05,0xFF,0xFF,0xFF,0xCB,0x07,0x0A,0x01,0xA5,0x00,0x03,0,0},
	{0x00,0x06,0xFF,0xFF,0xFF,0xCC,0x07,0x0A,0x01,0xA3,0x01,0x03,0,0},
	{0x00,0x07,0xFF,0xFF,0xFF,0xCD,0x07,0x0A,0x01,0xA5,0x00,0x03,0,0},
	{0x00,0x08,0xFF,0xFF,0xFF,0xCE,0x07,0x0A,0x01,0xA4,0x01,0x03,0,0}
};
char user_select[3]={0};
//ble state
#define BLE_STATE_PIN 0x07

void xs_user_task()
{
	static int t=0;
	if(t++>50)
	{
		UART_PRINTF("xs_user_task\n");
		t=0;
	}

}
void init_ble_state()
{
	gpio_config(BLE_STATE_PIN, OUTPUT, PULL_NONE);
	gpio_set(BLE_STATE_PIN, 1);

	UART_PRINTF("init_ble_state:1\n");

}
void set_ble_state(char s)
{
	gpio_set(BLE_STATE_PIN, s);
}
//uart----------------------------------------------------------------------------------

 void xs_uart_send_data(uint8_t *buf, uint8_t len)
 {
 	UART_PRINTF("uart_tx:");
 	for(uint8_t i=0; i<len; i++)
	{
		UART_PRINTF("%x ", buf[i]);
	}
	UART_PRINTF("\r\n");
 	uart_write(buf,len,NULL,NULL);
 }

 void xs_uart_received_isr(uint8_t *buf, uint8_t len)
 {
 	UART_PRINTF("uart_rx:");
 	for(uint8_t i=0; i<len; i++)
	{
		UART_PRINTF("%02x ", buf[i]);
	}
	UART_PRINTF("\r\n");
	// xs_uart_send_data("55667788",8);
	if(buf[0]=='0')
	{

	}
	else if(buf[0]=='1')
	{
		
		char user_1[12]={0};
		user_1[0]=0x02;
		UART_PRINTF("buf[0]=='1'\r\n");

		app_fff1_send_lvl(user_1,FFF0_FFF1_DATA_LEN);

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
	else if(buf[0]=='8')
	{
		set_bass_batt_0x2a19_rd(58);
	}
	else if(buf[0]=='9')
	{
		uint8_t b[19]={0};
		uint8_t b1[4]={0};
		uint8_t b2[4]={0};
		uint8_t b4[1]={0};
		uint8_t b5[1]={0};
		b[0]=0x20;
		b[1]=0x01;
		b[2]=0x01;
		b[3]=0x01;

		set_fee0_fee3_0x2a9f_ntf(b);

		// b1[0]=0;
		// b1[1]=0;
		// b1[2]=0;
		// b1[3]=0;
		// set_fee0_fee1_0x2a99_ntf(b1);

		b2[0]=0xD0;
		b2[1]=0x07;
		b2[2]=5;
		b2[3]=6;
		set_fee0_fee2_0x2a85_rd(b2);
		b4[0]=1;
		set_fee0_fee4_0x2a8c_rd(b4);
		b5[0]=185;
		set_fee0_fee5_0x2a8e_rd(b5);



	}
	else if(buf[0]=='A')
	{
		uint8_t b[19]={0};
		b[0]=0x20;
		b[1]=0x02;
		b[2]=0x01;

		set_fee0_fee3_0x2a9f_ntf(b);
	}
	else if(buf[0]=='B')
	{
		uint8_t b[19]={0};
		b[0]=0x20;
		b[1]=0x02;
		b[2]=0x05;
		b[3]=0x00;

		set_fee0_fee3_0x2a9f_ntf(b);

	}
 }
 //uart2_write("123456",6,0,0);
 void xs_uart2_send_data(uint8_t *buf, uint8_t len)
 {
 	UART_PRINTF("uart2_tx:");
 	for(uint8_t i=0; i<len; i++)
	{
		UART_PRINTF("%02X ", buf[i]);
	}
	UART_PRINTF("\r\n");
 	uart2_write(buf,len,NULL,NULL);
 }
 void xs_uart2_received_isr(uint8_t *buf, uint8_t len)
 {
 	UART_PRINTF("uart2_rx:");
 	for(uint8_t i=0; i<len; i++)
	{
		UART_PRINTF("%02X ", buf[i]);
	}
	UART_PRINTF("\r\n");

	if(buf[0]==0xFF && buf[1]==0xA5)
	{
		if(buf[6]==0xBA)
		{
			struct bass_env_tag* bass_env = PRF_ENV_GET(BASS, bass);
 			bass_env->batt_lvl[0]=buf[2];
		}
		else if(buf[6]==0xE0)
		{
			g_user_list[g_user_num][10]=buf[2];
			g_user_list[g_user_num][6]=0x07;
			g_user_list[g_user_num][5]=buf[3];
			g_user_list[g_user_num][7]=buf[4];
			g_user_list[g_user_num][8]=buf[5];

		}
		else if(buf[6]==0xE1)
		{
			g_user_list[g_user_num][9]=buf[2];
			g_user_list[g_user_num][11]=buf[5];
		}
		else if(buf[6]==0xE4)
		{
			g_user_list[g_user_num][12]=buf[2];
			g_user_list[g_user_num][13]=buf[3];

		}
		else if(buf[6]==0xE5)
		{
			g_user_list[g_user_num][1]=buf[2];
			g_user_num++;

		}
		else if(buf[6]==0xF0)
		{
			send_user_list();
		}

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
	UART_PRINTF("set_ff50_ff51_0x2a9d_ind\r\n");
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
	UART_PRINTF("set_ff60_ff61_0x2a9b_ind\r\n");
}
//User Data
void set_fee0_fee1_0x2a99_ntf(uint8_t* buf)
{
	extern void app_fee1_send_lvl(uint8_t* buf, uint8_t len);
	struct fee0s_env_tag* fee0s_env = PRF_ENV_GET(FEE0S, fee0s);
	
 	app_fee1_send_lvl(buf,FEE0_FEE1_DATA_LEN);
 	memcpy(fee0s_env->fee1_value,buf,FEE0_FEE1_DATA_LEN);
 	UART_PRINTF("set_fee0_fee1_0x2a99_ntf\r\n");
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
	
 	app_fee3_send_lvl(buf,FEE0_FEE3_DATA_LEN);
 	memcpy(fee0s_env->fee3_value,buf,FEE0_FEE3_DATA_LEN);
 	UART_PRINTF("set_fee0_fee3_0x2a9f_ntf\r\n");
}
void fee0_fee3_0x2a9f_cb(uint8_t* buf)
{
	UART_PRINTF("fee0_fee3_0x2a9f_cb\r\n");
	//creat user 0x01 00 00
	unsigned char buff[8]={0};	
	if(buf[0]=0x01)
	{
		g_user_num=0;

		buff[0]=0xA5;
		buff[1]=0x23;
		buff[2]=0;
		buff[3]=0;
		buff[4]=0;
		buff[5]=0;
		buff[6]=0;
		buff[7]=buff[1]+buff[2]+buff[3]+buff[4]+buff[5]+buff[6];
		xs_uart2_send_data(buff,8);//send time	
	}
	
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
void set_ff80_ff81_0x2a9b_ntf(uint8_t* buf)
{
	extern void app_ff81_send_lvl(uint8_t* buf, uint8_t len);
	struct ff80s_env_tag* ff80s_env = PRF_ENV_GET(FF80S, ff80s);
	
 	app_ff81_send_lvl(buf,FF80_FF81_DATA_LEN);
 	memcpy(ff80s_env->ff81_value,buf,FF80_FF81_DATA_LEN);
}
void ff80_ff81_0x2a9b_cb(uint8_t* buf)
{
	UART_PRINTF("ff80_ff81_0x2a9b_cb\r\n");
	// set_ff80_ff81_0x2a9b_ntf(buf);
	// set_bass_batt_0x2a19_rd(80);
	
	unsigned char buff[8]={0};	
	int w = 0;
	int y = buf[0]+buf[1]<<8,m = buf[2],d = buf[3];
	unsigned short days=day_of_year(y,m,d);
	
	if (m==1 || m==2)
	{
		m = (m==1?13:14);
		y--;
	}
	
	w = (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400+1)%7;
	w=(w==6)?0:w+1;

	buff[0]=0xA5;
	buff[1]=0x30;
	buff[2]=buf[0];
	buff[3]=days>>8;
	buff[4]=days&0xff;
	buff[5]=0;
	buff[6]=0;
	buff[7]=buff[1]+buff[2]+buff[3]+buff[4]+buff[5]+buff[6];
	xs_uart2_send_data(buff,8);//send date

	buff[0]=0xA5;
	buff[1]=0x31;
	buff[2]=buf[4];
	buff[3]=buf[5];
	buff[4]=buf[6];
	buff[5]=0;
	buff[6]=0;
	buff[7]=buff[1]+buff[2]+buff[3]+buff[4]+buff[5]+buff[6];
	xs_uart2_send_data(buff,8);//send time

}
// battery
void set_bass_batt_0x2a19_rd(uint8_t batt)
{
	struct bass_env_tag* bass_env = PRF_ENV_GET(BASS, bass);
 	bass_env->batt_lvl[0]=batt;
}
//user list
void fff0_fff1_cb(uint8_t* buf)
{
	static char index=0;
	if(buf[0]==0)
	{
		unsigned char buff[8]={0};	
		buff[0]=0xA5;
		buff[1]=0x23;
		buff[2]=0;
		buff[3]=0;
		buff[4]=0;
		buff[5]=0;
		buff[6]=0;
		buff[7]=buff[1]+buff[2]+buff[3]+buff[4]+buff[5]+buff[6];
		xs_uart2_send_data(buff,8);//
	}
	UART_PRINTF("fff0_fff1_cb\r\n");


}
void send_user_list()
{
	if(g_user_num>0)
	{
		int i=0;
		char user_finish_flag[1]={1};
		for(i=0;i<g_user_num;i++)
		{		
			app_fff1_send_lvl(g_user_list[i],FFF0_FFF1_DATA_LEN);
		}
		app_fff1_send_lvl(user_finish_flag,1);
	}
	else
	{
		char no_user_flag[1]={2};
		app_fff1_send_lvl(no_user_flag,1);
	}
}

void fff0_fff2_cb(uint8_t* buf)
{
	UART_PRINTF("fff0_fff2_cb\r\n");
}
void fff0_fff3_cb(uint8_t* buf)
{
	UART_PRINTF("fff0_fff3_cb\r\n");
	user_select[0]=buf[1];
	user_select[1]=buf[2];
	user_select[2]=buf[3];

	//for(int i=0;i<u)
}
void fff0_fff4_cb(uint8_t* buf)
{
	UART_PRINTF("fff0_fff4_cb\r\n");
}
void fff0_fff5_cb(uint8_t* buf)
{
	UART_PRINTF("fff0_fff5_cb\r\n");
}
//function
unsigned short day_of_year(unsigned short y,unsigned short m,unsigned short d)
{
	unsigned short k,leap,s;
	unsigned short days[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	leap = (y%4==0&&y%100!=0)||(y%400==0);
	s = d;
	for(k=1;k<m;k++)
	{
		s+=days[k];
	}
	if(leap==1&&m>2)
		s+=1;
	return s;
}
