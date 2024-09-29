//==============================================================================
// Start of file
//==============================================================================
//
//  TM4CSPI.c
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
//  Source:        TM4CSPI.c
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
//! This is I2C Device Driver module. 
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
#include <TM4CSPI.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_gpio.h>
#include <driverlib/flash.h>
#include <driverlib/gpio.h>
#include <driverlib/i2c.h>
#include <driverlib/pin_map.h>
#include <driverlib/pwm.h>
#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <driverlib/udma.h>
#include "EK_TM4C1294XL.h"
//
//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================

/* Externs */
extern const SPI_Config SPI_config[];

/* Used to check status and initialization */
static int SPI_count = -1;

/* Default SPI parameters structure */
const SPI_Params SPI_defaultParams = {
    SPI_MODE_BLOCKING,  /* transferMode */
    SPI_WAIT_FOREVER,   /* transferTimeout */
    NULL,               /* transferCallbackFxn */
    SPI_MASTER,         /* mode */
    1000000,            /* bitRate */
    8,                  /* dataSize */
    SPI_POL0_PHA0,      /* frameFormat */
    (uintptr_t) NULL    /* custom */
};
//==============================================================================
// LOCAL FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
// LOCAL FUNCTIONS DEFINITIONS
//==============================================================================

//==============================================================================
// GLOBAL  FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
// GLOBAL  FUNCTIONS IMPLEMENTATIONS
//==============================================================================

//==============================================================================
//
//  void SPI_close(SPI_Handle handle)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function closes the SPI driver
//
//==============================================================================
void SPI_close(SPI_Handle handle)
{
    handle->fxnTablePtr->closeFxn(handle);
}

//==============================================================================
//
//  int SPI_control(SPI_Handle handle, unsigned int cmd, void *arg)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function sends control data to SPI module
//
//==============================================================================

int SPI_control(SPI_Handle handle, unsigned int cmd, void *arg)
{
    return (handle->fxnTablePtr->controlFxn(handle, cmd, arg));
}

//==============================================================================
//
//  void SPI_init(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function initializes the SPI driver
//
//==============================================================================

void SPI_init(void)
{
    if (SPI_count == -1) {
        /* Call each driver's init function */
        for (SPI_count = 0; SPI_config[SPI_count].fxnTablePtr != NULL; SPI_count++) {
            SPI_config[SPI_count].fxnTablePtr->initFxn((SPI_Handle)&(SPI_config[SPI_count]));
        }
    }
}

//==============================================================================
//
//  SPI_Handle SPI_open(unsigned int index, SPI_Params *params)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function Opens the SPI driver
//
//==============================================================================

SPI_Handle SPI_open(unsigned int index, SPI_Params *params)
{
    SPI_Handle handle;

    if (index >= SPI_count) {
        return (NULL);
    }

    /* If params are NULL use defaults */
    if (params == NULL) {
        params = (SPI_Params *) &SPI_defaultParams;
    }

    /* Get handle for this driver instance */
    handle = (SPI_Handle)&(SPI_config[index]);

    return (handle->fxnTablePtr->openFxn(handle, params));
}

//==============================================================================
//
//  void SPI_Params_init(SPI_Params *params)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function initializes the SPI Parameters
//
//==============================================================================

void SPI_Params_init(SPI_Params *params)
{
    *params = SPI_defaultParams;
}

//==============================================================================
//
//  void SPI_serviceISR(SPI_Handle handle)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function service SPI ISR
//
//==============================================================================

void SPI_serviceISR(SPI_Handle handle)
{
    handle->fxnTablePtr->serviceISRFxn(handle);
}

/*
 *  ======== SPI_transfer ========
 */
bool SPI_transfer(SPI_Handle handle, SPI_Transaction *transaction)
{
    return (handle->fxnTablePtr->transferFxn(handle, transaction));
}

//==============================================================================
//
//  void SPI_transferCancel(SPI_Handle handle)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function performs SPI Transfer
//
//==============================================================================

void SPI_transferCancel(SPI_Handle handle)
{
    handle->fxnTablePtr->transferCancelFxn(handle);
}

