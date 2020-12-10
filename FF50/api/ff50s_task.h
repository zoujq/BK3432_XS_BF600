/**
 ****************************************************************************************
 *
 * @file ff50s_task.h
 *
 * @brief Header file - Battery Service Server Role Task.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */


#ifndef _FF50S_TASK_H_
#define _FF50S_TASK_H_


#include "rwprf_config.h"
#if (BLE_FF50_SERVER)
#include <stdint.h>
#include "rwip_task.h" // Task definitions
/*
 * DEFINES
 ****************************************************************************************
 */

///Maximum number of FF50 Server task instances
#define FF50S_IDX_MAX     0x01
///Maximal number of FF50 that can be added in the DB

#define  FF50_FF51_DATA_LEN  15
#define  FF50_FF52_DATA_LEN  4
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the FF50S task
enum ff50s_state
{
    /// Idle state
    FF50S_IDLE,
    /// busy state
    FF50S_BUSY,
    /// Number of defined states.
    FF50S_STATE_MAX
};

/// Messages for FF50 Server
enum ff50s_msg_id
{
    /// Start the FF50 Server - at connection used to restore bond data
    FF50S_CREATE_DB_REQ   = TASK_FIRST_MSG(TASK_ID_FF50S),
    
    /// FF51 Level Value Update Request
    FF50S_FF51_LEVEL_UPD_REQ,
    /// Inform APP if FF51 Level value has been notified or not
    FF50S_FF51_LEVEL_UPD_RSP,
    /// Inform APP that FF51 Level Notification Configuration has been changed - use to update bond data
    FF50S_FF51_LEVEL_NTF_CFG_IND,
    
    FF50S_FF52_WRITER_REQ_IND,

    FF50S_FF51_LEVEL_PERIOD_NTF
    
        
};

/// Features Flag Masks
enum ff50s_features
{
    /// FF51 Level Characteristic doesn't support notifications
    FF50_FF51_LVL_NTF_NOT_SUP,
    /// FF51 Level Characteristic support notifications
    FF50_FF51_LVL_NTF_SUP,
};
/*
 * APIs Structures
 ****************************************************************************************
 */

/// Parameters for the database creation
struct ff50s_db_cfg
{
    /// Number of FF50 to add
    uint8_t ff50_nb;
    /// Features of each FF50 instance
    uint8_t features;
   };

/// Parameters of the @ref FF50S_ENABLE_REQ message
struct ff50s_enable_req
{
    /// connection index
    uint8_t  conidx;
    /// Notification Configuration
    uint8_t  ntf_cfg;
    /// Old FF51 Level used to decide if notification should be triggered
    uint8_t  old_ff51_lvl;
};


///Parameters of the @ref FF50S_BATT_LEVEL_UPD_REQ message
struct ff50s_ff51_level_upd_req
{
    /// BAS instance
    uint8_t conidx;
    
    uint8_t length;
    /// ff51 Level
    uint8_t ff51_level[FF50_FF51_DATA_LEN];
};

///Parameters of the @ref FF50S_FF51_LEVEL_UPD_RSP message
struct ff50s_ff51_level_upd_rsp
{
    ///status
    uint8_t status;
};

///Parameters of the @ref BASS_BATT_LEVEL_NTF_CFG_IND message
struct ff50s_ff51_level_ntf_cfg_ind
{
    /// connection index
    uint8_t  conidx;
    ///Notification Configuration
    uint8_t  ntf_cfg;
};


/// Parameters of the @ref FF50S_FF52_WRITER_REQ_IND message
struct ff50s_ff52_writer_ind
{
    /// Alert level
    uint8_t ff52_value[FF50_FF52_DATA_LEN];
    
    uint8_t length;
    /// Connection index
    uint8_t conidx;
};


/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler ff50s_default_handler;
#endif // BLE_FF50_SERVER


#endif /* _FF50S_TASK_H_ */

