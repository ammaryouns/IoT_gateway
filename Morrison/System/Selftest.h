//==============================================================================
// Start of file
//==============================================================================
//
//  SelfTest.h
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
//  Source:        SelfTest.h
//
//  Project:       Morrison
//
//  Author:        Muhammad Shuaib
//
//  Date:          2016/30/11
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This module contains functions prototypes that handle the instrument
//! self-test  during power-on
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//
// Revision: 1.0 2016/30/11 Muhammad Shuaib
//           Initial Version
//
//==============================================================================

//==============================================================================
//  INCLUDES
//==============================================================================
//
#include <stdint.h>
#include <stdbool.h>
#include <TM4CPWM.h>
//
//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================

typedef enum
{
  SELFTEST_STEP_DATAFLASH = 0,
  SELFTEST_STEP_BATTERY,
  SELFTEST_STEP_FLASHCRC,
  SELFTEST_STEP_INSTRUMENT_PARAM,
  SELFTEST_STEP_LENS,
  SELFTEST_STEP_CORE_CONN_PERIPH,  
  SELFTEST_STEP_CORE_CONN,
  SELFTEST_STEP_NFC_HARD,
  SELFTEST_STEP_BTLE_HARD,
  SELFTEST_STEP_GPS_HARD,
  SELFTEST_STEP_TIME_UPDATE,
  
  SELFTEST_STEP_LIM,
} SELFTEST_STEP_ENUM;

//==============================================================================
// LOCAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
// LOCAL DATA DEFINITIONS
//==============================================================================

//==============================================================================
// GLOBAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
// GLOBAL DATA DEFINITIONS
//==============================================================================

//==============================================================================
// LOCAL FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
// LOCAL FUNCTIONS DEFINITIONS
//==============================================================================

//==============================================================================
// GLOBAL  FUNCTIONS IMPLEMENTATAIONS
//==============================================================================

//==============================================================================
//
//  SelfTestHandler(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function 
//
//==============================================================================

bool SelfTestHandler(SELFTEST_STEP_ENUM selfTestStep);

//==============================================================================
//
//  DiagnosticsLEDs(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function handles LEDs diagnostics
//
//=============================================================================

void DiagnosticsLEDs(
                            LED_NUMBER_ENUM ledNumber,  // LED No. to be tested
                            bool isIndicatorActivated   // LED activation command
                                );

//==============================================================================
//
//  DiagnosticsLEDsAllOff(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/17/12
//
//! This function deactivates all LEDs
//
//=============================================================================

void DiagnosticsLEDsAllOff(void);

//------------------------------------------------------------------------------
//   CalculateFlashCrc (unsigned char *src, unsigned int len )
//
//   Date:     2016/07/12
//
//!  This function calculates the CRC of the ROM Flash and compares it against
//!  the saved value.
//
//------------------------------------------------------------------------------

unsigned int CalculateFlashCrc(
                             unsigned char *src,      //!< starting address
                             unsigned int len         //!< Lenght of CRC data 
                             );

//==============================================================================
//  End Of File
//==============================================================================