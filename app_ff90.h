/**
 ****************************************************************************************
 *
 * @file app_ff90.c
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
#ifndef APP_FF90_H_
#define APP_FF90_H_
/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief ff90 Application Module entry point
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

/// ff90s Application Module Environment Structure
struct app_ff90_env_tag
{
    /// Connection handle
    uint8_t conidx;
    /// Current Battery Level
    uint8_t ff91_lvl;
};
/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// ff90s Application environment
extern struct app_ff90_env_tag app_ff90_env;

/// Table of message handlers
extern const struct ke_state_handler app_ff90_table_handler;
/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * ff90s Application Functions
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize ff90s Application Module
 ****************************************************************************************
 */
void app_ff90_init(void);
/**
 ****************************************************************************************
 * @brief Add a ff90 Service instance in the DB
 ****************************************************************************************
 */
void app_ff90_add_ff90s(void);
/**
 ****************************************************************************************
 * @brief Enable the ff90 Service
 ****************************************************************************************
 */
void app_ff90_enable_prf(uint8_t conidx);
/**
 ****************************************************************************************
 * @brief Send a Battery level value
 ****************************************************************************************
 */
void app_ff90_send_lvl(uint8_t batt_lvl);


#endif // APP_BATT_H_
