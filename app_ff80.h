/**
 ****************************************************************************************
 *
 * @file app_ff80.c
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
#ifndef APP_FF80_H_
#define APP_FF80_H_
/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief ff80 Application Module entry point
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

/// ff80s Application Module Environment Structure
struct app_ff80_env_tag
{
    /// Connection handle
    uint8_t conidx;
    /// Current Battery Level
    uint8_t ff81_lvl;
};
/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// ff80s Application environment
extern struct app_ff80_env_tag app_ff80_env;

/// Table of message handlers
extern const struct ke_state_handler app_ff80_table_handler;
/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * ff80s Application Functions
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize ff80s Application Module
 ****************************************************************************************
 */
void app_ff80_init(void);
/**
 ****************************************************************************************
 * @brief Add a ff80 Service instance in the DB
 ****************************************************************************************
 */
void app_ff80_add_ff80s(void);
/**
 ****************************************************************************************
 * @brief Enable the ff80 Service
 ****************************************************************************************
 */
void app_ff80_enable_prf(uint8_t conidx);
/**
 ****************************************************************************************
 * @brief Send a Battery level value
 ****************************************************************************************
 */
void app_ff80_send_lvl(uint8_t batt_lvl);


#endif // APP_BATT_H_
