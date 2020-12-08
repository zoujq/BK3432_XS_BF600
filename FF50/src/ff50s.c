/**
 ****************************************************************************************
 *
 * @file ff50s.c
 *
 * @brief ff50 Server Implementation.
 *
 * Copyright (C) beken 2009-2015
 *
 *
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_FF50_SERVER)
#include "attm.h"
#include "ff50s.h"
#include "ff50s_task.h"
#include "prf_utils.h"
#include "prf.h"
#include "ke_mem.h"

#include "uart.h"



/*
 * FF50 ATTRIBUTES DEFINITION
 ****************************************************************************************
 */
 
/// Full FF50 Database Description - Used to add attributes into the database
const struct attm_desc ff50_att_db[FF50S_IDX_NB] =
{
    // FF50 Service Declaration
    [FF50S_IDX_SVC]            =   {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0, 0},

    [FF50S_IDX_FF52_LVL_CHAR]  =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},
    //  Characteristic Value
    [FF50S_IDX_FF52_LVL_VAL]   =   {ATT_USER_SERVER_CHAR_FF52,PERM(WRITE_COMMAND, ENABLE), PERM(RI, ENABLE), FF50_FF52_DATA_LEN *sizeof(uint8_t)},

    // ff51 Level Characteristic Declaration
    [FF50S_IDX_FF51_LVL_CHAR]  =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},
    // ff51 Level Characteristic Value
    [FF50S_IDX_FF51_LVL_VAL]   =   {ATT_USER_SERVER_CHAR_FF51, PERM(WRITE_COMMAND, ENABLE) , PERM(RI, ENABLE), FF50_FF51_DATA_LEN * sizeof(uint8_t)},

    // ff51 Level Characteristic - Client Characteristic Configuration Descriptor
    [FF50S_IDX_FF51_LVL_NTF_CFG] = {ATT_DESC_CLIENT_CHAR_CFG,  PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 0, 0},

};/// Macro used to retrieve permission value from access and rights on attribute.


static uint8_t ff50s_init (struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl,  struct ff50s_db_cfg* params)
{
    uint16_t shdl;
    struct ff50s_env_tag* ff50s_env = NULL;
    // Status
    uint8_t status = GAP_ERR_NO_ERROR;
    
    //-------------------- allocate memory required for the profile  ---------------------
    ff50s_env = (struct ff50s_env_tag* ) ke_malloc(sizeof(struct ff50s_env_tag), KE_MEM_ATT_DB);
    memset(ff50s_env, 0 , sizeof(struct ff50s_env_tag));

    // Service content flag
    uint8_t cfg_flag = FF50S_CFG_FLAG_MANDATORY_MASK;

    // Save database configuration
    ff50s_env->features |= (params->features) ;
   
    // Check if notifications are supported
    if (params->features == FF50_FF51_LVL_NTF_SUP)
    {
        cfg_flag |= FF50_CFG_FLAG_NTF_SUP_MASK;
    }
    shdl = *start_hdl;

    //Create FF50 in the DB
    //------------------ create the attribute database for the profile -------------------
    status = attm_svc_create_db(&(shdl), ATT_USER_SERVER_FF50, (uint8_t *)&cfg_flag,
            FF50S_IDX_NB, NULL, env->task, &ff50_att_db[0],
            (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_AUTH | PERM_MASK_SVC_EKS)));
                


    //Set optional permissions
    if (status == GAP_ERR_NO_ERROR)
    {
        //Set optional permissions
        if(params->features == FF50_FF51_LVL_NTF_SUP)
        {
            // Battery Level characteristic value permissions
            uint16_t perm = PERM(RD, ENABLE) | PERM(IND, ENABLE);

            attm_att_set_permission(shdl + FF50S_IDX_FF51_LVL_VAL, perm, 0);
        }
    }

    //-------------------- Update profile task information  ---------------------
    if (status == ATT_ERR_NO_ERROR)
    {

        // allocate BASS required environment variable
        env->env = (prf_env_t*) ff50s_env;
        *start_hdl = shdl;
        ff50s_env->start_hdl = *start_hdl;
        ff50s_env->prf_env.app_task = app_task
                | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        ff50s_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id                     = TASK_ID_FF50S;
        env->desc.idx_max           = FF50S_IDX_MAX;
        env->desc.state             = ff50s_env->state;
        env->desc.default_handler   = &ff50s_default_handler;

        // service is ready, go into an Idle state
        ke_state_set(env->task, FF50S_IDLE);
    }
    else if(ff50s_env != NULL)
    {
        ke_free(ff50s_env);
    }
     
    return (status);
}


