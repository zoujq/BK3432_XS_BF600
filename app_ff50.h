/**
 ****************************************************************************************
 *
 * @file app_ff50.c
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
#ifndef APP_FF50_H_
#define APP_FF50_H_
/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief ff50 Application Module entry point
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

/// ff50s Application Module Environment Structure
struct app_ff50_env_tag
{
    /// Connection handle
    uint8_t conidx;
    /// Current Battery Level
    uint8_t ff51_lvl;
};
/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// ff50s Application environment
extern struct app_ff50_env_tag app_ff50_env;

/// Table of message handlers
extern const struct ke_state_handler app_ff50_table_handler;
/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * ff50s Application Functions
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize ff50s Application Module
 ****************************************************************************************
 */
void app_ff50_init(void);
/**
 ****************************************************************************************
 * @brief Add a ff50 Service instance in the DB
 ****************************************************************************************
 */
void app_ff50_add_ff50s(void);
/**
 ****************************************************************************************
 * @brief Enable the ff50 Service
 ****************************************************************************************
 */
void app_ff50_enable_prf(uint8_t conidx);
/**
 ****************************************************************************************
 * @brief Send a Battery level value
 ****************************************************************************************
 */
void app_ff50_send_lvl(uint8_t batt_lvl);


#endif // APP_BATT_H_
