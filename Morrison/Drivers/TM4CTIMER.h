
//==============================================================================
//
//  TIMER_TM4C1294.h
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
//  Source:        TIMER_TM4C1294.h
//
//  Project:       Morrison
//
//  Author:        Saleem
//
//  Date:          2016/11/24
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! The header file contains the functions required for reading and writing the 
//! the internal flash
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//  Revision: 1.0
//  Author: Muhammad Saleem
//  Date: 12/08/2016
//     Initial Revision
//
//  Revision: 1.1
//  Author: Muhammad Shuaib
//  Date: 12/08/2016
//
//==============================================================================

#ifndef __TM4CTIMER_H__
#define __TM4CTIMER_H__

//==============================================================================
//  INCLUDES 
//==============================================================================

#include <stdbool.h>
#include <stdint.h>
#include <driverlib\std.h>

//==============================================================================
//  CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

#define SYSTEM_CLOCK_FREQUENCY  120000000u      // System clock frequency in MHz

//==============================================================================
//  GLOBAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
//  GLOBAL DATA DEFINITIONS
//==============================================================================

//==============================================================================
//  GLOBAL FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
//   RTOSTimerInit(void)
//
//  Author:   Muhammad Saleem
//  Date:     11/24/2016
//
//  Revision: 1.1
//  Author: Muhammad Shuaib
//  12/08/2016
//
//! This function initilzizes RTOS Timer0
//
//==============================================================================

void RTOSTimerInit(void);

//==============================================================================
//
//  void ButtonTimerInit(void)
//
//  Author:     Muhammad Saleem
//  Date:       2016/21/11
//
//  Revision: 1.1
//  Author: Muhammad Shuaib
//  Date: 12/08/2016
//
//! This function configures and initializes Timer0 module for button interrupts
//
//============================================================================== 

void ButtonTimerInit(void);

//==============================================================================
//
//  void ButtonTimerLoadCounterValue(unsigned int timerDelayValue)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/09/12
//
//! This function loads new counter value to button timer and enables it
//
//============================================================================== 

void ButtonTimerLoadCounterValue(
                                 unsigned int timerDelayValue // Delay value in ms
                                     );

//==============================================================================
//
//  void ButtonTimerDisable(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/09/12
//
//! This function clears button timer interrupts and disables the timer
//
//============================================================================== 

void ButtonTimerDisable(void);

//==============================================================================
//
//  void Timer1Init(unsigned short timerPeriod)
//
//  Author:     Muhammad Saleem
//  Date:       2016/21/11
//
//  Revision: 1.1
//  Author: Muhammad Shuaib
//  Date: 12/08/2016
//
//! This function configures and initializes Timer1 module
//
//============================================================================== 

void Timer1Init(
                unsigned short timerPeriod // Timer period in msec
                    );

//==============================================================================
//   RTOSTimerIRQHandler(UArg arg)
//
//   Author:   Saleem
//   Date:     2016/11/24
//
//   Revision: 1.1
//   Author: Muhammad Shuaib
//  Date: 12/08/2016
//
//! This is RTOS Timer IRQ handler
//
//==============================================================================

void RTOSTimerIRQHandler(UArg arg);

//==============================================================================
//
//  void ButtonTimerIRQHandler(void)
//
//  Author:     Muhammad Saleem
//  Date:       2016/21/11
//
//  Revision: 1.1
//  Author: Muhammad Shuaib
//  
//! This function handles Timer0 timeout interrupts
//
//============================================================================== 

void ButtonTimerIRQHandler(void);

//==============================================================================
//
//  void Timer1IRQHandler(void)
//
//  Author:     Muhammad Saleem
//  Date:       2016/21/11
//
//  Revision: 1.1
//  Author: Muhammad Shuaib
//  Date: 12/08/2016
//
//! This function handles Timer1 timeout interrupts
//
//============================================================================== 

void Timer1IRQHandler(void);

#endif /* __TIMERTM4C_H__ */

//==============================================================================
// END OF FILE
//==============================================================================