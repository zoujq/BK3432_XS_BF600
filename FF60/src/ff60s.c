/**
 ****************************************************************************************
 *
 * @file ff60s.c
 *
 * @brief ff60 Server Implementation.
 *
 * Copyright (C) beken 2009-2015
 *
 *
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_FF60_SERVER)
#include "attm.h"
#include "ff60s.h"
#include "ff60s_task.h"
#include "prf_utils.h"
#include "prf.h"
#include "ke_mem.h"

#include "uart.h"



/*
 * FF60 ATTRIBUTES DEFINITION
 ****************************************************************************************
 */
 
/// Full FF60 Database Description - Used to add attributes into the database
const struct attm_desc ff60_att_db[FF60S_IDX_NB] =
{
    // FF60 Service Declaration
    [FF60S_IDX_SVC]            =   {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0, 0},

	[FF60S_IDX_FF62_LVL_CHAR]  =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},
    //  Characteristic Value
    [FF60S_IDX_FF62_LVL_VAL]   =   {ATT_USER_SERVER_CHAR_FF62,PERM(IND, ENABLE), PERM(RI, ENABLE), FF60_FF62_DATA_LEN *sizeof(uint8_t)},

	// ff61 Level Characteristic Declaration
	[FF60S_IDX_FF61_LVL_CHAR]  =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},
    // ff61 Level Characteristic Value
    [FF60S_IDX_FF61_LVL_VAL]   =   {ATT_USER_SERVER_CHAR_FF61, PERM(RD, ENABLE) , PERM(RI, ENABLE), FF60_FF61_DATA_LEN * sizeof(uint8_t)},

	// ff61 Level Characteristic - Client Characteristic Configuration Descriptor
	[FF60S_IDX_FF61_LVL_NTF_CFG] = {ATT_DESC_CLIENT_CHAR_CFG,  PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 0, 0},

};/// Macro used to retrieve permission value from access and rights on attribute.


static uint8_t ff60s_init (struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl,  struct ff60s_db_cfg* params)
{
    uint16_t shdl;
    struct ff60s_env_tag* ff60s_env = NULL;
    // Status
    uint8_t status = GAP_ERR_NO_ERROR;
    
    //-------------------- allocate memory required for the profile  ---------------------
    ff60s_env = (struct ff60s_env_tag* ) ke_malloc(sizeof(struct ff60s_env_tag), KE_MEM_ATT_DB);
    memset(ff60s_env, 0 , sizeof(struct ff60s_env_tag));

    // Service content flag
    uint8_t cfg_flag = FF60S_CFG_FLAG_MANDATORY_MASK;

    // Save database configuration
    ff60s_env->features |= (params->features) ;
   
    // Check if notifications are supported
    if (params->features == FF60_FF61_LVL_NTF_SUP)
    {
        cfg_flag |= FF60_CFG_FLAG_NTF_SUP_MASK;
    }
    shdl = *start_hdl;

    //Create FF60 in the DB
    //------------------ create the attribute database for the profile -------------------
    status = attm_svc_create_db(&(shdl), ATT_USER_SERVER_FF60, (uint8_t *)&cfg_flag,
            FF60S_IDX_NB, NULL, env->task, &ff60_att_db[0],
            (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_AUTH | PERM_MASK_SVC_EKS)));
				


    // //Set optional permissions
    // if (status == GAP_ERR_NO_ERROR)
    // {
    //     //Set optional permissions
    //     if(params->features == FF60_FF61_LVL_NTF_SUP)
    //     {
    //         // Battery Level characteristic value permissions
    //         uint16_t perm = PERM(RD, ENABLE) | PERM(NTF, ENABLE);

    //         attm_att_set_permission(shdl + FF60S_IDX_FF61_LVL_VAL, perm, 0);
    //     }
    // }

    //-------------------- Update profile task information  ---------------------
    if (status == ATT_ERR_NO_ERROR)
    {

        // allocate BASS required environment variable
        env->env = (prf_env_t*) ff60s_env;
        *start_hdl = shdl;
        ff60s_env->start_hdl = *start_hdl;
        ff60s_env->prf_env.app_task = app_task
                | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        ff60s_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id                     = TASK_ID_FF60S;
        env->desc.idx_max           = FF60S_IDX_MAX;
        env->desc.state             = ff60s_env->state;
        env->desc.default_handler   = &ff60s_default_handler;

        // service is ready, go into an Idle state
        ke_state_set(env->task, FF60S_IDLE);
    }
    else if(ff60s_env != NULL)
    {
        ke_free(ff60s_env);
    }
     
    return (status);
}


