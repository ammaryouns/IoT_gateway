//==============================================================================
// Start of file
//==============================================================================
//
//  Buttton.h
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
//  Source:        Button.h
//
//  Project:       Morrison
//
//  Author:        Muhammad Shuaib
//
//  Date:          2016/05/12
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This is button header module.
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//
// Revision: 1.0 2016/05/12 Muhammad Shuaib
//           Initial Version
//
//==============================================================================

//==============================================================================
//  INCLUDES
//==============================================================================
//
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
//
//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================

#define BUTTON_CODE_NO_BUTTON           0x00    //!< Null button code
#define POWER_BUTTON_CODE               0x01    //!< Power button code
#define RESET_BUTTON_CODE               0x02    //!< Reset button pressed code
#define POWER_BUTTON_LONG_CODE          0x04    //!< Power button long press code
#define RESET_BUTTON_LONG_CODE          0x08    //!< Reset button long press code
#define BUTTON_BUFFER_SIZE              20u     //!< Size of the button buffer
#define BUTTON_TASK_DELAY               40u     //!< Delay for button task
#define LONG_DETECTION_LIMIT            10u      //!< Long press button detection count limit
#define LONG_DETECTION_LIMIT_FIRST      50u     //!< First long press button detection count limit
//==============================================================================
// LOCAL function PROTOTYPES
//==============================================================================

//==============================================================================
// LOCAL function DEFINITIONS
//==============================================================================

//==============================================================================
// GLOBAL  function IMPLEMENTATIONS
//==============================================================================

//==============================================================================
//
//  ButtonInit(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/05/12
//
//! This function configures the GPIO for buttons
//
//==============================================================================

void ButtonInit(void);

//==============================================================================
//
//  ButtonIRQHandler(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/05/12
//
//! This function handles Ppwer button interrupts
//
//==============================================================================
 
void ButtonIRQHandler(unsigned int index);

//==============================================================================
//
//  DetectButtonPeekingMode(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/05/12
//
//! This function detecs pressed button in peeking mode
//
//==============================================================================

static void DetectButtonPeekingMode(void);

//==============================================================================
//
//  DetectButtonNormalMode(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/05/12
//
//! This function detecs pressed button in normal mode
//
//==============================================================================

static void DetectButtonNormalMode(void);

//------------------------------------------------------------------------------
//   ButtonProcessTimerInterrupt(void)
//
//   Author:   Muhammad Shuaib
//   Date:     2016/05/12
//
//!  This function handles the time interrupt activated due to button press
//
//------------------------------------------------------------------------------

void ButtonProcessTimerInterrupt(void);

//------------------------------------------------------------------------------
//   ButtonDecode(unsigned int *status)
//
//   Author:   Muhammad Shuaib
//
//   Date:    2016/06/12
//
//!  This function returns the status of pressed buttons by reading respective
//!  GPIO interrupts
//
//------------------------------------------------------------------------------

void ButtonPressed(
                   bool *isButtonPressed // Pressed button status
                       );

//------------------------------------------------------------------------------
//   ButtonDecode(unsigned int *code)
//
//   Author:   Muhammad Shuaib
//
//   Date:    2016/06/12
//
//!  This function returns the code of pressed buttons by reading respective
//!  GPIO interrupts
//
//------------------------------------------------------------------------------

void ButtonDecode(
                  unsigned int *code // Pressed buttons code
                      );
//------------------------------------------------------------------------------
//   bool ButtonGetCanStartInitTask(void)
//
//   Author:   Muhammad Shuaib
//   Date:     2016/15/12
//
//!  This function gets the variable that is used to start initialization task
//
//------------------------------------------------------------------------------

bool ButtonGetCanStartInitTask(void);


//==============================================================================
//  End Of File
//==============================================================================