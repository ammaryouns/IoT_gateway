//==============================================================================
//
//  PWMWrapper.c
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
//  Source:        LEDs.c
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
//! This module contains global and local functions for LEDs.
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//   Revision: 1.0    2016/11/29  Muhammad Ashar
//
//==============================================================================
//  INCLUDES 
//==============================================================================
#include "LEDs.h"
#include "TM4CPWM.h"
#include "driverlib/pin_map.h"
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"

//==============================================================================
//   CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

#define PWM_FREQUENCY_GEN_0     1000u   //!Todo: PWM Gnerator_0 frequecny
#define PWM_DUTYCYCLE_GEN_0     50u     //!Todo: PWM Gnerator_0 duty cycle

#define PWM_FREQUENCY_GEN_1     1000u   //!Todo: PWM Gnerator_1 frequecny
#define PWM_DUTYCYCLE_GEN_1     50u     //!Todo: PWM Gnerator_1 duty cycle

#define PWM_FREQUENCY_GEN_2     1000u   //!Todo: PWM Gnerator_2 frequecny
#define PWM_DUTYCYCLE_GEN_2     50u     //!Todo: PWM Gnerator_2 duty cycle

//==============================================================================
//    GLOBAL DATA DEFINITIONS
//==============================================================================

//==============================================================================
//    LOCAL FUNCTIONS PROTOTYPES
//==============================================================================

static void LEDInit(LED_NUMBER_ENUM ledNumber);

//==============================================================================
//  LOCAL FUNCTIONS IMPLEMENTATION
//==============================================================================

//------------------------------------------------------------------------------
//   LEDInit(LED_NUMBER_ENUM ledToBeActivated)
//
//   Date:     2016/11/29
//
//!  This function initializes the PWM generator with required frequency and
//!  duty cycle for LED(s) to be activated
//
//------------------------------------------------------------------------------

static void LEDInit(
                    LED_NUMBER_ENUM ledNumber    // LED number
                      )
{  
  switch(ledNumber)
  {
    // For LED_1 and LED_2
    case LED_NUMBER_1:
    case LED_NUMBER_2:
      // Configure PWM generator 0
      // Set the frequency value
      PWMSetFrequency(PWM_GENERATOR_0, PWM_FREQUENCY_GEN_0);
      // Set the duty cycle value      
      PWMSetDutyCycle(PWM_GENERATOR_0, PWM_DUTYCYCLE_GEN_0);
      // Activate the PWM generator.
      PWMStart(PWM_GENERATOR_0);
      break;
    
    // For LED_3 and LED_4
    case LED_NUMBER_3:
    case LED_NUMBER_4:
      // Configure PWM generator 1
      // Set the frequency value
      PWMSetFrequency(PWM_GENERATOR_1, PWM_FREQUENCY_GEN_1);
      // Set the duty cycle value      
      PWMSetDutyCycle(PWM_GENERATOR_1, PWM_DUTYCYCLE_GEN_1);
      // Activate the PWM generator.
      PWMStart(PWM_GENERATOR_1);
      break;
      
    // For LED_5 and LED_6
    case LED_NUMBER_5:
    case LED_NUMBER_6:
      // Configure PWM generator 2
      // Set the frequency value
      PWMSetFrequency(PWM_GENERATOR_2, PWM_FREQUENCY_GEN_2);
      // Set the duty cycle value      
      PWMSetDutyCycle(PWM_GENERATOR_2, PWM_DUTYCYCLE_GEN_2);
      // Activate the PWM generator.
      PWMStart(PWM_GENERATOR_2);
      break;  
      
    default:
      break;
  }
}

//==============================================================================
//  GLOBAL FUNCTIONS IMPLEMENTATIONS
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
              )
{
  // Configure the PWM settings for LED
  LEDInit(ledNumber);
  // Enable the PWM signal output to LED
  PWMEnable(ledNumber);
  
}

//------------------------------------------------------------------------------
//   LEDOff(unsigned char LEDnumber)
//
//   Date:     2016/11/30
//
//!  This function turns off the LED using GPIO pin.
//
//------------------------------------------------------------------------------

void LEDOff(
            LED_NUMBER_ENUM ledNumber    // LED number
              )
{  
  // Enable the PWM generator output
  PWMDisable(ledNumber); 
}

//==============================================================================
//  End Of File
//==============================================================================
