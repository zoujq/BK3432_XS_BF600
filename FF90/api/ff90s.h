/**
 ****************************************************************************************
 *
 * @file ff90s.h
 *
 * @brief Header file - FF90 Service Server Role
 *
 * Copyright (C) beken 2009-2015
 *
 *
 ****************************************************************************************
 */
#ifndef _FF90S_H_
#define _FF90S_H_

/**
 ****************************************************************************************
 * @addtogroup  FF90 'Profile' Server
 * @ingroup FF90
 * @brief FF90 'Profile' Server
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "rwprf_config.h"

#if (BLE_FF90_SERVER)

#include "ff90s_task.h"
#include "atts.h"
#include "prf_types.h"
#include "prf.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define FF90S_CFG_FLAG_MANDATORY_MASK       (0x3F)
#define FF90_CFG_FLAG_NTF_SUP_MASK         (0x08)
#define FF90_CFG_FLAG_MTP_FF91_MASK         (0x40)

#define FF91_LVL_MAX               			(100)

#define FF91_FLAG_NTF_CFG_BIT             (0x02)



enum
{		
		ATT_USER_SERVER_FF90 						= ATT_UUID_16(0xFF90),
	  
		ATT_USER_SERVER_CHAR_FF91       = ATT_UUID_16(0xFF91),
		
		ATT_USER_SERVER_CHAR_FF92				= ATT_UUID_16(0xFF92),
		
};

/// Battery Service Attributes Indexes
enum
{
	FF90S_IDX_SVC,

	FF90S_IDX_FF92_LVL_CHAR,
	FF90S_IDX_FF92_LVL_VAL,

	FF90S_IDX_FF91_LVL_CHAR,
	FF90S_IDX_FF91_LVL_VAL,
	FF90S_IDX_FF91_LVL_NTF_CFG,

	FF90S_IDX_NB,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// FF90 'Profile' Server environment variable
struct ff90s_env_tag
{
    /// profile environment
    prf_env_t prf_env;
   
    /// On-going operation
    struct ke_msg * operation;
    /// FF90 Services Start Handle
    uint16_t start_hdl;
    /// Level of the FF91
    uint8_t ff91_lvl[FF90_FF91_DATA_LEN];
	
	uint8_t ff92_value[FF90_FF92_DATA_LEN];
    /// BASS task state
    ke_state_t state[FF90S_IDX_MAX];
    /// Notification configuration of peer devices.
    uint8_t ntf_cfg[BLE_CONNECTION_MAX];
    /// Database features
    uint8_t features;

};



/**
 ****************************************************************************************
 * @brief Retrieve ff90 service profile interface
 *
 * @return ff90 service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* ff90s_prf_itf_get(void);

uint16_t ff90s_get_att_handle(uint8_t att_idx);

uint8_t  ff90s_get_att_idx(uint16_t handle, uint8_t *att_idx);

void ff90s_notify_ff91_lvl(struct ff90s_env_tag* ff90s_env, struct ff90s_ff91_level_upd_req const *param);

#endif /* #if (BLE_FF90_SERVER) */



#endif /*  _FF90_H_ */