static void ff50s_destroy(struct prf_task_env* env)
{
    struct ff50s_env_tag* ff50s_env = (struct ff50s_env_tag*) env->env;

    // clear on-going operation
    if(ff50s_env->operation != NULL)
    {
        ke_free(ff50s_env->operation);
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(ff50s_env);
}

static void ff50s_create(struct prf_task_env* env, uint8_t conidx)
{
    struct ff50s_env_tag* ff50s_env = (struct ff50s_env_tag*) env->env;
    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);

    // force notification config to zero when peer device is connected
    ff50s_env->ntf_cfg[conidx] = 0;
}


static void ff50s_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct ff50s_env_tag* ff50s_env = (struct ff50s_env_tag*) env->env;

    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);
    // force notification config to zero when peer device is disconnected
    ff50s_env->ntf_cfg[conidx] = 0;
}


void ff50s_notify_ff51_lvl(struct ff50s_env_tag* ff50s_env, struct ff50s_ff51_level_upd_req const *param)
{
    // Allocate the GATT notification message
    struct gattc_send_evt_cmd *ff51_lvl = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
            KE_BUILD_ID(TASK_GATTC, 0), prf_src_task_get(&(ff50s_env->prf_env),0),
            gattc_send_evt_cmd, sizeof(uint8_t)* (param->length));

    // Fill in the parameter structure
    // ff51_lvl->operation = GATTC_NOTIFY;
    ff51_lvl->operation =GATTC_INDICATE;
    ff51_lvl->handle = ff50s_get_att_handle(FF50S_IDX_FF51_LVL_VAL);
    // pack measured value in database
    ff51_lvl->length = param->length;
    //ff51_lvl->value[0] = ff50s_env->ff51_lvl[0];
    memcpy(&ff51_lvl->value[0],&param->ff51_level[0],param->length);
    // send notification to peer device
    ke_msg_send(ff51_lvl);
}



/// BASS Task interface required by profile manager
const struct prf_task_cbs ff50s_itf =
{
        (prf_init_fnct) ff50s_init,
        ff50s_destroy,
        ff50s_create,
        ff50s_cleanup,
};


const struct prf_task_cbs* ff50s_prf_itf_get(void)
{
   return &ff50s_itf;
}


uint16_t ff50s_get_att_handle( uint8_t att_idx)
{
        
    struct ff50s_env_tag *ff50s_env = PRF_ENV_GET(FF50S, ff50s);
    uint16_t handle = ATT_INVALID_HDL;
   
    handle = ff50s_env->start_hdl;

    // increment index according to expected index
    if(att_idx < FF50S_IDX_FF51_LVL_NTF_CFG)
    {
        handle += att_idx;
    }
    // FF51 notification
    else if((att_idx == FF50S_IDX_FF51_LVL_NTF_CFG) && (((ff50s_env->features ) & 0x01) == FF50_FF51_LVL_NTF_SUP))
    {
        handle += FF50S_IDX_FF51_LVL_NTF_CFG;           
    }             
    else
    {
        handle = ATT_INVALID_HDL;
    }
    

    return handle;
}

uint8_t ff50s_get_att_idx(uint16_t handle, uint8_t *att_idx)
{
    struct ff50s_env_tag* ff50s_env = PRF_ENV_GET(FF50S, ff50s);
    uint16_t hdl_cursor = ff50s_env->start_hdl;
    uint8_t status = PRF_APP_ERROR;

    // Browse list of services
    // handle must be greater than current index 
    // check if it's a mandatory index
    if(handle <= (hdl_cursor + FF50S_IDX_FF51_LVL_VAL))
    {
        *att_idx = handle -hdl_cursor;
        status = GAP_ERR_NO_ERROR;
        
    }
    hdl_cursor += FF50S_IDX_FF51_LVL_VAL;

    // check if it's a notify index
    if(((ff50s_env->features ) & 0x01) == FF50_FF51_LVL_NTF_SUP)
    {
        hdl_cursor++;
        if(handle == hdl_cursor)
        {
            *att_idx = FF50S_IDX_FF51_LVL_NTF_CFG;
            status = GAP_ERR_NO_ERROR;
        }
    }
    hdl_cursor++;
    
    return (status);
}


#endif // (BLE_ff50_SERVER)


 
