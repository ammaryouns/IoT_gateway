//==============================================================================
//
//  PWMWrapper.h
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
//  Source:        PWMWrapper.h
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
//! The header file contains the functions required for running PWM.
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//  Revision: 1.0  2016/11/08  Ali Zulqarnain Anjum
//      Initial Revision 
//
//==============================================================================

#ifndef __PWMWRAPPER_H__
#define __PWMWRAPPER_H__

//==============================================================================
//  INCLUDES 
//==============================================================================

//==============================================================================
//  GLOBAL CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

#define LED_1_PORT   GPIO_PORTF_BASE
#define LED_1_PIN    GPIO_PIN_0
#define LED_2_PORT   GPIO_PORTF_BASE
#define LED_2_PIN    GPIO_PIN_1
#define LED_3_PORT   GPIO_PORTF_BASE   //TODO: Board Update
#define LED_3_PIN    GPIO_PIN_2        //TODO: Board Update
#define LED_4_PORT   GPIO_PORTF_BASE   //TODO: Board Update
#define LED_4_PIN    GPIO_PIN_3        //TODO: Board Update
#define LED_5_PORT   GPIO_PORTG_BASE   //TODO: Board Update
#define LED_5_PIN    GPIO_PIN_0        //TODO: Board Update
#define LED_6_PORT   GPIO_PORTG_BASE   //TODO: Board Update
#define LED_6_PIN    GPIO_PIN_1        //TODO: Board Update
#define PWM_GENERATOR_0   0
#define PWM_GENERATOR_1   1
#define PWM_GENERATOR_2   2

//==============================================================================
//  GLOBAL DATA STRUCTURES DEFINITION
//==============================================================================

//==============================================================================
//  GLOBAL DATA
//==============================================================================

// LED Number
typedef enum
{
    LED_NUMBER_1 = 0,  //First LED
    LED_NUMBER_2 = 1,  //Second LED
    LED_NUMBER_3 = 2,  //Third LED
    LED_NUMBER_4 = 3,  //Fourth LED
    LED_NUMBER_5 = 4,  //Fifth LED
    LED_NUMBER_6 = 5,  //Sixth LED
}LED_NUMBER_ENUM;
// PWM Generator Settings
typedef struct
{
   unsigned char Number;               //Generator Number
   unsigned short freqKHz;            //Generator frequency in Khz
   unsigned char dutyCycle;           //Generator %duty cycle
   unsigned char outputEnable[2];     //Generator's output
}PWM_GENERATOR_SETTINGS;

//==============================================================================
//  EXTERNAL OR GLOBAL FUNCTIONS
//==============================================================================
//------------------------------------------------------------------------------
//   PWMSetFrequency( unsigned char genNumber, unsigned short freqKhz)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function is used to set the frequency of PWM
//  
//------------------------------------------------------------------------------

void PWMSetFrequency( unsigned char genNumber, unsigned short freqKhz);

//------------------------------------------------------------------------------
//   unsigned short PWMGetFrequency( unsigned char genNumber )
//
//   Date:     2016/11/08
//
//!  This function returns the current Frequency of PWM
//  
//------------------------------------------------------------------------------

unsigned short PWMGetFrequency( unsigned char genNumber );

//------------------------------------------------------------------------------
//   PWMSetDutyCycle( unsigned char genNumber, unsigned char dutyCycle)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function is used to set the duty cycle of PWM
//  
//------------------------------------------------------------------------------

void PWMSetDutyCycle( unsigned char genNumber, unsigned char dutyCycle);

//------------------------------------------------------------------------------
//   unsigned char PWMGetDutyCycle( unsigned char genNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function returns the current duty cycle of PWM
//  
//------------------------------------------------------------------------------

unsigned char PWMGetDutyCycle( unsigned char genNumber );

//------------------------------------------------------------------------------
//   PWMStart( unsigned char genNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function starts the PWM
//  
//------------------------------------------------------------------------------

void PWMStart( unsigned char genNumber  );

//------------------------------------------------------------------------------
//   PWMDisable( LED_NUMBER_ENUM ledNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function stops the PWM
//  
//------------------------------------------------------------------------------

void PWMDisable( LED_NUMBER_ENUM ledNumber );

//------------------------------------------------------------------------------
//   PWMEnable( LED_NUMBER_ENUM ledNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function stops the PWM
//  
//------------------------------------------------------------------------------

void PWMEnable( LED_NUMBER_ENUM ledNumber );

//------------------------------------------------------------------------------
//   unsigned char PWMGetState( LED_NUMBER_ENUM ledNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function returns the current state (enable/Disable) of PWM
//  
//------------------------------------------------------------------------------

unsigned char PWMGetState( LED_NUMBER_ENUM ledNumber );

//------------------------------------------------------------------------------
//   PWMInit( void )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/08
//
//!  This function initializes the PWM
//  
//------------------------------------------------------------------------------

void PWMInit( void );



#endif /* __DATALOGSESSION_H__ */
//==============================================================================
//  End Of File
//==============================================================================