/**
 ****************************************************************************************
 *
 * @file ff50s.h
 *
 * @brief Header file - FF50 Service Server Role
 *
 * Copyright (C) beken 2009-2015
 *
 *
 ****************************************************************************************
 */
#ifndef _FF50S_H_
#define _FF50S_H_

/**
 ****************************************************************************************
 * @addtogroup  FF50 'Profile' Server
 * @ingroup FF50
 * @brief FF50 'Profile' Server
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "rwprf_config.h"

#if (BLE_FF50_SERVER)

#include "ff50s_task.h"
#include "atts.h"
#include "prf_types.h"
#include "prf.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define FF50S_CFG_FLAG_MANDATORY_MASK       (0x3F)
#define FF50_CFG_FLAG_NTF_SUP_MASK         (0x08)
#define FF50_CFG_FLAG_MTP_FF51_MASK         (0x40)

#define FF51_LVL_MAX               			(100)

#define FF51_FLAG_NTF_CFG_BIT             (0x02)



enum
{		
		ATT_USER_SERVER_FF50 						= ATT_UUID_16(0x181D),

		ATT_USER_SERVER_CHAR_FF52					= ATT_UUID_16(0x2a9e),
	  
		ATT_USER_SERVER_CHAR_FF51       			= ATT_UUID_16(0x2a9d),
		
		
		
};

/// Battery Service Attributes Indexes
enum
{
	FF50S_IDX_SVC,

	FF50S_IDX_FF52_LVL_CHAR,
	FF50S_IDX_FF52_LVL_VAL,

	FF50S_IDX_FF51_LVL_CHAR,
	FF50S_IDX_FF51_LVL_VAL,
	FF50S_IDX_FF51_LVL_NTF_CFG,

	FF50S_IDX_NB,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// FF50 'Profile' Server environment variable
struct ff50s_env_tag
{
    /// profile environment
    prf_env_t prf_env;
   
    /// On-going operation
    struct ke_msg * operation;
    /// FF50 Services Start Handle
    uint16_t start_hdl;
    /// Level of the FF51
    uint8_t ff51_lvl[FF50_FF51_DATA_LEN];
	
	uint8_t ff52_value[FF50_FF52_DATA_LEN];
    /// BASS task state
    ke_state_t state[FF50S_IDX_MAX];
    /// Notification configuration of peer devices.
    uint8_t ntf_cfg[BLE_CONNECTION_MAX];
    /// Database features
    uint8_t features;

};



/**
 ****************************************************************************************
 * @brief Retrieve ff50 service profile interface
 *
 * @return ff50 service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* ff50s_prf_itf_get(void);

uint16_t ff50s_get_att_handle(uint8_t att_idx);

uint8_t  ff50s_get_att_idx(uint16_t handle, uint8_t *att_idx);

void ff50s_notify_ff51_lvl(struct ff50s_env_tag* ff50s_env, struct ff50s_ff51_level_upd_req const *param);

#endif /* #if (BLE_FF50_SERVER) */



#endif /*  _FF50_H_ */



