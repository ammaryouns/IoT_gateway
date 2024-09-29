//==============================================================================
// Start of file
//==============================================================================
//
//  TM4C12C.c
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
//  Author:        Muhammad Shuaib
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
//! This is i2C Device Driver module. 
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//
// Revision: 1.0 2016/21/11 Muhammad Shuaib
//           Initial Version
//
//==============================================================================

//==============================================================================
//  INCLUDES
//==============================================================================
//

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <drivers/TM4CI2C.h>

//
//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================

// Externs 
extern const I2C_Config I2C_config[];

// Used to check status and initialization 
static int I2C_count = -1;

// Default I2C parameters structure 
const I2C_Params I2C_defaultParams = {
    I2C_MODE_BLOCKING,  // transferMode 
    NULL,               // transferCallbackFxn 
    I2C_100kHz,         // bitRate 
    (uintptr_t) NULL    // custom 
};
//==============================================================================
// LOCAL function PROTOTYPES
//==============================================================================

//==============================================================================
// LOCAL function DEFINITIONS
//==============================================================================

//==============================================================================
// GLOBAL  function PROTOTYPES
//==============================================================================

//==============================================================================
// GLOBAL  function IMPLEMENTATIONS
//==============================================================================

//==============================================================================
//
//  void I2C_close(I2C_Handle handle)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function closes the i2c handle 
//
//============================================================================== 

void I2C_close(I2C_Handle handle)
{
    handle->fxnTablePtr->closeFxn(handle);
}

//==============================================================================
//
//  int I2C_control(I2C_Handle handle, unsigned int cmd, void *arg)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function is used for sending control commands for i2c module
//
//============================================================================== 

 
int I2C_control(I2C_Handle handle, unsigned int cmd, void *arg)
{
    return (handle->fxnTablePtr->controlFxn(handle, cmd, arg));
}

//==============================================================================
//
//  void I2C_init(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function initializes the I2C
//
//============================================================================== 

void I2C_init(void)
{
    if (I2C_count == -1) {
        // Call each driver's init function 
        for (I2C_count = 0; I2C_config[I2C_count].fxnTablePtr != NULL; I2C_count++) {
            I2C_config[I2C_count].fxnTablePtr->initFxn((I2C_Handle)&(I2C_config[I2C_count]));
        }
    }
}
//==============================================================================
//
//  I2C_Handle I2C_open(unsigned int index, I2C_Params *params)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function Opens the I2C driver
//
//============================================================================== 

I2C_Handle I2C_open(unsigned int index, I2C_Params *params)
{
    I2C_Handle handle;

    if (index >= I2C_count) {
        return (NULL);
    }

    // If params are NULL use defaults. 
    if (params == NULL) {
        params = (I2C_Params *) &I2C_defaultParams;
    }

    // Get handle for this driver instance 
    handle = (I2C_Handle)&(I2C_config[index]);

    return (handle->fxnTablePtr->openFxn(handle, params));
}

//==============================================================================
//
//  void I2C_Params_init(I2C_Params *params)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function initializes the I2C parameters
//
//============================================================================== 

void I2C_Params_init(I2C_Params *params)
{
    *params = I2C_defaultParams;
}

//==============================================================================
//
//  bool I2C_transfer(I2C_Handle handle, I2C_Transaction *transaction)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function transfers the I2C data
//
//============================================================================== 

bool I2C_transfer(I2C_Handle handle, I2C_Transaction *transaction)
{
    return (handle->fxnTablePtr->transferFxn(handle, transaction));
}
