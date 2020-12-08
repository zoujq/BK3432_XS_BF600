/**
 ****************************************************************************************
 *
 * @file ffa0s.c
 *
 * @brief ffa0 Server Implementation.
 *
 * Copyright (C) beken 2009-2015
 *
 *
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_FFA0_SERVER)
#include "attm.h"
#include "ffa0s.h"
#include "ffa0s_task.h"
#include "prf_utils.h"
#include "prf.h"
#include "ke_mem.h"

#include "uart.h"



/*
 * FFA0 ATTRIBUTES DEFINITION
 ****************************************************************************************
 */
 
/// Full FFA0 Database Description - Used to add attributes into the database
const struct attm_desc ffa0_att_db[FFA0S_IDX_NB] =
{
    // FFA0 Service Declaration
    [FFA0S_IDX_SVC]            =   {ATT_DECL_PRIMARY_SERVICE, PERM(RD, ENABLE), 0, 0},

	[FFA0S_IDX_FFA2_LVL_CHAR]  =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},
    //  Characteristic Value
    [FFA0S_IDX_FFA2_LVL_VAL]   =   {ATT_USER_SERVER_CHAR_FFA2,PERM(WRITE_COMMAND, ENABLE), PERM(RI, ENABLE), FFA0_FFA2_DATA_LEN *sizeof(uint8_t)},

	// ffa1 Level Characteristic Declaration
	[FFA0S_IDX_FFA1_LVL_CHAR]  =   {ATT_DECL_CHARACTERISTIC, PERM(RD, ENABLE), 0, 0},
    // ffa1 Level Characteristic Value
    [FFA0S_IDX_FFA1_LVL_VAL]   =   {ATT_USER_SERVER_CHAR_FFA1, PERM(WRITE_COMMAND, ENABLE) , PERM(RI, ENABLE), FFA0_FFA1_DATA_LEN * sizeof(uint8_t)},

	// ffa1 Level Characteristic - Client Characteristic Configuration Descriptor
	[FFA0S_IDX_FFA1_LVL_NTF_CFG] = {ATT_DESC_CLIENT_CHAR_CFG,  PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 0, 0},

};/// Macro used to retrieve permission value from access and rights on attribute.


static uint8_t ffa0s_init (struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl,  struct ffa0s_db_cfg* params)
{
    uint16_t shdl;
    struct ffa0s_env_tag* ffa0s_env = NULL;
    // Status
    uint8_t status = GAP_ERR_NO_ERROR;
    
    //-------------------- allocate memory required for the profile  ---------------------
    ffa0s_env = (struct ffa0s_env_tag* ) ke_malloc(sizeof(struct ffa0s_env_tag), KE_MEM_ATT_DB);
    memset(ffa0s_env, 0 , sizeof(struct ffa0s_env_tag));

    // Service content flag
    uint8_t cfg_flag = FFA0S_CFG_FLAG_MANDATORY_MASK;

    // Save database configuration
    ffa0s_env->features |= (params->features) ;
   
    // Check if notifications are supported
    if (params->features == FFA0_FFA1_LVL_NTF_SUP)
    {
        cfg_flag |= FFA0_CFG_FLAG_NTF_SUP_MASK;
    }
    shdl = *start_hdl;

    //Create FFA0 in the DB
    //------------------ create the attribute database for the profile -------------------
    status = attm_svc_create_db(&(shdl), ATT_USER_SERVER_FFA0, (uint8_t *)&cfg_flag,
            FFA0S_IDX_NB, NULL, env->task, &ffa0_att_db[0],
            (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_AUTH | PERM_MASK_SVC_EKS)));
				


    //Set optional permissions
    if (status == GAP_ERR_NO_ERROR)
    {
        //Set optional permissions
        if(params->features == FFA0_FFA1_LVL_NTF_SUP)
        {
            // Battery Level characteristic value permissions
            uint16_t perm = PERM(RD, ENABLE) | PERM(NTF, ENABLE);

            attm_att_set_permission(shdl + FFA0S_IDX_FFA1_LVL_VAL, perm, 0);
        }
    }

    //-------------------- Update profile task information  ---------------------
    if (status == ATT_ERR_NO_ERROR)
    {

        // allocate BASS required environment variable
        env->env = (prf_env_t*) ffa0s_env;
        *start_hdl = shdl;
        ffa0s_env->start_hdl = *start_hdl;
        ffa0s_env->prf_env.app_task = app_task
                | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        ffa0s_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id                     = TASK_ID_FFA0S;
        env->desc.idx_max           = FFA0S_IDX_MAX;
        env->desc.state             = ffa0s_env->state;
        env->desc.default_handler   = &ffa0s_default_handler;

        // service is ready, go into an Idle state
        ke_state_set(env->task, FFA0S_IDLE);
    }
    else if(ffa0s_env != NULL)
    {
        ke_free(ffa0s_env);
    }
     
    return (status);
}


