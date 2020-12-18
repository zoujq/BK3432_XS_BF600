/**
 ****************************************************************************************
 *
 * @file   fff0s_task.c
 *
 * @brief FFF0 Server Role Task Implementation.
 *
 * Copyright (C) Beken 2009-2015
 *
 *
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_FFF0_SERVER)

#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "atts.h"
#include "co_utils.h"
#include "fff0s.h"
#include "fff0s_task.h"
#include "prf_utils.h"
#include "uart.h"



static int fff0s_fff1_level_upd_req_handler(ke_msg_id_t const msgid,
                                            struct fff0s_fff1_level_upd_req const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    int msg_status = KE_MSG_SAVED;
    uint8_t state = ke_state_get(dest_id);
	
    // check state of the task
    if(state == FFF0S_IDLE)
    {
        struct fff0s_env_tag* fff0s_env = PRF_ENV_GET(FFF0S, fff0s);

        // put task in a busy state
        ke_state_set(dest_id, FFF0S_BUSY);						
		fff0s_notify_fff1_lvl(fff0s_env, param);
		ke_state_set(dest_id, FFF0S_IDLE);   
		msg_status = KE_MSG_CONSUMED;	
    }

    return (msg_status);
  }

static int fff0s_fff3_level_upd_req_handler(ke_msg_id_t const msgid,
                                            struct fff0s_fff3_level_upd_req const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    int msg_status = KE_MSG_SAVED;
    uint8_t state = ke_state_get(dest_id);
	
    // check state of the task
    if(state == FFF0S_IDLE)
    {
        struct fff0s_env_tag* fff0s_env = PRF_ENV_GET(FFF0S, fff0s);

        // put task in a busy state
        ke_state_set(dest_id, FFF0S_BUSY);						
		fff0s_notify_fff3_lvl(fff0s_env, param);
		ke_state_set(dest_id, FFF0S_IDLE);   
		msg_status = KE_MSG_CONSUMED;	
    }

    return (msg_status);
  }

  
static int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gattc_att_info_req_ind *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{

    struct gattc_att_info_cfm * cfm;
    uint8_t  att_idx = 0;
    // retrieve handle information
    uint8_t status = fff0s_get_att_idx(param->handle, &att_idx);

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;

    if(status == GAP_ERR_NO_ERROR)
    {
        // check if it's a client configuration char
        if(att_idx == FFF0S_IDX_FFF1_LVL_NTF_CFG)
        {
            // CCC attribute length = 2
            cfm->length = 2;
        }
		else if(att_idx == FFF0S_IDX_FFF3_LVL_NTF_CFG)
		{
			// CCC attribute length = 2
            cfm->length = 2;
		}
        // not expected request
        else
        {
            cfm->length = 0;
            status = ATT_ERR_WRITE_NOT_PERMITTED;
        }
    }

    cfm->status = status;
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}



static int gattc_write_req_ind_handler(ke_msg_id_t const msgid, struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct gattc_write_cfm * cfm;
    uint8_t att_idx = 0;
    uint8_t conidx = KE_IDX_GET(src_id);
    // retrieve handle information
    uint8_t status = fff0s_get_att_idx(param->handle,  &att_idx);
    // If the attribute has been found, status is GAP_ERR_NO_ERROR
    if (status == GAP_ERR_NO_ERROR)
    {
        struct fff0s_env_tag* fff0s_env = PRF_ENV_GET(FFF0S, fff0s);
        // Extract value before check
        uint16_t ntf_cfg = co_read16p(&param->value[0]);

        // Only update configuration if value for stop or notification enable
        if ((att_idx == FFF0S_IDX_FFF1_LVL_NTF_CFG)
                && ((ntf_cfg == PRF_CLI_STOP_NTFIND) 
                    ||(ntf_cfg == PRF_CLI_START_NTF)
                    || (ntf_cfg == PRF_CLI_START_IND)))
        {
            // Conserve information in environment
            if (ntf_cfg == PRF_CLI_START_NTF || (ntf_cfg == PRF_CLI_START_IND))
            {
                // Ntf cfg bit set to 1
                fff0s_env->ntf_cfg[conidx] |= (FFF0_FFF1_LVL_NTF_SUP );
            }
            else
            {
                // Ntf cfg bit set to 0
                fff0s_env->ntf_cfg[conidx] &= ~(FFF0_FFF1_LVL_NTF_SUP );
            }

            // Inform APP of configuration change
            struct fff0s_fff1_level_ntf_cfg_ind * ind = KE_MSG_ALLOC(FFF0S_FFF1_LEVEL_NTF_CFG_IND,
                    prf_dst_task_get(&(fff0s_env->prf_env), conidx), dest_id,
                    fff0s_fff1_level_ntf_cfg_ind);
            ind->conidx = conidx;
            ind->ntf_cfg = fff0s_env->ntf_cfg[conidx];
						
            ke_msg_send(ind);			
        }
		else if ((att_idx == FFF0S_IDX_FFF1_LVL_NTF_CFG)
                && ((ntf_cfg == PRF_CLI_STOP_NTFIND) || (ntf_cfg == PRF_CLI_START_IND)))
		{
           //....
		}
		else if ((att_idx == FFF0S_IDX_FFF3_LVL_NTF_CFG)
                && ((ntf_cfg == PRF_CLI_STOP_NTFIND) || 
                    (ntf_cfg == PRF_CLI_START_NTF)
                    || (ntf_cfg == PRF_CLI_START_IND)))
        {
            // Conserve information in environment
            if (ntf_cfg == PRF_CLI_START_NTF || (ntf_cfg == PRF_CLI_START_IND))
            {
                // Ntf cfg bit set to 1
                fff0s_env->ntf_cfg[conidx] |= (FFF0_FFF3_LVL_NTF_SUP);
            }
            else
            {
                // Ntf cfg bit set to 0
                fff0s_env->ntf_cfg[conidx] &= ~(FFF0_FFF3_LVL_NTF_SUP);
            }

            // Inform APP of configuration change
            struct fff0s_fff3_level_ntf_cfg_ind * ind = KE_MSG_ALLOC(FFF0S_FFF3_LEVEL_NTF_CFG_IND,
                    prf_dst_task_get(&(fff0s_env->prf_env), conidx), dest_id,
                    fff0s_fff3_level_ntf_cfg_ind);
            ind->conidx = conidx;
            ind->ntf_cfg = fff0s_env->ntf_cfg[conidx];
						
            ke_msg_send(ind);			
        }
		else if ((att_idx == FFF0S_IDX_FFF3_LVL_NTF_CFG)
                && ((ntf_cfg == PRF_CLI_STOP_NTFIND) || (ntf_cfg == PRF_CLI_START_IND)))
		{
            //....
		}
		else if (att_idx == FFF0S_IDX_FFF1_LVL_VAL)
		{

			// Allocate the alert value change indication
			struct fff0s_fff1_writer_ind *ind = KE_MSG_ALLOC(FFF0S_FFF1_WRITER_REQ_IND,
			        prf_dst_task_get(&(fff0s_env->prf_env), conidx),
			        dest_id, fff0s_fff1_writer_ind);
			
			// Fill in the parameter structure	
			memcpy(ind->fff1_value,&param->value,param->length);
			ind->conidx = conidx;
			ind->length = param->length;
			
			// Send the message
			ke_msg_send(ind);
		}
        else if (att_idx == FFF0S_IDX_FFF2_LVL_VAL)
        {
            
            // Allocate the alert value change indication
            struct fff0s_fff2_writer_ind *ind = KE_MSG_ALLOC(FFF0S_FFF2_WRITER_REQ_IND,
                    prf_dst_task_get(&(fff0s_env->prf_env), conidx),
                    dest_id, fff0s_fff2_writer_ind);
            
            // Fill in the parameter structure  
            memcpy(ind->fff2_value,&param->value,param->length);
            ind->conidx = conidx;
            ind->length = param->length;
            
            // Send the message
            ke_msg_send(ind);
        }
        else if (att_idx == FFF0S_IDX_FFF3_LVL_VAL)
        {
            
            // Allocate the alert value change indication
            struct fff0s_fff3_writer_ind *ind = KE_MSG_ALLOC(FFF0S_FFF3_WRITER_REQ_IND,
                    prf_dst_task_get(&(fff0s_env->prf_env), conidx),
                    dest_id, fff0s_fff3_writer_ind);
            
            // Fill in the parameter structure  
            memcpy(ind->fff3_value,&param->value,param->length);
            ind->conidx = conidx;
            ind->length = param->length;
            
            // Send the message
            ke_msg_send(ind);
        }
        else if (att_idx == FFF0S_IDX_FFF4_LVL_VAL)
        {
            
            // Allocate the alert value change indication
            struct fff0s_fff4_writer_ind *ind = KE_MSG_ALLOC(FFF0S_FFF4_WRITER_REQ_IND,
                    prf_dst_task_get(&(fff0s_env->prf_env), conidx),
                    dest_id, fff0s_fff4_writer_ind);
            
            // Fill in the parameter structure  
            memcpy(ind->fff4_value,&param->value,param->length);
            ind->conidx = conidx;
            ind->length = param->length;
            
            // Send the message
            ke_msg_send(ind);
        }
        else if (att_idx == FFF0S_IDX_FFF5_LVL_VAL)
        {
            
            // Allocate the alert value change indication
            struct fff0s_fff5_writer_ind *ind = KE_MSG_ALLOC(FFF0S_FFF5_WRITER_REQ_IND,
                    prf_dst_task_get(&(fff0s_env->prf_env), conidx),
                    dest_id, fff0s_fff5_writer_ind);
            
            // Fill in the parameter structure  
            memcpy(ind->fff5_value,&param->value,param->length);
            ind->conidx = conidx;
            ind->length = param->length;
            
            // Send the message
            ke_msg_send(ind);
        }
        else
        {
            status = PRF_APP_ERROR;
        }

    }

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
    cfm->handle = param->handle;
    cfm->status = status;
    ke_msg_send(cfm);


    return (KE_MSG_CONSUMED);
}   



static int gattc_read_req_ind_handler(ke_msg_id_t const msgid, struct gattc_read_req_ind const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct gattc_read_cfm * cfm;
    uint8_t  att_idx = 0;
    uint8_t conidx = KE_IDX_GET(src_id);
    // retrieve handle information
    uint8_t status = fff0s_get_att_idx(param->handle, &att_idx);
    uint16_t length = 0;
    struct fff0s_env_tag* fff0s_env = PRF_ENV_GET(FFF0S, fff0s);

    //Send write response
    cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, length);
    cfm->handle = param->handle;
    cfm->status = status;
    cfm->length = length;

    
    // If the attribute has been found, status is GAP_ERR_NO_ERROR
    if (status == GAP_ERR_NO_ERROR)
    {
        // read notification information
        if (att_idx == FFF0S_IDX_FFF1_LVL_VAL)
        {
            cfm->length = FFF0_FFF1_DATA_LEN;
            memcpy(cfm->value,fff0s_env->fff1_value,cfm->length);
            UART_PRINTF("FFF0 FFF1 value gattc_read_req_ind_handler ");
        }
        // read notification information
        else if (att_idx == FFF0S_IDX_FFF1_LVL_NTF_CFG)
        {
            cfm->length = sizeof(uint16_t);
            uint16_t ntf_cfg = (fff0s_env->ntf_cfg[conidx] & FFF0_FFF1_LVL_NTF_SUP) ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND;
            co_write16p(cfm->value, ntf_cfg);
        }
        else if(att_idx == FFF0S_IDX_FFF2_LVL_VAL)
        {
            cfm->length = FFF0_FFF2_DATA_LEN;
            memcpy(cfm->value,fff0s_env->fff2_value,cfm->length);
            UART_PRINTF("FFF0 FFF2 value gattc_read_req_ind_handler ");
        }
		else if(att_idx == FFF0S_IDX_FFF3_LVL_VAL)
		{
			cfm->length = FFF0_FFF3_DATA_LEN;
            memcpy(cfm->value,fff0s_env->fff3_value,cfm->length);
            UART_PRINTF("FFF0 FFF3 value gattc_read_req_ind_handler ");
		}
		else if(att_idx == FFF0S_IDX_FFF3_LVL_NTF_CFG)
		{
			cfm->length = sizeof(uint16_t);
            uint16_t ntf_cfg = (fff0s_env->ntf_cfg[conidx] & FFF0_FFF3_LVL_NTF_SUP) ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND;
            co_write16p(cfm->value, ntf_cfg);
		}
        else if(att_idx == FFF0S_IDX_FFF4_LVL_VAL)
        {
            cfm->length = FFF0_FFF4_DATA_LEN;
            memcpy(cfm->value,fff0s_env->fff4_value,cfm->length);
            UART_PRINTF("FFF0 FFF4 value gattc_read_req_ind_handler ");
        }
        else if(att_idx == FFF0S_IDX_FFF5_LVL_VAL)
        {
            cfm->length = FFF0_FFF5_DATA_LEN;
            memcpy(cfm->value,fff0s_env->fff5_value,cfm->length);
            UART_PRINTF("FFF0 FFF5 value gattc_read_req_ind_handler ");
        }
        else
        {
            status = PRF_APP_ERROR;
        }
    }
    
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}   

extern uint16_t g_hande_fff0;
static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,  struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    if(param->operation == GATTC_NOTIFY || param->operation == GATTC_INDICATE)
    {   	
      	uint8_t conidx = KE_IDX_GET(src_id);
      	struct fff0s_env_tag* fff0s_env = PRF_ENV_GET(FFF0S, fff0s);

		if(g_hande_fff0 == fff0s_get_att_handle(FFF0S_IDX_FFF1_LVL_VAL))
		{
			 struct fff0s_fff1_level_upd_rsp *rsp = KE_MSG_ALLOC(FFF0S_FFF1_LEVEL_UPD_RSP,
                                                         prf_dst_task_get(&(fff0s_env->prf_env), conidx),
                                                         dest_id, fff0s_fff1_level_upd_rsp);
			rsp->status = param->status;			
        	ke_msg_send(rsp);
		}
		else
		{
			struct fff0s_fff3_level_upd_rsp *rsp = KE_MSG_ALLOC(FFF0S_FFF3_LEVEL_UPD_RSP,
                                                         prf_dst_task_get(&(fff0s_env->prf_env), conidx),
                                                         dest_id, fff0s_fff3_level_upd_rsp);
			rsp->status = param->status;			
        	ke_msg_send(rsp);
		}
    }

	// go back in to idle mode
    ke_state_set(dest_id, ke_state_get(dest_id) & ~FFF0S_BUSY);
	
    return (KE_MSG_CONSUMED);
}

/// Default State handlers definition
const struct ke_msg_handler fff0s_default_state[] =
{
    {FFF0S_FFF1_LEVEL_UPD_REQ,      (ke_msg_func_t) fff0s_fff1_level_upd_req_handler},
	{FFF0S_FFF3_LEVEL_UPD_REQ,      (ke_msg_func_t) fff0s_fff3_level_upd_req_handler},
    {GATTC_ATT_INFO_REQ_IND,        (ke_msg_func_t) gattc_att_info_req_ind_handler},
    {GATTC_WRITE_REQ_IND,           (ke_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_READ_REQ_IND,            (ke_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_CMP_EVT,                 (ke_msg_func_t) gattc_cmp_evt_handler},
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler fff0s_default_handler = KE_STATE_HANDLER(fff0s_default_state);

#endif /* #if (BLE_FFF0_SERVER) */


