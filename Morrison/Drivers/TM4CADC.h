//==============================================================================
// Start of file
//==============================================================================
//
//  TM4CADC.h
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
//  Source:        TM4CADC.h
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
//! This is ADC Device Driver module. It contains global functions declarations
//! that are used for analogue to digital conversion
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
//
//==============================================================================
// CONSTANTS, DEFINES AND MACROS
//==============================================================================

//==============================================================================
// GLOBAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
// GLOBAL  FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
//
//  ADCInit(void);
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This functions configures the ADC0 module
//
//==============================================================================

void ADCInit(void);

//==============================================================================
//
//  ADCReadChannel(
//                 uint32_t *adcReadBuffer
//                  )
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This functions uses the ADC0 module, reads AIN0 and AIN1 and saves the read
//! values of two channels in adcReadBuffer
//
//==============================================================================

void ADCReadChannel(
                    uint32_t *adcReadBuffer // Pointer to read buffer
                      );

//==============================================================================
//  End Of File
//==============================================================================