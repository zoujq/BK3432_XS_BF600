/**
 ****************************************************************************************
 *
 * @file fee0s.h
 *
 * @brief Header file - FFF0 Service Server Role
 *
 * Copyright (C) beken 2009-2015
 *
 *
 ****************************************************************************************
 */
#ifndef _FEE0S_H_
#define _FEE0S_H_

/**
 ****************************************************************************************
 * @addtogroup  FFF0 'Profile' Server
 * @ingroup FFF0
 * @brief FFF0 'Profile' Server
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "rwprf_config.h"

#if (BLE_FEE0_SERVER)

#include "fee0s_task.h"
#include "atts.h"
#include "prf_types.h"
#include "prf.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define FEE0S_CFG_FLAG_MANDATORY_MASK       (0x7FFF)
#define FEE0_CFG_FLAG_NTF_SUP_MASK          (0x08)
#define FEE0_CFG_FLAG_MTP_FEE1_MASK         (0x40)

#define FEE1_LVL_MAX               			(100)

#define FEE1_FLAG_NTF_CFG_BIT             (0x02)



enum
{		
		ATT_USER_SERVER_FEE0 						= ATT_UUID_16(0x181C), 		
		
		ATT_USER_SERVER_CHAR_FEE2					= ATT_UUID_16(0x2a85),
		
		ATT_USER_SERVER_CHAR_FEE4					= ATT_UUID_16(0x2a8c),

		ATT_USER_SERVER_CHAR_FEE5					= ATT_UUID_16(0x2a8e),
		
		ATT_USER_SERVER_CHAR_FEE1                   = ATT_UUID_16(0x2a99),

		ATT_USER_SERVER_CHAR_FEE6					= ATT_UUID_16(0x2a9a),

		ATT_USER_SERVER_CHAR_FEE3					= ATT_UUID_16(0x2a9f),
		
};


/// Battery Service Attributes Indexes
enum
{
	FEE0S_IDX_SVC,


	FEE0S_IDX_FEE2_LVL_CHAR,
	FEE0S_IDX_FEE2_LVL_VAL,

	FEE0S_IDX_FEE4_LVL_CHAR,
	FEE0S_IDX_FEE4_LVL_VAL,

	FEE0S_IDX_FEE5_LVL_CHAR,
	FEE0S_IDX_FEE5_LVL_VAL,

	FEE0S_IDX_FEE1_LVL_CHAR,
	FEE0S_IDX_FEE1_LVL_VAL,
	FEE0S_IDX_FEE1_LVL_NTF_CFG,


	FEE0S_IDX_FEE6_LVL_CHAR,
	FEE0S_IDX_FEE6_LVL_VAL,


	FEE0S_IDX_FEE3_LVL_CHAR,
	FEE0S_IDX_FEE3_LVL_VAL,
	FEE0S_IDX_FEE3_LVL_NTF_CFG,	

	FEE0S_IDX_NB,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// FFF0 'Profile' Server environment variable
struct fee0s_env_tag
{
    /// profile environment
    prf_env_t prf_env;
   
    /// On-going operation
    struct ke_msg * operation;
    /// FFF0 Services Start Handle
    uint16_t start_hdl;
    /// Level of the FFF1
    uint8_t fee1_value[FEE0_FEE1_DATA_LEN];

    uint8_t fee2_value[FEE0_FEE2_DATA_LEN];

	uint8_t fee3_value[FEE0_FEE3_DATA_LEN];
	
	uint8_t fee4_value[FEE0_FEE4_DATA_LEN];

	uint8_t fee5_value[FEE0_FEE5_DATA_LEN];

	uint8_t fee6_value[FEE0_FEE6_DATA_LEN];
	
    /// BASS task state
    ke_state_t state[FEE0S_IDX_MAX];
    /// Notification configuration of peer devices.
    uint8_t ntf_cfg[BLE_CONNECTION_MAX];
    /// Database features
    uint8_t features;

};



/**
 ****************************************************************************************
 * @brief Retrieve fff0 service profile interface
 *
 * @return fff0 service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* fee0s_prf_itf_get(void);

uint16_t fee0s_get_att_handle(uint8_t att_idx);

uint8_t  fee0s_get_att_idx(uint16_t handle, uint8_t *att_idx);

void fee0s_notify_fee1_lvl(struct fee0s_env_tag* fee0s_env, struct fee0s_fee1_level_upd_req const *param);

void fee0s_notify_fee3_lvl(struct fee0s_env_tag* fee0s_env, struct fee0s_fee3_level_upd_req const *param);

#endif /* #if (BLE_FFF0_SERVERs) */



#endif /*  _FFF0_H_ */



