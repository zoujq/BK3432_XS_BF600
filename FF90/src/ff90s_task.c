/**
 ****************************************************************************************
 *
 * @file   ff90s_task.c
 *
 * @brief FF90 Server Role Task Implementation.
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

#if (BLE_FF90_SERVER)

#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "atts.h"
#include "co_utils.h"
#include "ff90s.h"
#include "ff90s_task.h"
#include "uart.h"
#include "prf_utils.h"




static int ff90s_ff91_level_upd_req_handler(ke_msg_id_t const msgid,
                                            struct ff90s_ff91_level_upd_req const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    int msg_status = KE_MSG_SAVED;
    uint8_t state = ke_state_get(dest_id);
	
    // check state of the task
    if(state == FF90S_IDLE)
    {
        struct ff90s_env_tag* ff90s_env = PRF_ENV_GET(FF90S, ff90s);

        // put task in a busy state
        ke_state_set(dest_id, FF90S_BUSY);						
		ff90s_notify_ff91_lvl(ff90s_env, param);
		ke_state_set(dest_id, FF90S_IDLE);   
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
    uint8_t status = ff90s_get_att_idx(param->handle, &att_idx);

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;

    if(status == GAP_ERR_NO_ERROR)
    {
        // check if it's a client configuration char
        if(att_idx == FF90S_IDX_FF91_LVL_NTF_CFG)
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
    uint8_t status = ff90s_get_att_idx(param->handle,  &att_idx);
		
    // If the attribute has been found, status is GAP_ERR_NO_ERROR
    if (status == GAP_ERR_NO_ERROR)
    {
        struct ff90s_env_tag* ff90s_env = PRF_ENV_GET(FF90S, ff90s);
        // Extract value before check
        uint16_t ntf_cfg = co_read16p(&param->value[0]);

        // Only update configuration if value for stop or notification enable
        if ((att_idx == FF90S_IDX_FF91_LVL_NTF_CFG)
                && ((ntf_cfg == PRF_CLI_STOP_NTFIND) || (ntf_cfg == PRF_CLI_START_NTF)))
        {

            // Conserve information in environment
            if (ntf_cfg == PRF_CLI_START_NTF)
            {
                // Ntf cfg bit set to 1
                ff90s_env->ntf_cfg[conidx] |= (FF90_FF91_LVL_NTF_SUP );
            }
            else
            {
                // Ntf cfg bit set to 0
                ff90s_env->ntf_cfg[conidx] &= ~(FF90_FF91_LVL_NTF_SUP );
            }

            // Inform APP of configuration change
            struct ff90s_ff91_level_ntf_cfg_ind * ind = KE_MSG_ALLOC(FF90S_FF91_LEVEL_NTF_CFG_IND,
                    prf_dst_task_get(&(ff90s_env->prf_env), conidx), dest_id,
                    ff90s_ff91_level_ntf_cfg_ind);
            ind->conidx = conidx;
            ind->ntf_cfg = ff90s_env->ntf_cfg[conidx];
						
            ke_msg_send(ind);			
        }
		else if (att_idx == FF90S_IDX_FF92_LVL_VAL)
		{
			// Allocate the alert value change indication
			struct ff90s_ff92_writer_ind *ind = KE_MSG_ALLOC(FF90S_FF92_WRITER_REQ_IND,
			        prf_dst_task_get(&(ff90s_env->prf_env), conidx),
			        dest_id, ff90s_ff92_writer_ind);
			
			// Fill in the parameter structure	
			memcpy(ind->ff92_value,&param->value[0],param->length);
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
    uint8_t status = ff90s_get_att_idx(param->handle, &att_idx);
    uint16_t length = 0;
    struct ff90s_env_tag* ff90s_env = PRF_ENV_GET(FF90S, ff90s);

    // If the attribute has been found, status is GAP_ERR_NO_ERROR
    if (status == GAP_ERR_NO_ERROR)
    {
        // read notification information
        if (att_idx == FF90S_IDX_FF91_LVL_VAL)
        {
            length = FF90_FF91_DATA_LEN * sizeof(uint8_t);
        }
        // read notification information
        else if (att_idx == FF90S_IDX_FF91_LVL_NTF_CFG)
        {
            length = sizeof(uint16_t);
        }
        else
        {
            status = PRF_APP_ERROR;
        }
    }

    //Send write response
    cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, length);
    cfm->handle = param->handle;
    cfm->status = status;
    cfm->length = length;
   
    if (status == GAP_ERR_NO_ERROR)
    {
        // read notification information
        if (att_idx == FF90S_IDX_FF91_LVL_VAL)
        {
            cfm->value[0] = ff90s_env->ff91_lvl[0];
        }
        // retrieve notification config
        else if (att_idx == FF90S_IDX_FF91_LVL_NTF_CFG)
        {
            uint16_t ntf_cfg = (ff90s_env->ntf_cfg[conidx] & FF90_FF91_LVL_NTF_SUP) ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND;
            co_write16p(cfm->value, ntf_cfg);
        }  
        else
        {
            /* Not Possible */
        }
    }

    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}   


static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,  struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    if(param->operation == GATTC_NOTIFY)
    {	
      	uint8_t conidx = KE_IDX_GET(src_id);
      	struct ff90s_env_tag* ff90s_env = PRF_ENV_GET(FF90S, ff90s);

	    struct ff90s_ff91_level_upd_rsp *rsp = KE_MSG_ALLOC(FF90S_FF91_LEVEL_UPD_RSP,
                                                         prf_dst_task_get(&(ff90s_env->prf_env), conidx),
                                                         dest_id, ff90s_ff91_level_upd_rsp);

        rsp->status = param->status;			
        ke_msg_send(rsp);
    }

	// go back in to idle mode
    ke_state_set(dest_id, ke_state_get(dest_id) & ~FF90S_BUSY);
	
    return (KE_MSG_CONSUMED);
}

/// Default State handlers definition
const struct ke_msg_handler ff90s_default_state[] =
{
    {FF90S_FF91_LEVEL_UPD_REQ,      (ke_msg_func_t) ff90s_ff91_level_upd_req_handler},
    {GATTC_ATT_INFO_REQ_IND,        (ke_msg_func_t) gattc_att_info_req_ind_handler},
    {GATTC_WRITE_REQ_IND,           (ke_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_READ_REQ_IND,            (ke_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_CMP_EVT,                 (ke_msg_func_t) gattc_cmp_evt_handler},
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler ff90s_default_handler = KE_STATE_HANDLER(ff90s_default_state);

#endif /* #if (BLE_FF90_SERVER) */


