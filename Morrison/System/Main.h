//==============================================================================
//
//  Main.h
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
//  Source:        Main.h
//
//  Project:       Morrison
//
//  Author:        Muhammad Shuaib
//
//  Date:          2016/06/12
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This is main module header file for Morrison firmware. 
//!
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//  Revision: 1.0  2016/06/12 Muhammad Shuaib
//      Initial version of Morrison
//
//==============================================================================
//  INCLUDES
//==============================================================================
//
#include <stdint.h>
#include <stdbool.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Timer.h>
//
//==============================================================================
//  CONSTANTS, TYPEDEFS AND MACROS
//==============================================================================

//==============================================================================
//  GLOBAL DATA DECLARATIONS
//==============================================================================

extern Semaphore_Struct buttonSemaphoreStruct;
extern Semaphore_Handle buttonSemaphoreHandle;
extern Timer_Handle buttonTimerHandle;

//==============================================================================
//  GLOBAL FUNCTIONS PROTOTYPES
//==============================================================================

//------------------------------------------------------------------------------
//   GetIsDeviceInPeeking(void)
//
//   Author:  Muhammad Shuaib 
//   Date:    2016/06/12
//
//! This function is used by other modules to get the status of isDeviceInPeeking
//
//------------------------------------------------------------------------------

bool GetIsDeviceInPeeking(void);

//------------------------------------------------------------------------------
//   SetIsDeviceInPeeking(void)
//
//   Author:  Muhammad Shuaib 
//   Date:    2016/06/12
//
//! This function is used by other modules to set the status of isDeviceInPeeking
//
//------------------------------------------------------------------------------

void SetIsDeviceInPeeking(bool setIsDeviceInPeeking);

//==============================================================================
//  End Of File
//==============================================================================