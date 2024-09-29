//==============================================================================
//
//  TM4CPWM.c
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
//  Source:        TM4CPWM.c
//
//  Project:       Morrison
//
//  Author:        Ali Zulqarnain Anjum
//
//  Date:          2016/11/08
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This module contains global and local functions for PWM.
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//   Revision: 1.0    2016/11/08  Ali Zulqarnain Anjum
//
//==============================================================================
//  INCLUDES 
//==============================================================================

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


//==============================================================================
//    LOCAL DATA STRUCTURE DEFINITION
//==============================================================================

//==============================================================================
//    GLOBAL DATA DECLARATIONS
//==============================================================================

static PWM_GENERATOR_SETTINGS pwmGenOne,pwmGenTwo,pwmGenThree;                  //!< PWM Generator Number

//==============================================================================
//    LOCAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
//    LOCAL FUNCTION PROTOTYPES
//==============================================================================


//==============================================================================
//    LOCAL FUNCTIONS IMPLEMENTATION
//==============================================================================


//==============================================================================
//    GLOBAL FUNCTIONS IMPLEMENTATION
//==============================================================================

//------------------------------------------------------------------------------
//   PWMSetFrequency( LED_NUMBER_ENUM ledNumber, unsigned short freqKhz)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function is used to set the frequency of PWM
//  
//------------------------------------------------------------------------------

void PWMSetFrequency( unsigned char genNubmer, unsigned short freqKhz)
{
    //Set the frequency of PWM
    if ( genNubmer == PWM_GENERATOR_0 )
    {
        pwmGenOne.freqKHz = freqKhz;
    }
    else if ( genNubmer == PWM_GENERATOR_1 )
    {
        pwmGenTwo.freqKHz = freqKhz;
    }
    else if ( genNubmer == PWM_GENERATOR_2 )
    {
        pwmGenThree.freqKHz = freqKhz;
    }
    else
    {
        //Do nothing
    }
}

//------------------------------------------------------------------------------
//   unsigned short PWMGetFrequency( unsigned char genNubmer )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function returns the current Frequency of PWM
//  
//------------------------------------------------------------------------------

unsigned short PWMGetFrequency( unsigned char genNubmer )
{
    // current frequency
    unsigned short currentFreq = 0u;
    //Get the frequency of PWM
    if ( genNubmer == PWM_GENERATOR_0 )
    {
        currentFreq = pwmGenOne.freqKHz;
    }
    else if ( genNubmer == PWM_GENERATOR_1 )
    {
        currentFreq = pwmGenTwo.freqKHz;
    }
    else if ( genNubmer == PWM_GENERATOR_2 )
    {
        currentFreq = pwmGenThree.freqKHz;
    }
    else
    {
        //Do nothing
    }
    return currentFreq;
}

//------------------------------------------------------------------------------
//   PWMSetDutyCycle( unsigned char genNubmer, unsigned char dutyCycle)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function is used to set the duty cycle of PWM
//  
//------------------------------------------------------------------------------

void PWMSetDutyCycle( unsigned char genNubmer, unsigned char dutyCycle)
{
    //Set the current duty cycle of PWM
    if ( genNubmer == PWM_GENERATOR_0 )
    {
        pwmGenOne.dutyCycle = dutyCycle;
    }
    else if (  genNubmer == PWM_GENERATOR_1)
    {
        pwmGenTwo.dutyCycle = dutyCycle;
    }
    else if (  genNubmer == PWM_GENERATOR_2 )
    {
        pwmGenThree.dutyCycle = dutyCycle;
    }
    else
    {
        //Do nothing
    }
}

//------------------------------------------------------------------------------
//   unsigned char PWMGetDutyCycle( unsigned char genNubmer )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function returns the current duty cycle of PWM
//  
//------------------------------------------------------------------------------

unsigned char PWMGetDutyCycle( unsigned char genNubmer )
{
    // current duty cycle
    unsigned char currentDutyCycle = 0u;
    //Get the duty cycle of PWM
    if ( genNubmer == PWM_GENERATOR_0 )
    {
        currentDutyCycle = pwmGenOne.dutyCycle;
    }
    else if ( genNubmer == PWM_GENERATOR_1 )
    {
        currentDutyCycle = pwmGenTwo.dutyCycle;
    }
    else if ( genNubmer == PWM_GENERATOR_2 )
    {
        currentDutyCycle = pwmGenThree.dutyCycle;
    }
    else
    {
        //Do nothing
    }
    return currentDutyCycle;
}

//------------------------------------------------------------------------------
//   PWMStart( unsigned char genNubmer )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function starts the PWM
//  
//------------------------------------------------------------------------------

