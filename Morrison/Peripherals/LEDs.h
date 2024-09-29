//==============================================================================
//
//  LEDs.h
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
//  Source:        LEDs.h
//
//  Project:       Morrison
//
//  Author:        Muhammad Ashar
//
//  Date:          2016/11/29
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This file contains declaration of functions which are used to turn the LEDs 
//! on and off and global defines for this module. LEDs are turned on in 
//! conditions such as power on power off, under and over range gas errors etc.
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//
//  
//     
//
//==============================================================================

#ifndef __LEDS_H__
#define __LEDS_H__

//==============================================================================
//  INCLUDES
//==============================================================================
//
#include <stdbool.h>
#include "TM4CPWM.h"
//
//==============================================================================
//  CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

//==============================================================================
//  GLOBAL DATA STRUCTURES DEFINITION
//==============================================================================

//==============================================================================
//  GLOBAL DATA
//==============================================================================

//==============================================================================
//  EXTERNAL OR GLOBAL FUNCTIONS
//==============================================================================

//------------------------------------------------------------------------------
//   LEDOn(LED_NUMBER_ENUM ledNumber)
//
//   Date:     2016/11/29
//
//!  This function turns on the LEDs using PWM generators.
//
//------------------------------------------------------------------------------

void LEDOn(
              LED_NUMBER_ENUM ledNumber        // LED number
                );

//------------------------------------------------------------------------------
//   LEDOff(LED_NUMBER_ENUM ledNumber)
//
//   Date:     2016/11/29
//
//!  This function turns off the LED using PWM.
//
//------------------------------------------------------------------------------

void LEDOff(
            LED_NUMBER_ENUM ledNumber   // LED number
              );
#endif /* __LEDS_H__ */
//==============================================================================
//  End Of File
//==============================================================================