static void ffa0s_destroy(struct prf_task_env* env)
{
    struct ffa0s_env_tag* ffa0s_env = (struct ffa0s_env_tag*) env->env;

    // clear on-going operation
    if(ffa0s_env->operation != NULL)
    {
        ke_free(ffa0s_env->operation);
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(ffa0s_env);
}

static void ffa0s_create(struct prf_task_env* env, uint8_t conidx)
{
    struct ffa0s_env_tag* ffa0s_env = (struct ffa0s_env_tag*) env->env;
    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);

    // force notification config to zero when peer device is connected
    ffa0s_env->ntf_cfg[conidx] = 0;
}


static void ffa0s_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct ffa0s_env_tag* ffa0s_env = (struct ffa0s_env_tag*) env->env;

    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);
    // force notification config to zero when peer device is disconnected
    ffa0s_env->ntf_cfg[conidx] = 0;
}


void ffa0s_notify_ffa1_lvl(struct ffa0s_env_tag* ffa0s_env, struct ffa0s_ffa1_level_upd_req const *param)
{
    // Allocate the GATT notification message
    struct gattc_send_evt_cmd *ffa1_lvl = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
            KE_BUILD_ID(TASK_GATTC, 0), prf_src_task_get(&(ffa0s_env->prf_env),0),
            gattc_send_evt_cmd, sizeof(uint8_t)* (param->length));

    // Fill in the parameter structure
    ffa1_lvl->operation = GATTC_NOTIFY;
    ffa1_lvl->handle = ffa0s_get_att_handle(FFA0S_IDX_FFA1_LVL_VAL);
    // pack measured value in database
    ffa1_lvl->length = param->length;
  	//ffa1_lvl->value[0] = ffa0s_env->ffa1_lvl[0];
		memcpy(&ffa1_lvl->value[0],&param->ffa1_level[0],param->length);
    // send notification to peer device
    ke_msg_send(ffa1_lvl);
}



/// BASS Task interface required by profile manager
const struct prf_task_cbs ffa0s_itf =
{
        (prf_init_fnct) ffa0s_init,
        ffa0s_destroy,
        ffa0s_create,
        ffa0s_cleanup,
};


const struct prf_task_cbs* ffa0s_prf_itf_get(void)
{
   return &ffa0s_itf;
}


uint16_t ffa0s_get_att_handle( uint8_t att_idx)
{
		
    struct ffa0s_env_tag *ffa0s_env = PRF_ENV_GET(FFA0S, ffa0s);
    uint16_t handle = ATT_INVALID_HDL;
   
    handle = ffa0s_env->start_hdl;

    // increment index according to expected index
    if(att_idx < FFA0S_IDX_FFA1_LVL_NTF_CFG)
    {
        handle += att_idx;
    }
    // FFA1 notification
    else if((att_idx == FFA0S_IDX_FFA1_LVL_NTF_CFG) && (((ffa0s_env->features ) & 0x01) == FFA0_FFA1_LVL_NTF_SUP))
    {
        handle += FFA0S_IDX_FFA1_LVL_NTF_CFG;			
    }		      
    else
    {
        handle = ATT_INVALID_HDL;
    }
    

    return handle;
}

uint8_t ffa0s_get_att_idx(uint16_t handle, uint8_t *att_idx)
{
    struct ffa0s_env_tag* ffa0s_env = PRF_ENV_GET(FFA0S, ffa0s);
    uint16_t hdl_cursor = ffa0s_env->start_hdl;
    uint8_t status = PRF_APP_ERROR;

    // Browse list of services
    // handle must be greater than current index 
    // check if it's a mandatory index
    if(handle <= (hdl_cursor + FFA0S_IDX_FFA1_LVL_VAL))
    {
        *att_idx = handle -hdl_cursor;
        status = GAP_ERR_NO_ERROR;
        
    }
    hdl_cursor += FFA0S_IDX_FFA1_LVL_VAL;

    // check if it's a notify index
    if(((ffa0s_env->features ) & 0x01) == FFA0_FFA1_LVL_NTF_SUP)
    {
        hdl_cursor++;
        if(handle == hdl_cursor)
        {
            *att_idx = FFA0S_IDX_FFA1_LVL_NTF_CFG;
            status = GAP_ERR_NO_ERROR;
        }
    }
    hdl_cursor++;
    
    return (status);
}


#endif // (BLE_ffa0_SERVER)


 