void PWMStart( unsigned char genNubmer )
{
    // Number of cycles per second
    unsigned int numberOfCycles = 0u;
    // Duty Cycle
    unsigned int onTime = 0u;
    if ( genNubmer == PWM_GENERATOR_0 )
    {
       // Configure the PWM0 to count down without synchronization.
       PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN |
                    PWM_GEN_MODE_NO_SYNC);
      // use the following equation: N = (1 / f) * SysClk.  Where N is the
      // TODO: modify this calculation to use the clock frequency that you are
      // using. system clock in this code is 12MHZ
      numberOfCycles = 12000000/(pwmGenOne.freqKHz*1000);
      PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, numberOfCycles);
      // Set PWM0 to a duty cycle of 25%.  You set the duty cycle as a function
      // of the period.  Since the period was set above, you can use the
      // PWMGenPeriodGet() function.  For this example the PWM will be high for
      // 25% of the time or 16000 clock ticks (64000 / 4).
      //
      onTime = (numberOfCycles * pwmGenOne.dutyCycle)/100;
      PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, onTime);
      PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, onTime);  
      // Enable the PWM generator block.
      PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    }
    else if ( genNubmer == PWM_GENERATOR_1 )
    {
       // Configure the PWM0 to count down without synchronization.
       PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN |
                    PWM_GEN_MODE_NO_SYNC);
      // TODO: modify this calculation to use the clock frequency that you are
      // using. system clock in this code is 12MHZ
       numberOfCycles = 12000000/(pwmGenTwo.freqKHz*1000);
       PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, numberOfCycles);
       onTime = (numberOfCycles * pwmGenTwo.dutyCycle)/100;
       PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, onTime);
       PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, onTime);
       // Enable the PWM generator block.
       PWMGenEnable(PWM0_BASE, PWM_GEN_1);
    }
    else if ( genNubmer == PWM_GENERATOR_2 )
    {
       // Configure the PWM0 to count down without synchronization.
       PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN |
                    PWM_GEN_MODE_NO_SYNC);
      // use the following equation: N = (1 / f) * SysClk.  Where N is the
      // TODO: modify this calculation to use the clock frequency that you are
      // using. system clock in this code is 12MHZ
      numberOfCycles = 12000000/(pwmGenThree.freqKHz*1000);
      PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, numberOfCycles);
      onTime = (numberOfCycles * pwmGenThree.dutyCycle)/100;
      PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, onTime);
      PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, onTime);
      // Enable the PWM generator block.
      PWMGenEnable(PWM0_BASE, PWM_GEN_2);
    }
    else
    {
        //Do nothing
    }
      
}

//------------------------------------------------------------------------------
//   PWMDisable( LED_NUMBER_ENUM ledNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function stops the PWM
//  
//------------------------------------------------------------------------------

void PWMDisable( LED_NUMBER_ENUM ledNumber )
{
   if(ledNumber == LED_NUMBER_1)
   {
     // Enable the PWM0 Bit0 (PD0) output signal.
     PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, false);
     // Set the current state of PWM as enable
     // TODO: make this boolean
     pwmGenOne.outputEnable[0] = 0u;
   }
   else if (ledNumber == LED_NUMBER_2)
   {
     // Enable the PWM0 Bit0 (PD0) output signal.
     PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, false);
     // Set the current state of PWM as enable
     // TODO: make this boolean
     pwmGenOne.outputEnable[1] = 0u;
   }
   else if (ledNumber == LED_NUMBER_3)
   {
     // Enable the PWM0 Bit0 (PD0) output signal.
     PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, false);
     // Set the current state of PWM as enable
     // TODO: make this boolean
     pwmGenTwo.outputEnable[0] = 0u;
   }
   else if (ledNumber == LED_NUMBER_4)
   {
     // Enable the PWM0 Bit0 (PD0) output signal.
     PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);
     // Set the current state of PWM as enable
     // TODO: make this boolean
     pwmGenTwo.outputEnable[1] = 0u;
   }
   else if (ledNumber == LED_NUMBER_5)
   {
     // Enable the PWM0 Bit0 (PD0) output signal.
     PWMOutputState(PWM0_BASE, PWM_OUT_4_BIT, false);
     // Set the current state of PWM as enable
     // TODO: make this boolean
     pwmGenThree.outputEnable[0] = 0u;
   }
   else if (ledNumber == LED_NUMBER_6)
   {
      // Enable the PWM0 Bit0 (PD0) output signal.
     PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, false);
     // Set the current state of PWM as enable
     // TODO: make this boolean
     pwmGenThree.outputEnable[1] = 0u;
   }
   else
   {
      //Do nothing
   }
}
//------------------------------------------------------------------------------
//   PWMEnable( LED_NUMBER_ENUM ledNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function stops the PWM
//  
//------------------------------------------------------------------------------

