/**
 ****************************************************************************************
 *
 * @file app_ff80.c
 *
 * @brief ff80 Application Module entry point
 *
 * @auth  gang.cheng
 *
 * @date  2016.05.31
 *
 * Copyright (C) Beken 2009-2016
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

#include "rwip_config.h"     // SW configuration

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <string.h>
#include "app_ff80.h"              // Battery Application Module Definitions
#include "app.h"                    // Application Definitions
#include "app_task.h"             // application task definitions
#include "ff80s_task.h"           // health thermometer functions
#include "co_bt.h"
#include "prf_types.h"             // Profile common types definition
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "ff80s.h"
#include "ke_timer.h"
#include "uart.h"



/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// ff80 Application Module Environment Structure
struct app_ff80_env_tag app_ff80_env;

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */


void app_ff80_init(void)
{

	// Reset the environment
	memset(&app_ff80_env, 0, sizeof(struct app_ff80_env_tag));
}

void app_ff80_add_ff80s(void)
{

	struct ff80s_db_cfg *db_cfg;

	struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
	                                        TASK_GAPM, TASK_APP,
	                                        gapm_profile_task_add_cmd, sizeof(struct ff80s_db_cfg));
	// Fill message
	req->operation = GAPM_PROFILE_TASK_ADD;
	req->sec_lvl =   0;
	req->prf_task_id = TASK_ID_FF80S;
	req->app_task = TASK_APP;
	req->start_hdl = 0; //req->start_hdl = 0; dynamically allocated


	// Set parameters
	db_cfg = (struct ff80s_db_cfg* ) req->param;

	// Sending of notifications is supported
	db_cfg->features = FF80_FF81_LVL_NTF_SUP;
	// Send the message
	ke_msg_send(req);
}


void app_ff81_send_lvl(uint8_t* buf, uint8_t len)
{
	// Allocate the message
	struct ff80s_ff81_level_upd_req * req = KE_MSG_ALLOC(FF80S_FF81_LEVEL_UPD_REQ,
	                                        prf_get_task_from_id(TASK_ID_FF80S),
	                                        TASK_APP,
	                                        ff80s_ff81_level_upd_req);
	// Fill in the parameter structure
	req->length = len;
	memcpy(req->ff81_level, buf, len);

	// Send the message
	ke_msg_send(req);
}


static int ff80s_ff81_level_ntf_cfg_ind_handler(ke_msg_id_t const msgid,
        struct ff80s_ff81_level_ntf_cfg_ind const *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
	UART_PRINTF("param->ntf_cfg = %x\r\n",param->ntf_cfg);
	if(param->ntf_cfg == PRF_CLI_STOP_NTFIND)
	{
		// ke_timer_clear(FF80S_FF81_LEVEL_PERIOD_NTF,dest_id);
	}
	else
	{
		// ke_timer_set(FF80S_FF81_LEVEL_PERIOD_NTF,dest_id , 1);
	}

	return (KE_MSG_CONSUMED);
}

static int ff81_level_upd_handler(ke_msg_id_t const msgid,
                                  struct ff80s_ff81_level_upd_rsp const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
	if(param->status == GAP_ERR_NO_ERROR)
	{
		// uint8_t buf[128];
		// memset(buf, 0xcc, 128);
		// app_ff81_send_lvl(buf, 128);
	}

	return (KE_MSG_CONSUMED);
}



/**
 ****************************************************************************************
 * @brief
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_ff80_msg_dflt_handler(ke_msg_id_t const msgid,
                                     void const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
	UART_PRINTF("%s\r\n", __func__);

	// Drop the message
	return (KE_MSG_CONSUMED);
}


static int ff81_writer_req_handler(ke_msg_id_t const msgid,
                                   struct ff80s_ff81_writer_ind *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
	extern void ff80_ff81_0x2a9b_cb(uint8_t* buf);
	// Drop the message
	UART_PRINTF("FF81 param->value = 0x ");

	for(uint8_t i = 0; i < param->length; i++)
	{
		UART_PRINTF("%02x ",param->ff81_value[i]);
	}
	UART_PRINTF("\r\n");
	ff80_ff81_0x2a9b_cb(param->ff81_value);
	return (KE_MSG_CONSUMED);
}


static int ff81_period_ntf_handler(ke_msg_id_t const msgid,
                                   struct ff80s_ff81_level_ntf_cfg_ind const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
	// uint8_t buf[128];
	// memset(buf, 0xff, 128);
	// app_ff81_send_lvl(buf, 128);
	//ke_timer_set(FF80S_FF81_LEVEL_PERIOD_NTF,dest_id , 100);

	return (KE_MSG_CONSUMED);
}



/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler app_ff80_msg_handler_list[] =
{
	// Note: first message is latest message checked by kernel so default is put on top.
	{KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_ff80_msg_dflt_handler},
	{FF80S_FF81_LEVEL_NTF_CFG_IND,  (ke_msg_func_t)ff80s_ff81_level_ntf_cfg_ind_handler},
	{FF80S_FF81_LEVEL_UPD_RSP,      (ke_msg_func_t)ff81_level_upd_handler},
	{FF80S_FF81_WRITER_REQ_IND,		(ke_msg_func_t)ff81_writer_req_handler},
	{FF80S_FF81_LEVEL_PERIOD_NTF,	(ke_msg_func_t)ff81_period_ntf_handler},
};

const struct ke_state_handler app_ff80_table_handler =
{&app_ff80_msg_handler_list[0], (sizeof(app_ff80_msg_handler_list)/sizeof(struct ke_msg_handler))};


