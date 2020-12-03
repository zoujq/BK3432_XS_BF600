/**
 ****************************************************************************************
 *
 * @file app_ff60.c
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
#ifndef APP_FF60_H_
#define APP_FF60_H_
/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief ff60 Application Module entry point
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

/// ff60s Application Module Environment Structure
struct app_ff60_env_tag
{
    /// Connection handle
    uint8_t conidx;
    /// Current Battery Level
    uint8_t ff61_lvl;
};
/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// ff60s Application environment
extern struct app_ff60_env_tag app_ff60_env;

/// Table of message handlers
extern const struct ke_state_handler app_ff60_table_handler;
/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * ff60s Application Functions
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize ff60s Application Module
 ****************************************************************************************
 */
void app_ff60_init(void);
/**
 ****************************************************************************************
 * @brief Add a ff60 Service instance in the DB
 ****************************************************************************************
 */
void app_ff60_add_ff60s(void);
/**
 ****************************************************************************************
 * @brief Enable the ff60 Service
 ****************************************************************************************
 */
void app_ff60_enable_prf(uint8_t conidx);
/**
 ****************************************************************************************
 * @brief Send a Battery level value
 ****************************************************************************************
 */
void app_ff60_send_lvl(uint8_t batt_lvl);


#endif // APP_BATT_H_
