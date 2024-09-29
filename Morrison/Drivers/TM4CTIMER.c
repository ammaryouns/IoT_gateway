//==============================================================================
// Start of file
//==============================================================================
//
//  TM4CTIMER.c
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
//  Source:        TM4CTIMER.c
//
//  Project:       Morrison
//
//  Author:        Muhammad Saleem
//
//  Date:          2016/21/11
//
//  Revision:      1.1
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This is Timer module.
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//
//  Revision: 1.0 2016/21/11 Muhammad Saleem
//           Initial Version
//
//  Revision: 1.1 2016/08/12 Muhammad Shuaib
//!          Updated Timer initialization and IRQ handler functions
//
//==============================================================================

//==============================================================================
//  INCLUDES
//==============================================================================
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "TM4CTIMER.h"
#include "Button.h"
#include "GPIO.h"
#include "Board.h"

// include files for RTOS timer
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/hal/Timer.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Diags.h>
#include "Main.h"

//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================

#define TASKSTACKSIZE   512
#define BUTTON_TIMER_ID 2

//==============================================================================
// LOCAL DATA DECLARATIONS
//==============================================================================

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];
Swi_Struct swi0Struct;
Swi_Handle swi0Handle;
Semaphore_Struct sem0Struct;
Semaphore_Handle sem0Handle;

//==============================================================================
// LOCAL DATA DEFINITIONS
//==============================================================================

//==============================================================================
// LOCAL FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
// LOCAL FUNCTIONS IMPLEMENTATIONS
//==============================================================================

//==============================================================================
// GLOBAL FUNCTIONS IMPLEMENTATIONS
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
//! This function initilzizes RTOS Timer
//
//==============================================================================

void RTOSTimerInit(void)
{	
    // Timer parameters
    Timer_Params buttonTimerParams; 
    
    // Initializae timer parameters
    Timer_Params_init(&buttonTimerParams);
    // Configure timer period type as micro seconds
    buttonTimerParams.periodType = Timer_PeriodType_MICROSECS;
    // Configure timer for 20 milli seconds interrupt
    buttonTimerParams.period = (1000u*20);
    // Set timer run mode to one shot
    buttonTimerParams.runMode = Timer_RunMode_ONESHOT;
    // Set timer shall be started by user
    buttonTimerParams.startMode = Timer_StartMode_USER;
    // Set the timer argument
    buttonTimerParams.arg = BUTTON_TIMER_ID;
    // Create the timer
    buttonTimerHandle = Timer_create(BUTTON_TIMER_ID, (Timer_FuncPtr)RTOSTimerIRQHandler, &buttonTimerParams, NULL);
    // If timer was not created successfully
    if(buttonTimerHandle == NULL)
    {
        // Abort
        System_abort("Timer could not be created");
    }    
}

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

void ButtonTimerInit(void)
{    
    // Variable to calculte timer counter value
    unsigned int timerCounterValue = 0u;
    
    // Enable Timer0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    // Wait for the Timer0 module to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
    // Configure Timer0 for concatinated one shot mode    
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);
    // Calculate timer load value for 40ms interrupt
    timerCounterValue = ((SYSTEM_CLOCK_FREQUENCY/1000u)*40u);
    // Load timer counter value
    TimerLoadSet(TIMER0_BASE, TIMER_BOTH,timerCounterValue);
    // Enable processor interrupts 
    IntMasterEnable();
    // Configure the Timer interrupt for timer timeout.
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Enable Timer
    TimerEnable(TIMER0_BASE, TIMER_BOTH);
    // Register Timer IRQ handler
    TimerIntRegister(TIMER0_BASE, TIMER_BOTH, ButtonTimerIRQHandler);    
}

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
                                     )
{
    // Variable to calculte timer counter value
    unsigned int timerCounterValue = 0u;
    
    // Calculate timer load value for required delay value
    timerCounterValue = ((SYSTEM_CLOCK_FREQUENCY/1000u)*timerDelayValue);
    // Load timer counter value
    TimerLoadSet(TIMER0_BASE, TIMER_BOTH,timerCounterValue);
    TimerEnable(TIMER0_BASE, TIMER_BOTH);    
}

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

void ButtonTimerDisable(void)
{
    // Clear any pending timer interrupts
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Disable the timer
    TimerDisable(TIMER0_BASE, TIMER_BOTH);
}

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
                    )
{
    // Variable to calculte timer counter value
    unsigned int timerCounterValue = 0u;
    
    // Enable Timer0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    // Wait for Timer1 module to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))
    {
    }
    // Configure Timer1 for concatinated one shot mode
    TimerConfigure(TIMER1_BASE, TIMER_CFG_ONE_SHOT );
    // Calculate Timer counter value based on system clock frequency
    timerCounterValue = ((120000000u/1000u)*timerPeriod);
    // Load the counter value
    TimerLoadSet(TIMER1_BASE, TIMER_BOTH,timerCounterValue);
    // Enable processor interrupts.    
    IntMasterEnable();
    // Configure the Timer1 interrupt for timer timeout.    
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    // Enable Timer1
    TimerEnable(TIMER1_BASE, TIMER_BOTH);
    // Register Timer1 IRQ handler
    TimerIntRegister(TIMER1_BASE, TIMER_BOTH, Timer1IRQHandler );    
}

//==============================================================================
// IRQ HANDLERS IMPLEMENTATIONS
//==============================================================================

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

void RTOSTimerIRQHandler(UArg arg)
{
    // Process timer interrupt for buttons
    ButtonProcessTimerInterrupt();  
}

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

void ButtonTimerIRQHandler(void)
{
    // Variable to add temporary delay for timer interrupt to be cleared
    unsigned char count;
    
    // Clear the timer interrupt flag.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Add a little bit delay
    for(count = 0; count <= 100u ; count++)
    {
    }            
    // Process timer interrupt for buttons
    ButtonProcessTimerInterrupt();
}

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

void Timer1IRQHandler(void)
{
    // Variable to add temporary delay for timer interrupt to be cleared
    unsigned char count;
    
    // Clear the timer interrupt flag.
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT | TIMER_TIMB_TIMEOUT);
    // Add a little delay
    for(count=0 ; count <= 100u ; count++)
    {
    }    
}
//==============================================================================
// END OF FILE
//==============================================================================