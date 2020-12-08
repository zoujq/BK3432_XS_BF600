/**
 ****************************************************************************************
 *
 * @file ff80s_task.h
 *
 * @brief Header file - Battery Service Server Role Task.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */


#ifndef _FF80S_TASK_H_
#define _FF80S_TASK_H_


#include "rwprf_config.h"
#if (BLE_FF80_SERVER)
#include <stdint.h>
#include "rwip_task.h" // Task definitions
/*
 * DEFINES
 ****************************************************************************************
 */

///Maximum number of FF80 Server task instances
#define FF80S_IDX_MAX     0x01
///Maximal number of FF80 that can be added in the DB

#define  FF80_FF81_DATA_LEN  5
#define  FF80_FF82_DATA_LEN  5
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the FF80S task
enum ff80s_state
{
    /// Idle state
    FF80S_IDLE,
    /// busy state
    FF80S_BUSY,
    /// Number of defined states.
    FF80S_STATE_MAX
};

/// Messages for FF80 Server
enum ff80s_msg_id
{
    /// Start the FF80 Server - at connection used to restore bond data
	FF80S_CREATE_DB_REQ   = TASK_FIRST_MSG(TASK_ID_FF80S),
	
    /// FF81 Level Value Update Request
    FF80S_FF81_LEVEL_UPD_REQ,
    /// Inform APP if FF81 Level value has been notified or not
    FF80S_FF81_LEVEL_UPD_RSP,
    /// Inform APP that FF81 Level Notification Configuration has been changed - use to update bond data
    FF80S_FF81_LEVEL_NTF_CFG_IND,
	
	FF80S_FF82_WRITER_REQ_IND,

	FF80S_FF81_LEVEL_PERIOD_NTF
	
		
};

/// Features Flag Masks
enum ff80s_features
{
    /// FF81 Level Characteristic doesn't support notifications
    FF80_FF81_LVL_NTF_NOT_SUP,
    /// FF81 Level Characteristic support notifications
    FF80_FF81_LVL_NTF_SUP,
};
/*
 * APIs Structures
 ****************************************************************************************
 */

/// Parameters for the database creation
struct ff80s_db_cfg
{
    /// Number of FF80 to add
    uint8_t ff80_nb;
    /// Features of each FF80 instance
    uint8_t features;
   };

/// Parameters of the @ref FF80S_ENABLE_REQ message
struct ff80s_enable_req
{
    /// connection index
    uint8_t  conidx;
    /// Notification Configuration
    uint8_t  ntf_cfg;
    /// Old FF81 Level used to decide if notification should be triggered
    uint8_t  old_ff81_lvl;
};


///Parameters of the @ref FF80S_BATT_LEVEL_UPD_REQ message
struct ff80s_ff81_level_upd_req
{
    /// BAS instance
    uint8_t conidx;
	
	uint8_t length;
    /// ff81 Level
    uint8_t ff81_level[FF80_FF81_DATA_LEN];
};

///Parameters of the @ref FF80S_FF81_LEVEL_UPD_RSP message
struct ff80s_ff81_level_upd_rsp
{
    ///status
    uint8_t status;
};

///Parameters of the @ref BASS_BATT_LEVEL_NTF_CFG_IND message
struct ff80s_ff81_level_ntf_cfg_ind
{
    /// connection index
    uint8_t  conidx;
    ///Notification Configuration
    uint8_t  ntf_cfg;
};


/// Parameters of the @ref FF80S_FF82_WRITER_REQ_IND message
struct ff80s_ff82_writer_ind
{
    /// Alert level
    uint8_t ff82_value[FF80_FF82_DATA_LEN];
	
	uint8_t length;
    /// Connection index
    uint8_t conidx;
};


/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler ff80s_default_handler;
#endif // BLE_FF80_SERVER


#endif /* _FF80S_TASK_H_ */

