/**
 ****************************************************************************************
 *
 * @file ff60s.h
 *
 * @brief Header file - FF60 Service Server Role
 *
 * Copyright (C) beken 2009-2015
 *
 *
 ****************************************************************************************
 */
#ifndef _FF60S_H_
#define _FF60S_H_

/**
 ****************************************************************************************
 * @addtogroup  FF60 'Profile' Server
 * @ingroup FF60
 * @brief FF60 'Profile' Server
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "rwprf_config.h"

#if (BLE_FF60_SERVER)

#include "ff60s_task.h"
#include "atts.h"
#include "prf_types.h"
#include "prf.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define FF60S_CFG_FLAG_MANDATORY_MASK       (0x3F)
#define FF60_CFG_FLAG_NTF_SUP_MASK         (0x08)
#define FF60_CFG_FLAG_MTP_FF61_MASK         (0x40)

#define FF61_LVL_MAX               			(100)

#define FF61_FLAG_NTF_CFG_BIT             (0x02)



enum
{		
		ATT_USER_SERVER_FF60 						= ATT_UUID_16(0x181B),
	  
		ATT_USER_SERVER_CHAR_FF61       			= ATT_UUID_16(0x2a9b),
		
		ATT_USER_SERVER_CHAR_FF62					= ATT_UUID_16(0x2a9c),
		
};

/// Battery Service Attributes Indexes
enum
{
	FF60S_IDX_SVC,

	FF60S_IDX_FF61_LVL_CHAR,
	FF60S_IDX_FF61_LVL_VAL,
	FF60S_IDX_FF61_LVL_NTF_CFG,
	
	FF60S_IDX_FF62_LVL_CHAR,
	FF60S_IDX_FF62_LVL_VAL,

	

	FF60S_IDX_NB,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// FF60 'Profile' Server environment variable
struct ff60s_env_tag
{
    /// profile environment
    prf_env_t prf_env;
   
    /// On-going operation
    struct ke_msg * operation;
    /// FF60 Services Start Handle
    uint16_t start_hdl;
    /// Level of the FF61
    uint8_t ff61_lvl[FF60_FF61_DATA_LEN];
	
	uint8_t ff62_value[FF60_FF62_DATA_LEN];
    /// BASS task state
    ke_state_t state[FF60S_IDX_MAX];
    /// Notification configuration of peer devices.
    uint8_t ntf_cfg[BLE_CONNECTION_MAX];
    /// Database features
    uint8_t features;

};



/**
 ****************************************************************************************
 * @brief Retrieve ff60 service profile interface
 *
 * @return ff60 service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* ff60s_prf_itf_get(void);

uint16_t ff60s_get_att_handle(uint8_t att_idx);

uint8_t  ff60s_get_att_idx(uint16_t handle, uint8_t *att_idx);

void ff60s_notify_ff61_lvl(struct ff60s_env_tag* ff60s_env, struct ff60s_ff61_level_upd_req const *param);

#endif /* #if (BLE_FF60_SERVER) */



#endif /*  _FF60_H_ */



