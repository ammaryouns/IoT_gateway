//==============================================================================
// Start of file
//==============================================================================
//
//  Initialization.c
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
//  Source:        Initialization.c
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
//! This module contains functions that are used to handle power-on
//! initialization.
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
#include "Initialization.h"
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include "Board.h"
#include "USBCDCD.h"
#include "TM4CSPI.h"
#include "TM4CUART.h"
#include "TM4CTIMER.h"
#include "TM4CADC.h"
#include "TM4CFlash.h"
#include "TM4CI2C.h"
#include "SelfTest.h"
#include "TM4CPWM.h"
#include "LEDs.h"
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include "Dataflash.h"
#include "ErrorLog.h"

//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================
extern Event_Struct evtStruct;
extern Event_Handle evtHandle;
//==============================================================================
// LOCAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
// LOCAL DATA DEFINITIONS
//==============================================================================
static bool selfTestStepStatus = false;
//==============================================================================
// GLOBAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
// GLOBAL DATA DEFINITIONS
//==============================================================================

static INIT_STEP_ENUM initializationStep = INIT_STEP_1;

//==============================================================================
// LOCAL FUNCTIONS PROTOTYPES
//==============================================================================
//
static void PowerOnDataflashInitialize(void);
static void PowerOnLogsInitialize(void);
static void PowerOnBatteryInitialize(void);
static void PowerOnFlashCRCCheck(void);
static void PowerOnInstrumentParametersInitialize(void);
static void PoweOnLENSInitialize(void);
static void PowerOnNetworkConnectivityPeripheralsInitialize(void);
static void PowerOnNetworkConnectivityInitialize(void);
static void PowerOnNFCInitialize(void);
static void PowerOnBTLEInitialize(void);
static void PowerOnTimeInitialize(void);
static void PowerOnLocationInitialize(void);
//
//==============================================================================
// LOCAL FUNCTIONS IMPLEMENTATAIONS
//==============================================================================

//==============================================================================
//
//  PowerOnDataflashInitialize(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/01/12
//
//! This function handles the initialization and self-test of dataflash during power-on.
// 
//==============================================================================

static void PowerOnDataflashInitialize(void)
{
    // Initialize dataflash
    DataFlashInit();
    // Perform dataflash diagnostics
    // Todo Temporarily commenting code for dependency of other modules
    // selfTestStepStatus = SelfTestHandler(SELFTEST_STEP_DATAFLASH);
    selfTestStepStatus = true;
    
}

//==============================================================================
//
//  PowerOnLogsInitialize(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/01/12
//
//! This function handles the initialization of logs during power-on
// 
//==============================================================================

static void PowerOnLogsInitialize(void)
{
    // Initialize error log
    // Todo Temporarily commenting code for dependency of other modules
    // ErrorLogInit();    
    selfTestStepStatus = true;
}

//==============================================================================
//
//  PowerOnBatteryInitialize(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/01/12
//
//! This function handles the initialization and self-test of battery during power-on.
// 
//==============================================================================

static void PowerOnBatteryInitialize(void)
{
    // Initialize battery controller IC
    //! Todo battery initialization function goes here
    
    // Perform dataflash diagnostics
    // Todo Temporarily commenting code for dependency of other modules
    // selfTestStepStatus = SelfTestHandler(SELFTEST_STEP_BATTERY);
    selfTestStepStatus = true;
    
    
}

//==============================================================================
//
//  PowerOnFlashCRCCheck(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/01/12
//
//! This function handles the validation of application code CRC during power-on.
//
//==============================================================================

static void PowerOnFlashCRCCheck(void)
{
    // Perform application code CRC diagnostics
    // Todo Temporarily commenting code for dependency of other modules
    // selfTestStepStatus = SelfTestHandler(SELFTEST_STEP_FLASHCRC);
    selfTestStepStatus = true;
}

//==============================================================================
//
//  PowerOnInstrumentParametersInitialize(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/01/12
//
//! This function handles the initialization and validation of instrument
//! confiugration parameters during power-on.
//
//==============================================================================

static void PowerOnInstrumentParametersInitialize(void)
{
    // !Todo Initialize intrument parameters
    // Perform instrument parameters CRC diagnostics
    // Todo Temporarily commenting code for dependency of other modules
    // selfTestStepStatus = SelfTestHandler(SELFTEST_STEP_INSTRUMENT_PARAM);
    selfTestStepStatus = true;
}

