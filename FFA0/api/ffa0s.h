/**
 ****************************************************************************************
 *
 * @file ffa0s.h
 *
 * @brief Header file - FFA0 Service Server Role
 *
 * Copyright (C) beken 2009-2015
 *
 *
 ****************************************************************************************
 */
#ifndef _FFA0S_H_
#define _FFA0S_H_

/**
 ****************************************************************************************
 * @addtogroup  FFA0 'Profile' Server
 * @ingroup FFA0
 * @brief FFA0 'Profile' Server
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "rwprf_config.h"

#if (BLE_FFA0_SERVER)

#include "ffa0s_task.h"
#include "atts.h"
#include "prf_types.h"
#include "prf.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define FFA0S_CFG_FLAG_MANDATORY_MASK       (0x3F)
#define FFA0_CFG_FLAG_NTF_SUP_MASK         (0x08)
#define FFA0_CFG_FLAG_MTP_FFA1_MASK         (0x40)

#define FFA1_LVL_MAX               			(100)

#define FFA1_FLAG_NTF_CFG_BIT             (0x02)



enum
{		
		ATT_USER_SERVER_FFA0 						= ATT_UUID_16(0xFFA0),
	  
		ATT_USER_SERVER_CHAR_FFA1       = ATT_UUID_16(0xFFA1),
		
		ATT_USER_SERVER_CHAR_FFA2				= ATT_UUID_16(0xFFA2),
		
};

/// Battery Service Attributes Indexes
enum
{
	FFA0S_IDX_SVC,

	FFA0S_IDX_FFA2_LVL_CHAR,
	FFA0S_IDX_FFA2_LVL_VAL,

	FFA0S_IDX_FFA1_LVL_CHAR,
	FFA0S_IDX_FFA1_LVL_VAL,
	FFA0S_IDX_FFA1_LVL_NTF_CFG,

	FFA0S_IDX_NB,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// FFA0 'Profile' Server environment variable
struct ffa0s_env_tag
{
    /// profile environment
    prf_env_t prf_env;
   
    /// On-going operation
    struct ke_msg * operation;
    /// FFA0 Services Start Handle
    uint16_t start_hdl;
    /// Level of the FFA1
    uint8_t ffa1_lvl[FFA0_FFA1_DATA_LEN];
	
	uint8_t ffa2_value[FFA0_FFA2_DATA_LEN];
    /// BASS task state
    ke_state_t state[FFA0S_IDX_MAX];
    /// Notification configuration of peer devices.
    uint8_t ntf_cfg[BLE_CONNECTION_MAX];
    /// Database features
    uint8_t features;

};



/**
 ****************************************************************************************
 * @brief Retrieve ffa0 service profile interface
 *
 * @return ffa0 service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* ffa0s_prf_itf_get(void);

uint16_t ffa0s_get_att_handle(uint8_t att_idx);

uint8_t  ffa0s_get_att_idx(uint16_t handle, uint8_t *att_idx);

void ffa0s_notify_ffa1_lvl(struct ffa0s_env_tag* ffa0s_env, struct ffa0s_ffa1_level_upd_req const *param);

#endif /* #if (BLE_FFA0_SERVER) */



#endif /*  _FFA0_H_ */



