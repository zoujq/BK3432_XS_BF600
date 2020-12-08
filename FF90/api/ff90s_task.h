/**
 ****************************************************************************************
 *
 * @file ff90s_task.h
 *
 * @brief Header file - Battery Service Server Role Task.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */


#ifndef _FF90S_TASK_H_
#define _FF90S_TASK_H_


#include "rwprf_config.h"
#if (BLE_FF90_SERVER)
#include <stdint.h>
#include "rwip_task.h" // Task definitions
/*
 * DEFINES
 ****************************************************************************************
 */

///Maximum number of FF90 Server task instances
#define FF90S_IDX_MAX     0x01
///Maximal number of FF90 that can be added in the DB

#define  FF90_FF91_DATA_LEN  5
#define  FF90_FF92_DATA_LEN  5
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the FF90S task
enum ff90s_state
{
    /// Idle state
    FF90S_IDLE,
    /// busy state
    FF90S_BUSY,
    /// Number of defined states.
    FF90S_STATE_MAX
};

/// Messages for FF90 Server
enum ff90s_msg_id
{
    /// Start the FF90 Server - at connection used to restore bond data
	FF90S_CREATE_DB_REQ   = TASK_FIRST_MSG(TASK_ID_FF90S),
	
    /// FF91 Level Value Update Request
    FF90S_FF91_LEVEL_UPD_REQ,
    /// Inform APP if FF91 Level value has been notified or not
    FF90S_FF91_LEVEL_UPD_RSP,
    /// Inform APP that FF91 Level Notification Configuration has been changed - use to update bond data
    FF90S_FF91_LEVEL_NTF_CFG_IND,
	
	FF90S_FF92_WRITER_REQ_IND,

	FF90S_FF91_LEVEL_PERIOD_NTF
	
		
};

/// Features Flag Masks
enum ff90s_features
{
    /// FF91 Level Characteristic doesn't support notifications
    FF90_FF91_LVL_NTF_NOT_SUP,
    /// FF91 Level Characteristic support notifications
    FF90_FF91_LVL_NTF_SUP,
};
/*
 * APIs Structures
 ****************************************************************************************
 */

/// Parameters for the database creation
struct ff90s_db_cfg
{
    /// Number of FF90 to add
    uint8_t ff90_nb;
    /// Features of each FF90 instance
    uint8_t features;
   };

/// Parameters of the @ref FF90S_ENABLE_REQ message
struct ff90s_enable_req
{
    /// connection index
    uint8_t  conidx;
    /// Notification Configuration
    uint8_t  ntf_cfg;
    /// Old FF91 Level used to decide if notification should be triggered
    uint8_t  old_ff91_lvl;
};


///Parameters of the @ref FF90S_BATT_LEVEL_UPD_REQ message
struct ff90s_ff91_level_upd_req
{
    /// BAS instance
    uint8_t conidx;
	
	uint8_t length;
    /// ff91 Level
    uint8_t ff91_level[FF90_FF91_DATA_LEN];
};

///Parameters of the @ref FF90S_FF91_LEVEL_UPD_RSP message
struct ff90s_ff91_level_upd_rsp
{
    ///status
    uint8_t status;
};

///Parameters of the @ref BASS_BATT_LEVEL_NTF_CFG_IND message
struct ff90s_ff91_level_ntf_cfg_ind
{
    /// connection index
    uint8_t  conidx;
    ///Notification Configuration
    uint8_t  ntf_cfg;
};


/// Parameters of the @ref FF90S_FF92_WRITER_REQ_IND message
struct ff90s_ff92_writer_ind
{
    /// Alert level
    uint8_t ff92_value[FF90_FF92_DATA_LEN];
	
	uint8_t length;
    /// Connection index
    uint8_t conidx;
};


/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler ff90s_default_handler;
#endif // BLE_FF90_SERVER


#endif /* _FF90S_TASK_H_ */