//==============================================================================
//
//  PoweOnLENSInitialize(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/01/12
//
//! This function handles the initialization and test of LENS communication.
//
//==============================================================================

static void PoweOnLENSInitialize(void)
{
    // !Todo LENS initialization function goes here
    // Perform LENS validation
    // Todo Temporarily commenting code for dependency of other modules
    // selfTestStepStatus = SelfTestHandler(SELFTEST_STEP_LENS);
    selfTestStepStatus = true;
}

//==============================================================================
//
//  PowerOnNetworkConnectivityPeripheralsInitialize(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/01/12
//
//! This function handles the initialization of network connectivity peripherals
//
//==============================================================================

static void PowerOnNetworkConnectivityPeripheralsInitialize(void)
{
    // Todo Temporarily commenting code for dependency of other modules
    // selfTestStepStatus = SelfTestHandler(SELFTEST_STEP_CORE_CONN_PERIPH);
    selfTestStepStatus = true;
}

//==============================================================================
//
//  PowerOnNetworkConnectivityInitialize(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/01/12
//
//! This function handles the initialization of network connectivity via Ethernet,
//! Cellular Wireless or Wifi as per user configured priority.
//
//==============================================================================

static void PowerOnNetworkConnectivityInitialize(void)
{
    // Todo Temporarily commenting code for dependency of other modules
    // selfTestStepStatus = SelfTestHandler(SELFTEST_STEP_CORE_CONN);
    selfTestStepStatus = true;
}

//==============================================================================
//
//  PowerOnNFCInitialize(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/01/12
//
//! This function handles initialization and test of NFC module during power-on.
//
//==============================================================================

static void PowerOnNFCInitialize(void)
{
    // !Todo NFC initialization function goes here, if needed.
    // Perform NFC hardware diagnostics
    // Todo Temporarily commenting code for dependency of other modules
    // selfTestStepStatus = SelfTestHandler(SELFTEST_STEP_NFC_HARD);
    selfTestStepStatus = true;
}

//==============================================================================
//
//  PowerOnBTLEInitialize(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/01/12
//
//! This function handles initialization and test of BTLE module during power-on.
//
//==============================================================================

static void PowerOnBTLEInitialize(void)
{
    // !Todo BTLE initialization function goes here, if needed.
    // Perform BTLE hardware diagnostics
    // Todo Temporarily commenting code for dependency of other modules
    // selfTestStepStatus = SelfTestHandler(SELFTEST_STEP_BTLE_HARD);
    selfTestStepStatus = true;
}

//==============================================================================
//
//  PowerOnTimeInitialize(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/01/12
//
//! This function handles the update of instrument time to UTC during power-on
//
//==============================================================================

static void PowerOnTimeInitialize(void)
{  
    // Perform real time update via internet or cellular network
    // Todo Temporarily commenting code for dependency of other modules
    // selfTestStepStatus = SelfTestHandler(SELFTEST_STEP_TIME_UPDATE);
    selfTestStepStatus = true;
}

//==============================================================================
//
//  PowerOnLocationInitialize(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/01/12
//
//! This function handles the update of instrument location via GPS during power-on
//
//==============================================================================

static void PowerOnLocationInitialize(void)
{
    // Todo Temporarily commenting code for dependency of other modules
    // selfTestStepStatus = SelfTestHandler(SELFTEST_STEP_GPS_HARD);
    selfTestStepStatus = true;
}

//==============================================================================
// GLOBAL  FUNCTIONS IMPLEMENTATAIONS
//==============================================================================

//==============================================================================
//
//  InitializationHandler(uint32_t *taskSleepValue, bool *canInitProcessProceed)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function is responsible for power-on initialization and self-test.
//
//==============================================================================

