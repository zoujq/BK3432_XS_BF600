/**
 ****************************************************************************************
 *
 * @file app_ff70.c
 *
 * @brief findt Application Module entry point
 *
 * @auth  gang.cheng
 *
 * @date  2016.05.31
 *
 * Copyright (C) Beken 2009-2016
 *
 *
 ****************************************************************************************
 */
#ifndef APP_FF70_H_
#define APP_FF70_H_
/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief ff70 Application Module entry point
 *
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration


#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"         // Kernel Task Definition

/*
 * STRUCTURES DEFINITION
 ****************************************************************************************
 */

/// ff70s Application Module Environment Structure
struct app_ff70_env_tag
{
    /// Connection handle
    uint8_t conidx;
    /// Current Battery Level
    uint8_t ff71_lvl;
};
/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// ff70s Application environment
extern struct app_ff70_env_tag app_ff70_env;

/// Table of message handlers
extern const struct ke_state_handler app_ff70_table_handler;
/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * ff70s Application Functions
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize ff70s Application Module
 ****************************************************************************************
 */
void app_ff70_init(void);
/**
 ****************************************************************************************
 * @brief Add a ff70 Service instance in the DB
 ****************************************************************************************
 */
void app_ff70_add_ff70s(void);
/**
 ****************************************************************************************
 * @brief Enable the ff70 Service
 ****************************************************************************************
 */
void app_ff70_enable_prf(uint8_t conidx);
/**
 ****************************************************************************************
 * @brief Send a Battery level value
 ****************************************************************************************
 */
void app_ff70_send_lvl(uint8_t batt_lvl);


#endif // APP_BATT_H_
