/**
 ****************************************************************************************
 *
 * @file ffa0s_task.h
 *
 * @brief Header file - Battery Service Server Role Task.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */


#ifndef _FFA0S_TASK_H_
#define _FFA0S_TASK_H_


#include "rwprf_config.h"
#if (BLE_FFA0_SERVER)
#include <stdint.h>
#include "rwip_task.h" // Task definitions
/*
 * DEFINES
 ****************************************************************************************
 */

///Maximum number of FFA0 Server task instances
#define FFA0S_IDX_MAX     0x01
///Maximal number of FFA0 that can be added in the DB

#define  FFA0_FFA1_DATA_LEN  5
#define  FFA0_FFA2_DATA_LEN  5
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Possible states of the FFA0S task
enum ffa0s_state
{
    /// Idle state
    FFA0S_IDLE,
    /// busy state
    FFA0S_BUSY,
    /// Number of defined states.
    FFA0S_STATE_MAX
};

/// Messages for FFA0 Server
enum ffa0s_msg_id
{
    /// Start the FFA0 Server - at connection used to restore bond data
	FFA0S_CREATE_DB_REQ   = TASK_FIRST_MSG(TASK_ID_FFA0S),
	
    /// FFA1 Level Value Update Request
    FFA0S_FFA1_LEVEL_UPD_REQ,
    /// Inform APP if FFA1 Level value has been notified or not
    FFA0S_FFA1_LEVEL_UPD_RSP,
    /// Inform APP that FFA1 Level Notification Configuration has been changed - use to update bond data
    FFA0S_FFA1_LEVEL_NTF_CFG_IND,
	
	FFA0S_FFA2_WRITER_REQ_IND,

	FFA0S_FFA1_LEVEL_PERIOD_NTF
	
		
};

/// Features Flag Masks
enum ffa0s_features
{
    /// FFA1 Level Characteristic doesn't support notifications
    FFA0_FFA1_LVL_NTF_NOT_SUP,
    /// FFA1 Level Characteristic support notifications
    FFA0_FFA1_LVL_NTF_SUP,
};
/*
 * APIs Structures
 ****************************************************************************************
 */

/// Parameters for the database creation
struct ffa0s_db_cfg
{
    /// Number of FFA0 to add
    uint8_t ffa0_nb;
    /// Features of each FFA0 instance
    uint8_t features;
   };

/// Parameters of the @ref FFA0S_ENABLE_REQ message
struct ffa0s_enable_req
{
    /// connection index
    uint8_t  conidx;
    /// Notification Configuration
    uint8_t  ntf_cfg;
    /// Old FFA1 Level used to decide if notification should be triggered
    uint8_t  old_ffa1_lvl;
};


///Parameters of the @ref FFA0S_BATT_LEVEL_UPD_REQ message
struct ffa0s_ffa1_level_upd_req
{
    /// BAS instance
    uint8_t conidx;
	
	uint8_t length;
    /// ffa1 Level
    uint8_t ffa1_level[FFA0_FFA1_DATA_LEN];
};

///Parameters of the @ref FFA0S_FFA1_LEVEL_UPD_RSP message
struct ffa0s_ffa1_level_upd_rsp
{
    ///status
    uint8_t status;
};

///Parameters of the @ref BASS_BATT_LEVEL_NTF_CFG_IND message
struct ffa0s_ffa1_level_ntf_cfg_ind
{
    /// connection index
    uint8_t  conidx;
    ///Notification Configuration
    uint8_t  ntf_cfg;
};


/// Parameters of the @ref FFA0S_FFA2_WRITER_REQ_IND message
struct ffa0s_ffa2_writer_ind
{
    /// Alert level
    uint8_t ffa2_value[FFA0_FFA2_DATA_LEN];
	
	uint8_t length;
    /// Connection index
    uint8_t conidx;
};


/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

extern const struct ke_state_handler ffa0s_default_handler;
#endif // BLE_FFA0_SERVER


#endif /* _FFA0S_TASK_H_ */

