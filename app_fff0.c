/**
 ****************************************************************************************
 *
 * @file app_fff0.c
 *
 * @brief fff0 Application Module entry point
 *
 * @auth  hai.xu
 *
 * @date  2016.05.31
 *
 * Copyright (C) Beken 2009-2016
 *
 *
 ************************************** **************************************************
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
#include "app_fff0.h"              // Battery Application Module Definitions
#include "app.h"                   // Application Definitions
#include "app_task.h"              // application task definitions
#include "fff0s_task.h"            // health thermometer functions
#include "co_bt.h"
#include "prf_types.h"             // Profile common types definition
#include "arch.h"                  // Platform Definitions
#include "prf.h"
#include "fff0s.h"
#include "ke_timer.h"
#include "uart.h"



/*
 * DEFINES
 ****************************************************************************************
 */
extern void fff0_fff1_cb(uint8_t* buf);
extern void fff0_fff2_cb(uint8_t* buf);
extern void fff0_fff3_cb(uint8_t* buf);
extern void fff0_fff4_cb(uint8_t* buf);
extern void fff0_fff5_cb(uint8_t* buf);

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// fff0 Application Module Environment Structure
struct app_fff0_env_tag app_fff0_env;

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */


void app_fff0_init(void)
{

  // Reset the environment
  memset(&app_fff0_env, 0, sizeof(struct app_fff0_env_tag));
}

void app_fff0_add_fff0s(void)
{

  struct fff0s_db_cfg *db_cfg;

  struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                          TASK_GAPM, TASK_APP,
                                          gapm_profile_task_add_cmd, sizeof(struct fff0s_db_cfg));
  // Fill message
  req->operation = GAPM_PROFILE_TASK_ADD;
  req->sec_lvl =   0;
  req->prf_task_id = TASK_ID_FFF0S;
  req->app_task = TASK_APP;
  req->start_hdl = 0; //req->start_hdl = 0; dynamically allocated


  // Set parameters
  db_cfg = (struct fff0s_db_cfg* ) req->param;

  // Sending of notifications is supported
  db_cfg->features = FFF0_FFF1_LVL_NTF_SUP | FFF0_FFF3_LVL_NTF_SUP;
  // Send the message
  ke_msg_send(req);
}


void app_fff1_send_lvl(uint8_t* buf, uint8_t len)
{

  // Allocate the message
  struct fff0s_fff1_level_upd_req * req = KE_MSG_ALLOC(FFF0S_FFF1_LEVEL_UPD_REQ,
                                          prf_get_task_from_id(TASK_ID_FFF0S),
                                          TASK_APP,
                                          fff0s_fff1_level_upd_req);
  // Fill in the parameter structure
  req->length = len;
  memcpy(req->fff1_level, buf, len);

  // Send the message
  ke_msg_send(req);
}

void app_fff3_send_lvl(uint8_t* buf, uint8_t len)
{
  // Allocate the message
  struct fff0s_fff3_level_upd_req * req = KE_MSG_ALLOC(FFF0S_FFF3_LEVEL_UPD_REQ,
                                          prf_get_task_from_id(TASK_ID_FFF0S),
                                          TASK_APP,
                                          fff0s_fff3_level_upd_req);
  // Fill in the parameter structure
  req->length = len;
  memcpy(req->fff3_level, buf, len);

  // Send the message
  ke_msg_send(req);
}


static int fff0s_fff1_level_ntf_cfg_ind_handler(ke_msg_id_t const msgid,
    struct fff0s_fff1_level_ntf_cfg_ind const *param,
    ke_task_id_t const dest_id,
    ke_task_id_t const src_id)
{
  UART_PRINTF("fff1->param->ntf_cfg = %x\r\n", param->ntf_cfg);
  if (param->ntf_cfg == PRF_CLI_STOP_NTFIND)
  {
    //ke_timer_clear(FFF0S_FFF1_LEVEL_PERIOD_NTF,dest_id);
  } else
  {
    //ke_timer_set(FFF0S_FFF1_LEVEL_PERIOD_NTF,dest_id , 100);
  }

  return (KE_MSG_CONSUMED);
}

static int fff0s_fff3_level_ntf_cfg_ind_handler(ke_msg_id_t const msgid,
    struct fff0s_fff3_level_ntf_cfg_ind const *param,
    ke_task_id_t const dest_id,
    ke_task_id_t const src_id)
{
  UART_PRINTF("fff3->param->ntf_cfg = %x\r\n", param->ntf_cfg);
  if (param->ntf_cfg == PRF_CLI_STOP_NTFIND)
  {
    //ke_timer_clear(FFF0S_FFF3_LEVEL_PERIOD_NTF,dest_id);
  } else
  {
    //ke_timer_set(FFF0S_FFF3_LEVEL_PERIOD_NTF,dest_id , 100);
  }

  return (KE_MSG_CONSUMED);
}
static int fff1_level_upd_handler(ke_msg_id_t const msgid,
                                  struct fff0s_fff1_level_upd_rsp const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
  UART_PRINTF("%s\r\n", __func__);

  if (param->status == GAP_ERR_NO_ERROR)
  {
    //uint8_t buf[128];
    //memset(buf, 0xcc, 128);
    //app_fff1_send_lvl(buf, 128);
  }

  return (KE_MSG_CONSUMED);
}

