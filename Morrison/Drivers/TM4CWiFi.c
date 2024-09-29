//==============================================================================
// Start of file
//==============================================================================
//
//  TM4CUART.c
//
//  Copyright (C) 2016 by Industrial Scientific.
//
//  This document and all information contained within are confidential and
//  proprietary property of Industrial Scientific Corporation. All rights
//  reserved. It is not to be reproduced or reused without the prior approval
//  of Industrial Scientific Corporation.
//
//==============================================================================
//  FILE INFORMATION
//==============================================================================
//
//  Source:        TM4CUART.c
//
//  Project:       Morrison
//
//  Author:        Muhammad Ashar
//
//  Date:          2016/21/11
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This is UART Device Driver module. It contains global functions that are used
//! for UART communication.
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//
// Revision: 1.0 2016/21/11 Muhammad Ashar
//           Initial Version
//
//==============================================================================

//==============================================================================
//  INCLUDES
//==============================================================================
#include <TM4CWiFi.h>
#include <stdint.h>
#include <stdlib.h>

//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================

// Externs
extern const WiFi_Config WiFi_config[];
// Number of WiFi modules. -1 if init has not been called.
static int WiFi_count = -1;
// Default WiFi parameters structure
const WiFi_Params WiFi_defaultParams = {
    1000000,         /* bitRate */
    3,               /* SimpleLink spawn task priority */
    (uintptr_t) NULL /* custom */
};
//==============================================================================
// LOCAL FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
// LOCAL FUNCTIONS DEFINITIONS
//==============================================================================

//==============================================================================
// GLOBAL  FUNCTIONS IMPLEMENTATIONS
//==============================================================================

//==============================================================================
// GLOBAL  FUNCTIONS IMPLEMENTATIONS
//==============================================================================

//==============================================================================
//
//  void WiFi_close(WiFi_Handle handle)
//
//  Author:     Fehan Arif Malik
//  Date:       2016/21/11
//
//! This function closes the wifi driver 
//
//============================================================================== 

void WiFi_close(WiFi_Handle handle)
{
    handle->fxnTablePtr->closeFxn(handle);
}

//==============================================================================
//
//  int WiFi_control(WiFi_Handle handle, unsigned int cmd, void *arg)
//
//  Author:     Fehan Arif Malik
//  Date:       2016/21/11
//
//! This function sends contorl information to wifi driver 
//
//============================================================================== 

int WiFi_control(WiFi_Handle handle, unsigned int cmd, void *arg)
{
    return (handle->fxnTablePtr->controlFxn(handle, cmd, arg));
}

//==============================================================================
//
//  void WiFi_init()
//
//  Author:     Fehan Arif Malik
//  Date:       2016/21/11
//
//! This function initializes the wifi
//
//============================================================================== 

void WiFi_init()
{
    /* Return if this function has already been called */
    if (WiFi_count == -1) {
        /* Call each driver's init function */
        for (WiFi_count = 0; WiFi_config[WiFi_count].fxnTablePtr != NULL;
             WiFi_count++) {
            WiFi_config[WiFi_count].fxnTablePtr->initFxn(
                (WiFi_Handle)&(WiFi_config[WiFi_count]));
        }
    }
}

//==============================================================================
//
//  WiFi_Handle WiFi_open(unsigned int wifiIndex, unsigned int spiIndex,
//                      WiFi_evntCallback evntCallback, WiFi_Params *params)
//
//  Author:     Fehan Arif Malik
//  Date:       2016/21/11
//
//! This function opens the wifi driver 
//
//============================================================================== 

WiFi_Handle WiFi_open(unsigned int wifiIndex, unsigned int spiIndex,
                      WiFi_evntCallback evntCallback, WiFi_Params *params)
{
    WiFi_Handle handle;

    if (wifiIndex >= WiFi_count) {
        return (NULL);
    }

    /* If params are NULL use defaults. */
    if (params == NULL) {
        params = (WiFi_Params *) &WiFi_defaultParams;
    }

    /* Get the handle for this driver instance */
    handle = (WiFi_Handle)&(WiFi_config[wifiIndex]);

    return (handle->fxnTablePtr->openFxn(handle, spiIndex, evntCallback,
                                         params));
}
//==============================================================================
//
//  void WiFi_Params_init(WiFi_Params *params)
//
//  Author:     Fehan Arif Malik
//  Date:       2016/21/11
//
//! This function initializes the wifi parameters
//
//============================================================================== 

void WiFi_Params_init(WiFi_Params *params)
{
    *params = WiFi_defaultParams;
}