INIT_STEP_ENUM InitializationHandler(uint32_t *taskSleepValue, bool *canInitProcessProceed)
{ 
    
    switch(initializationStep)
    {
        // Initialize communication and other peripherals drivers before
        // they can be accessed
    case INIT_STEP_1:      
        //Initialize SPI driver
        Board_initSPI();
        // Initialize I2C driver
        Board_initI2C();
        // Initialize UART driver
        Board_initUART();   
        // Initialize ADC drviver
        ADCInit();        
        // Initialize PWM driver
        PWMInit(); 
        // Put initialization task to sleep for 50 ms
        *taskSleepValue = 50u;        
        // Set the initilization process step status
        *canInitProcessProceed = true;
        System_printf("Initialization Started \n");     
        System_flush();
        break;
        
        // Initialize dataflash before any other peripherals.        
    case INIT_STEP_2:
        // Reset all LEDS
        DiagnosticsLEDsAllOff();
        // Initialize dataflash
        PowerOnDataflashInitialize();
        // Put initialization task to sleep for 50 ms
        *taskSleepValue = 50u;
        // Set the initilization process step status
        *canInitProcessProceed = selfTestStepStatus;        
        break;
        
        // Initialize logs(Error log, event log) and battery
    case INIT_STEP_3:
        // Initialize logs
        PowerOnLogsInitialize();
        // Initialize battery
        PowerOnBatteryInitialize();
        // Put initialization task to sleep for 50 ms
        *taskSleepValue = 50u;
        // Set the initilization process step status
        *canInitProcessProceed = selfTestStepStatus;        
        break;
        
    case INIT_STEP_4:
        // Perform Flash CRC check
        PowerOnFlashCRCCheck();
        //!Todo DiagnosticsLEDs(LED_NUMBER_1,true);
        *taskSleepValue = 50u;
        // Set the initilization process step status
        *canInitProcessProceed = selfTestStepStatus;     
        break;
        
    case INIT_STEP_5:
        // Initialize and validate instrument parameters
        PowerOnInstrumentParametersInitialize();                
        // Start diagnostics LED No.2
        DiagnosticsLEDs(LED_NUMBER_2,true);
        *taskSleepValue = 500u;
        // Set the initilization process step status
        *canInitProcessProceed = selfTestStepStatus; 
        break;
        
    case INIT_STEP_6:
        // Stop diagnostics LED No.2
        DiagnosticsLEDs(LED_NUMBER_2,false);
        // Initialize and validate LENS
        PoweOnLENSInitialize();
        // Start diagnostics LED No.3
        DiagnosticsLEDs(LED_NUMBER_3,true);
        *taskSleepValue = 500u;
        // Set the initilization process step status
        *canInitProcessProceed = selfTestStepStatus;
        break;
        
    case INIT_STEP_7:        
        // Stop diagnostics LED No.3
        DiagnosticsLEDs(LED_NUMBER_3,false);
        // Initialize and validate core connectivity peripherals
        PowerOnNetworkConnectivityPeripheralsInitialize();                
        // Start diagnostics LED No.4
        DiagnosticsLEDs(LED_NUMBER_4,true);
        *taskSleepValue = 500u;
        // Set the initilization process step status
        *canInitProcessProceed = selfTestStepStatus;        
        break;
        
    case INIT_STEP_8:
        // Stop diagnostics LED No.4
        DiagnosticsLEDs(LED_NUMBER_4,false);
        // Initialise core server connectivity
        PowerOnNetworkConnectivityInitialize();                
        // Start diagnostics LED No.5
        DiagnosticsLEDs(LED_NUMBER_5,true);
        *taskSleepValue = 500u;
        // Set the initilization process step status
        *canInitProcessProceed = selfTestStepStatus;        
        break;
        
    case INIT_STEP_9:
        // Stop diagnostics LED No.5
        DiagnosticsLEDs(LED_NUMBER_5,false);
        // Initialize NFC and BTLE
        PowerOnNFCInitialize();
        PowerOnBTLEInitialize();    
        // Start diagnostics LED No.6
        DiagnosticsLEDs(LED_NUMBER_6,true);
        *taskSleepValue = 500u;
        // Set the initilization process step status
        *canInitProcessProceed = selfTestStepStatus;        
        break;
        
    case INIT_STEP_10:        
        // Stop diagnostics LED No.6
        DiagnosticsLEDs(LED_NUMBER_6,false);
        // Initialize Time and Location
        PowerOnTimeInitialize();
        PowerOnLocationInitialize();                
        *taskSleepValue = 50u;
        // Start HTTP Client task
        Event_post(evtHandle, Event_Id_00);
        break;
        
    default:        
        break;   
        
    }  
    // Increment the initialization step
    initializationStep++;
    
    return initializationStep;
}

//==============================================================================
//  End Of File
//==============================================================================