void PWMEnable( LED_NUMBER_ENUM ledNumber )
{
   if(ledNumber == LED_NUMBER_1)
   {
     // Enable the PWM0 Bit0 (PD0) output signal.
     PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
     // Set the current state of PWM as enable
     // TODO: make this boolean
     pwmGenOne.outputEnable[0] = 1u;
   }
   else if (ledNumber == LED_NUMBER_2)
   {
     // Enable the PWM0 Bit0 (PD0) output signal.
     PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
     // Set the current state of PWM as enable
     // TODO: make this boolean
     pwmGenOne.outputEnable[1] = 1u;
   }
   else if (ledNumber == LED_NUMBER_3)
   {
     // Enable the PWM0 Bit0 (PD0) output signal.
     PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true);
     // Set the current state of PWM as enable
     // TODO: make this boolean
     pwmGenTwo.outputEnable[0] = 1u;
   }
   else if (ledNumber == LED_NUMBER_4)
   {
     // Enable the PWM0 Bit0 (PD0) output signal.
     PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, true);
     // Set the current state of PWM as enable
     // TODO: make this boolean
     pwmGenTwo.outputEnable[1] = 1u;
   }
   else if (ledNumber == LED_NUMBER_5)
   {
     // Enable the PWM0 Bit0 (PD0) output signal.
     PWMOutputState(PWM0_BASE, PWM_OUT_4_BIT, true);
     // Set the current state of PWM as enable
     // TODO: make this boolean
     pwmGenThree.outputEnable[0] = 1u;
   }
   else if (ledNumber == LED_NUMBER_6)
   {
      // Enable the PWM0 Bit0 (PD0) output signal.
     PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, true);
     // Set the current state of PWM as enable
     // TODO: make this boolean
     pwmGenThree.outputEnable[1] = 1u;
   }
   else
   {
      //Do nothing
   }
}
  
//------------------------------------------------------------------------------
//   unsigned char PWMGetState( LED_NUMBER_ENUM ledNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function returns the current state (enable/Disable) of PWM
//  
//------------------------------------------------------------------------------

unsigned char PWMGetState( LED_NUMBER_ENUM ledNumber )
{
   // current state of PWM
   unsigned char isPWMEnable = 0u;
   if(ledNumber == LED_NUMBER_1)
   {
       isPWMEnable = pwmGenOne.outputEnable[0];
   }
   else if (ledNumber == LED_NUMBER_2)
   {
       isPWMEnable = pwmGenOne.outputEnable[1];
   }
   else if (ledNumber == LED_NUMBER_3)
   {
       isPWMEnable = pwmGenTwo.outputEnable[0];
   }
   else if (ledNumber == LED_NUMBER_4)
   {
      isPWMEnable = pwmGenTwo.outputEnable[1];
   }
   else if (ledNumber == LED_NUMBER_5)
   {
      isPWMEnable = pwmGenThree.outputEnable[0];
   }
   else if (ledNumber == LED_NUMBER_6)
   {
      isPWMEnable =pwmGenThree.outputEnable[1];
   }
   else
   {
      //Do nothing
   }
   return isPWMEnable;
}

//------------------------------------------------------------------------------
//   PWMInit( void )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function initializes the PWM
//  
//------------------------------------------------------------------------------

void PWMInit( void )
{
    // The PWM peripheral must be enabled for use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    // Enable port F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    // Wait for enabling of the port
    SysCtlDelay(10);
    // Enable port G
    // TODO: Update the port when board arrived
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    // Wait for enabling of the port
    SysCtlDelay(10);
    // Unlock the Pin PF0 and Set the Commit Bit
    // See datasheet table 10-1 for explanation of
    // why this pin needs unlocking
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR)   |= 0x01;
    // Configure the GPIO pin muxing to select PWM00 functions for these pins.
    // This step selects which alternate function is available for these pins.
    // This is necessary if your part supports GPIO pin function muxing.
    // Consult the data sheet to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    GPIOPinConfigure(GPIO_PF0_M0PWM0);
    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinConfigure(GPIO_PF2_M0PWM2);
    GPIOPinConfigure(GPIO_PF3_M0PWM3);
    GPIOPinConfigure(GPIO_PG0_M0PWM4);
    GPIOPinConfigure(GPIO_PG1_M0PWM5);
    // Enable the pin type (TODO: Update after the arrival of board
    GPIOPinTypePWM(LED_1_PORT, LED_1_PIN | LED_2_PIN | LED_3_PIN | LED_4_PIN );
    GPIOPinTypePWM(LED_5_PORT, LED_5_PIN | LED_6_PIN );
    // Set the clock
    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_8);
}

//==============================================================================
//  End Of File
//==============================================================================

