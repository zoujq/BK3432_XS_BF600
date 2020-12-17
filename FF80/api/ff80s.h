/**
 ****************************************************************************************
 *
 * @file ff80s.h
 *
 * @brief Header file - FF80 Service Server Role
 *
 * Copyright (C) beken 2009-2015
 *
 *
 ****************************************************************************************
 */
#ifndef _FF80S_H_
#define _FF80S_H_

/**
 ****************************************************************************************
 * @addtogroup  FF80 'Profile' Server
 * @ingroup FF80
 * @brief FF80 'Profile' Server
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "rwprf_config.h"

#if (BLE_FF80_SERVER)

#include "ff80s_task.h"
#include "atts.h"
#include "prf_types.h"
#include "prf.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define FF80S_CFG_FLAG_MANDATORY_MASK       (0xFF)
#define FF80_CFG_FLAG_NTF_SUP_MASK         (0x08)
#define FF80_CFG_FLAG_MTP_FF81_MASK         (0x40)

#define FF81_LVL_MAX               			(100)

#define FF81_FLAG_NTF_CFG_BIT             (0x02)



enum
{		
		ATT_USER_SERVER_FF80 						= ATT_UUID_16(0x1805),
	  
		ATT_USER_SERVER_CHAR_FF81       			= ATT_UUID_16(0x2a2b),
		
		ATT_USER_SERVER_CHAR_FF82					= ATT_UUID_16(0x2a0f),

		ATT_USER_SERVER_CHAR_FF83					= ATT_UUID_16(0x2a14),
		
};

/// Battery Service Attributes Indexes
enum
{
	FF80S_IDX_SVC,

	FF80S_IDX_FF81_LVL_CHAR,
	FF80S_IDX_FF81_LVL_VAL,
	FF80S_IDX_FF81_LVL_NTF_CFG,

	FF80S_IDX_FF82_LVL_CHAR,
	FF80S_IDX_FF82_LVL_VAL,

	FF80S_IDX_FF83_LVL_CHAR,
	FF80S_IDX_FF83_LVL_VAL,


	FF80S_IDX_NB,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// FF80 'Profile' Server environment variable
struct ff80s_env_tag
{
    /// profile environment
    prf_env_t prf_env;
   
    /// On-going operation
    struct ke_msg * operation;
    /// FF80 Services Start Handle
    uint16_t start_hdl;
    /// Level of the FF81
    uint8_t ff81_value[FF80_FF81_DATA_LEN];
	
	uint8_t ff82_value[FF80_FF82_DATA_LEN];
    /// BASS task state
    ke_state_t state[FF80S_IDX_MAX];
    /// Notification configuration of peer devices.
    uint8_t ntf_cfg[BLE_CONNECTION_MAX];
    /// Database features
    uint8_t features;

};



/**
 ****************************************************************************************
 * @brief Retrieve ff80 service profile interface
 *
 * @return ff80 service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* ff80s_prf_itf_get(void);

uint16_t ff80s_get_att_handle(uint8_t att_idx);

uint8_t  ff80s_get_att_idx(uint16_t handle, uint8_t *att_idx);

void ff80s_notify_ff81_lvl(struct ff80s_env_tag* ff80s_env, struct ff80s_ff81_level_upd_req const *param);

#endif /* #if (BLE_FF80_SERVER) */



#endif /*  _FF80_H_ */



