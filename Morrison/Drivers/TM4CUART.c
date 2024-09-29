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
//
#include <stdint.h>
#include "TM4CUART.h"
#include "Board.h"

//
//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================

// Externs 
extern const UART_Config UART_config[];

// Used to check status and initialization 
static int UART_count = -1;

// Default UART parameters structure 
const UART_Params UART_defaultParams = {
    UART_MODE_BLOCKING,   // readMode 
    UART_MODE_BLOCKING,   // writeMode 
    UART_WAIT_FOREVER,    // readTimeout 
    UART_WAIT_FOREVER,    // writeTimeout 
    NULL,                 // readCallback 
    NULL,                 // writeCallback 
    UART_RETURN_NEWLINE,  // readReturnMode 
    UART_DATA_TEXT,       // readDataMode 
    UART_DATA_TEXT,       // writeDataMode 
    UART_ECHO_ON,         // readEcho 
#if defined(MSP430WARE)
    9600,                 // baudRate 
#else
    115200,               // baudRate 
#endif
    UART_LEN_8,           // dataLength 
    UART_STOP_ONE,        // stopBits 
    UART_PAR_NONE,        // parityType 
    (uintptr_t) NULL      // custom 
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
//  void UART_close(UART_Handle handle)
//
//  Author:     Muhammad Ashar
//  Date:       2016/21/11
//
//! This functions closed the UART driver
//
//============================================================================== 
void UART_close(UART_Handle handle)
{
    handle->fxnTablePtr->closeFxn(handle);
}
//==============================================================================
//
//  int UART_control(UART_Handle handle, unsigned int cmd, void *arg)
//
//  Author:     Muhammad Ashar
//  Date:       2016/21/11
//
//! This functions sets the control variable of the UART driver
//
//============================================================================== 
 
int UART_control(UART_Handle handle, unsigned int cmd, void *arg)
{
    return (handle->fxnTablePtr->controlFxn(handle, cmd, arg));
}

//==============================================================================
//
//  void UART_init(void)
//
//  Author:     Muhammad Ashar
//  Date:       2016/21/11
//
//! This functions initializes the UART driver
//
//============================================================================== 
 
void UART_init(void)
{
    if (UART_count == -1) {
        // Call each driver's init function 
        for (UART_count = 0; UART_config[UART_count].fxnTablePtr != NULL; UART_count++) {
            UART_config[UART_count].fxnTablePtr->initFxn((UART_Handle)&(UART_config[UART_count]));
        }
    }
}

//==============================================================================
//
//  UART_Handle UART_open(unsigned int index, UART_Params *params)
//
//  Author:     Muhammad Ashar
//  Date:       2016/21/11
//
//! This functions opens the UART driver
//
//============================================================================== 
 
UART_Handle UART_open(unsigned int index, UART_Params *params)
{
    UART_Handle         handle;

    if (index >= UART_count) {
        return (NULL);
    }

    // If params are NULL use defaults 
    if (params == NULL) {
        params = (UART_Params *) &UART_defaultParams;
    }

    // Get handle for this driver instance 
    handle = (UART_Handle)&(UART_config[index]);

    return (handle->fxnTablePtr->openFxn(handle, params));

}

//==============================================================================
//
//  void UART_Params_init(UART_Params *params)
//
//  Author:     Muhammad Ashar
//  Date:       2016/21/11
//
//! This functions initializes the UART parameters
//
//============================================================================== 
  
void UART_Params_init(UART_Params *params)
{
    *params = UART_defaultParams;
}

//==============================================================================
//
//  int UART_read(UART_Handle handle, void *buffer, size_t size)
//
//  Author:     Muhammad Ashar
//  Date:       2016/21/11
//
//! This functions Reads data from UART driver
//
//============================================================================== 

int UART_read(UART_Handle handle, void *buffer, size_t size)
{
    return (handle->fxnTablePtr->readFxn(handle, buffer, size));
}

//==============================================================================
//
//  int UART_readPolling(UART_Handle handle, void *buffer, size_t size)
//
//  Author:     Muhammad Ashar
//  Date:       2016/21/11
//
//! This functions pools the read data from the UART 
//
//============================================================================== 

int UART_readPolling(UART_Handle handle, void *buffer, size_t size)
{
    return (handle->fxnTablePtr->readPollingFxn(handle, buffer, size));
}

//==============================================================================
//
//  void UART_readCancel(UART_Handle handle)
//
//  Author:     Muhammad Ashar
//  Date:       2016/21/11
//
//! This functions cancel reading from the UART driver
//
//============================================================================== 

void UART_readCancel(UART_Handle handle)
{
    handle->fxnTablePtr->readCancelFxn(handle);
}

//==============================================================================
//
//  int UART_write(UART_Handle handle, const void *buffer, size_t size)
//
//  Author:     Muhammad Ashar
//  Date:       2016/21/11
//
//! This functions Writes data to UART 
//
//============================================================================== 
 
int UART_write(UART_Handle handle, const void *buffer, size_t size)
{
    return (handle->fxnTablePtr->writeFxn(handle, buffer, size));
}

//==============================================================================
//
//  int UART_writePolling(UART_Handle handle, const void *buffer, size_t size)
//
//  Author:     Muhammad Ashar
//  Date:       2016/21/11
//
//! This functions closed the UART driver
//
//============================================================================== 
 
int UART_writePolling(UART_Handle handle, const void *buffer, size_t size)
{
    return (handle->fxnTablePtr->writePollingFxn(handle, buffer, size));
}

//==============================================================================
//
//  void UART_writeCancel(UART_Handle handle)
//
//  Author:     Muhammad Ashar
//  Date:       2016/21/11
//
//! This functions cancles writing to the UART
//
//============================================================================== 

void UART_writeCancel(UART_Handle handle)
{
    handle->fxnTablePtr->writeCancelFxn(handle);
}