static void ff60s_destroy(struct prf_task_env* env)
{
    struct ff60s_env_tag* ff60s_env = (struct ff60s_env_tag*) env->env;

    // clear on-going operation
    if(ff60s_env->operation != NULL)
    {
        ke_free(ff60s_env->operation);
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(ff60s_env);
}

static void ff60s_create(struct prf_task_env* env, uint8_t conidx)
{
    struct ff60s_env_tag* ff60s_env = (struct ff60s_env_tag*) env->env;
    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);

    // force notification config to zero when peer device is connected
    ff60s_env->ntf_cfg[conidx] = 0;
}


static void ff60s_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct ff60s_env_tag* ff60s_env = (struct ff60s_env_tag*) env->env;

    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);
    // force notification config to zero when peer device is disconnected
    ff60s_env->ntf_cfg[conidx] = 0;
}


void ff60s_notify_ff61_lvl(struct ff60s_env_tag* ff60s_env, struct ff60s_ff61_level_upd_req const *param)
{
    // Allocate the GATT notification message
    struct gattc_send_evt_cmd *ff61_lvl = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
            KE_BUILD_ID(TASK_GATTC, 0), prf_src_task_get(&(ff60s_env->prf_env),0),
            gattc_send_evt_cmd, sizeof(uint8_t)* (param->length));

    // Fill in the parameter structure
    ff61_lvl->operation = GATTC_NOTIFY;
    ff61_lvl->handle = ff60s_get_att_handle(FF60S_IDX_FF61_LVL_VAL);
    // pack measured value in database
    ff61_lvl->length = param->length;
  	//ff61_lvl->value[0] = ff60s_env->ff61_lvl[0];
		memcpy(&ff61_lvl->value[0],&param->ff61_level[0],param->length);
    // send notification to peer device
    ke_msg_send(ff61_lvl);
}



/// BASS Task interface required by profile manager
const struct prf_task_cbs ff60s_itf =
{
        (prf_init_fnct) ff60s_init,
        ff60s_destroy,
        ff60s_create,
        ff60s_cleanup,
};


const struct prf_task_cbs* ff60s_prf_itf_get(void)
{
   return &ff60s_itf;
}


uint16_t ff60s_get_att_handle( uint8_t att_idx)
{
		
    struct ff60s_env_tag *ff60s_env = PRF_ENV_GET(FF60S, ff60s);
    uint16_t handle = ATT_INVALID_HDL;
   
    handle = ff60s_env->start_hdl;

    // increment index according to expected index
    if(att_idx < FF60S_IDX_FF61_LVL_NTF_CFG)
    {
        handle += att_idx;
    }
    // FF61 notification
    else if((att_idx == FF60S_IDX_FF61_LVL_NTF_CFG) && (((ff60s_env->features ) & 0x01) == FF60_FF61_LVL_NTF_SUP))
    {
        handle += FF60S_IDX_FF61_LVL_NTF_CFG;			
    }		      
    else
    {
        handle = ATT_INVALID_HDL;
    }
    

    return handle;
}

uint8_t ff60s_get_att_idx(uint16_t handle, uint8_t *att_idx)
{
    struct ff60s_env_tag* ff60s_env = PRF_ENV_GET(FF60S, ff60s);
    uint16_t hdl_cursor = ff60s_env->start_hdl;
    uint8_t status = PRF_APP_ERROR;

    // Browse list of services
    // handle must be greater than current index 
    // check if it's a mandatory index
    if(handle <= (hdl_cursor + FF60S_IDX_FF61_LVL_VAL))
    {
        *att_idx = handle -hdl_cursor;
        status = GAP_ERR_NO_ERROR;
        
    }
    hdl_cursor += FF60S_IDX_FF61_LVL_VAL;

    // check if it's a notify index
    if(((ff60s_env->features ) & 0x01) == FF60_FF61_LVL_NTF_SUP)
    {
        hdl_cursor++;
        if(handle == hdl_cursor)
        {
            *att_idx = FF60S_IDX_FF61_LVL_NTF_CFG;
            status = GAP_ERR_NO_ERROR;
        }
    }
    hdl_cursor++;
    
    return (status);
}


#endif // (BLE_ff60_SERVER)


 
