/**
 ****************************************************************************************
 *
 * @file app_ffa0.c
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
#ifndef APP_FFA0_H_
#define APP_FFA0_H_
/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief ffa0 Application Module entry point
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

/// ffa0s Application Module Environment Structure
struct app_ffa0_env_tag
{
    /// Connection handle
    uint8_t conidx;
    /// Current Battery Level
    uint8_t ffa1_lvl;
};
/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// ffa0s Application environment
extern struct app_ffa0_env_tag app_ffa0_env;

/// Table of message handlers
extern const struct ke_state_handler app_ffa0_table_handler;
/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * ffa0s Application Functions
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize ffa0s Application Module
 ****************************************************************************************
 */
void app_ffa0_init(void);
/**
 ****************************************************************************************
 * @brief Add a ffa0 Service instance in the DB
 ****************************************************************************************
 */
void app_ffa0_add_ffa0s(void);
/**
 ****************************************************************************************
 * @brief Enable the ffa0 Service
 ****************************************************************************************
 */
void app_ffa0_enable_prf(uint8_t conidx);
/**
 ****************************************************************************************
 * @brief Send a Battery level value
 ****************************************************************************************
 */
void app_ffa0_send_lvl(uint8_t batt_lvl);


#endif // APP_BATT_H_
