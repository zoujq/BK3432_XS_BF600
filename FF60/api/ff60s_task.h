/**
 ****************************************************************************************
 *
 * @file ff60s_task.h
 *
 * @brief Header file - Battery Service Server Role Task.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */


#ifndef _FF60S_TASK_H_
#define _FF60S_TASK_H_


#include "rwprf_config.h"
#if (BLE_FF60_SERVER)
#include <stdint.h>
#include "rwip_task.h" // Task definitions
/*
 * DEFINES
 ****************************************************************************************
 */

///Maximum number of FF60 Server task instances
#define FF60S_IDX_MAX     0x01
///Maximal number of FF60 that can be added in the DB

#define  FF60_FF61_DATA_LEN  5
#define  FF60_FF62_DATA_LEN  5
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the FF60S task
enum ff60s_state
{
    /// Idle state
    FF60S_IDLE,
    /// busy state
    FF60S_BUSY,
    /// Number of defined states.
    FF60S_STATE_MAX
};

/// Messages for FF60 Server
enum ff60s_msg_id
{
    /// Start the FF60 Server - at connection used to restore bond data
	FF60S_CREATE_DB_REQ   = TASK_FIRST_MSG(TASK_ID_FF60S),
	
    /// FF61 Level Value Update Request
    FF60S_FF61_LEVEL_UPD_REQ,
    /// Inform APP if FF61 Level value has been notified or not
    FF60S_FF61_LEVEL_UPD_RSP,
    /// Inform APP that FF61 Level Notification Configuration has been changed - use to update bond data
    FF60S_FF61_LEVEL_NTF_CFG_IND,
	
	FF60S_FF62_WRITER_REQ_IND,

	FF60S_FF61_LEVEL_PERIOD_NTF
	
		
};

/// Features Flag Masks
enum ff60s_features
{
    /// FF61 Level Characteristic doesn't support notifications
    FF60_FF61_LVL_NTF_NOT_SUP,
    /// FF61 Level Characteristic support notifications
    FF60_FF61_LVL_NTF_SUP,
};
/*
 * APIs Structures
 ****************************************************************************************
 */

/// Parameters for the database creation
struct ff60s_db_cfg
{
    /// Number of FF60 to add
    uint8_t ff60_nb;
    /// Features of each FF60 instance
    uint8_t features;
   };

/// Parameters of the @ref FF60S_ENABLE_REQ message
struct ff60s_enable_req
{
    /// connection index
    uint8_t  conidx;
    /// Notification Configuration
    uint8_t  ntf_cfg;
    /// Old FF61 Level used to decide if notification should be triggered
    uint8_t  old_ff61_lvl;
};


///Parameters of the @ref FF60S_BATT_LEVEL_UPD_REQ message
struct ff60s_ff61_level_upd_req
{
    /// BAS instance
    uint8_t conidx;
	
	uint8_t length;
    /// ff61 Level
    uint8_t ff61_level[FF60_FF61_DATA_LEN];
};

///Parameters of the @ref FF60S_FF61_LEVEL_UPD_RSP message
struct ff60s_ff61_level_upd_rsp
{
    ///status
    uint8_t status;
};

///Parameters of the @ref BASS_BATT_LEVEL_NTF_CFG_IND message
struct ff60s_ff61_level_ntf_cfg_ind
{
    /// connection index
    uint8_t  conidx;
    ///Notification Configuration
    uint8_t  ntf_cfg;
};


/// Parameters of the @ref FF60S_FF62_WRITER_REQ_IND message
struct ff60s_ff62_writer_ind
{
    /// Alert level
    uint8_t ff62_value[FF60_FF62_DATA_LEN];
	
	uint8_t length;
    /// Connection index
    uint8_t conidx;
};


/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler ff60s_default_handler;
#endif // BLE_FF60_SERVER


#endif /* _FF60S_TASK_H_ */