static int fff3_level_upd_handler(ke_msg_id_t const msgid,
                                  struct fff0s_fff3_level_upd_rsp const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
  UART_PRINTF("%s\r\n", __func__);

  if (param->status == GAP_ERR_NO_ERROR)
  {
    //uint8_t buf[128];
    //memset(buf, 0xcc, 128);
    //app_fff3_send_lvl(buf, 128);
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
static int app_fff0_msg_dflt_handler(ke_msg_id_t const msgid,
                                     void const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
  UART_PRINTF("%s\r\n", __func__);

  // Drop the message
  return (KE_MSG_CONSUMED);
}

static int fff1_writer_req_handler(ke_msg_id_t const msgid,
                                   struct fff0s_fff1_writer_ind *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
  // Drop the message
  UART_PRINTF("FFF1 param->value = 0x ");

  for (uint8_t i = 0; i < param->length; i++)
  {
    UART_PRINTF("%02x ", param->fff1_value[i]);
  }
  UART_PRINTF("\r\n");

  fff0_fff1_cb(param->fff1_value);

  return (KE_MSG_CONSUMED);
}
static int fff2_writer_req_handler(ke_msg_id_t const msgid,
                                   struct fff0s_fff2_writer_ind *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
  // Drop the message
  UART_PRINTF("FFF2 param->value = 0x ");

  for (uint8_t i = 0; i < param->length; i++)
  {
    UART_PRINTF("%02x ", param->fff2_value[i]);
  }
  UART_PRINTF("\r\n");
  fff0_fff2_cb(param->fff2_value);
  return (KE_MSG_CONSUMED);
}
static int fff3_writer_req_handler(ke_msg_id_t const msgid,
                                   struct fff0s_fff3_writer_ind *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
  // Drop the message
  UART_PRINTF("FFF3 param->value = 0x ");

  for (uint8_t i = 0; i < param->length; i++)
  {
    UART_PRINTF("%02x ", param->fff3_value[i]);
  }
  UART_PRINTF("\r\n");
  fff0_fff3_cb(param->fff3_value);
  return (KE_MSG_CONSUMED);
}

static int fff4_writer_req_handler(ke_msg_id_t const msgid,
                                   struct fff0s_fff4_writer_ind *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
  // Drop the message
  UART_PRINTF("FFF4 param->value = 0x ");

  for (uint8_t i = 0; i < param->length; i++)
  {
    UART_PRINTF("%02x ", param->fff4_value[i]);
  }
  UART_PRINTF("\r\n");
  fff0_fff4_cb(param->fff4_value);
  return (KE_MSG_CONSUMED);
}

static int fff5_writer_req_handler(ke_msg_id_t const msgid,
                                   struct fff0s_fff5_writer_ind *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
  // Drop the message
  UART_PRINTF("FFF5 param->value = 0x ");

  for (uint8_t i = 0; i < param->length; i++)
  {
    UART_PRINTF("%02x ", param->fff5_value[i]);
  }
  UART_PRINTF("\r\n");
  fff0_fff5_cb(param->fff5_value);
  return (KE_MSG_CONSUMED);
}



static int fff1_period_ntf_handler(ke_msg_id_t const msgid,
                                   struct fff0s_fff1_level_ntf_cfg_ind const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
  // uint8_t buf[20];
  // memset(buf, 0xff, 20);
  // app_fff1_send_lvl(buf, 20);
  //ke_timer_set(FFF0S_FFF1_LEVEL_PERIOD_NTF,dest_id , 100);

  return (KE_MSG_CONSUMED);
}

static int fff3_period_ntf_handler(ke_msg_id_t const msgid,
                                   struct fff0s_fff3_level_ntf_cfg_ind const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
  uint8_t buf[20];
  memset(buf, 0xcc, 20);
  app_fff3_send_lvl(buf, 20);
  //ke_timer_set(FFF0S_FFF3_LEVEL_PERIOD_NTF,dest_id , 100);

  return (KE_MSG_CONSUMED);
}


/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler app_fff0_msg_handler_list[] =
{
  // Note: first message is latest message checked by kernel so default is put on top.
  {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_fff0_msg_dflt_handler},
  {FFF0S_FFF1_LEVEL_NTF_CFG_IND,  (ke_msg_func_t)fff0s_fff1_level_ntf_cfg_ind_handler},
  {FFF0S_FFF3_LEVEL_NTF_CFG_IND,  (ke_msg_func_t)fff0s_fff3_level_ntf_cfg_ind_handler},
  {FFF0S_FFF1_LEVEL_UPD_RSP,      (ke_msg_func_t)fff1_level_upd_handler},
  {FFF0S_FFF3_LEVEL_UPD_RSP,      (ke_msg_func_t)fff3_level_upd_handler},
  {FFF0S_FFF1_WRITER_REQ_IND,    (ke_msg_func_t)fff1_writer_req_handler},
  {FFF0S_FFF2_WRITER_REQ_IND,   (ke_msg_func_t)fff2_writer_req_handler},
  {FFF0S_FFF3_WRITER_REQ_IND,   (ke_msg_func_t)fff3_writer_req_handler},
  {FFF0S_FFF4_WRITER_REQ_IND,   (ke_msg_func_t)fff4_writer_req_handler},
  {FFF0S_FFF5_WRITER_REQ_IND,   (ke_msg_func_t)fff5_writer_req_handler},
  {FFF0S_FFF1_LEVEL_PERIOD_NTF, (ke_msg_func_t)fff1_period_ntf_handler},
  {FFF0S_FFF3_LEVEL_PERIOD_NTF, (ke_msg_func_t)fff3_period_ntf_handler}
};

const struct ke_state_handler app_fff0_table_handler =
{&app_fff0_msg_handler_list[0], (sizeof(app_fff0_msg_handler_list) / sizeof(struct ke_msg_handler))};


