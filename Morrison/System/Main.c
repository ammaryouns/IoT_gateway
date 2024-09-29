//==============================================================================
//
//  Main.c
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
//  Source:        Main.c
//
//  Project:       Morrison
//
//  Author:        Fehan Arif Malik
//
//  Date:          2016/11/17
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This is main module for Morrison firmware. 
//!
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//  Revision: 1.0  2016/11/17  Fehan Arif
//      Initial version of Morrison
//
//==============================================================================
//  INCLUDES
//==============================================================================

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
#include <TM4CWiFi.h>
#include <WiFiCC3100.h>
#include "Board.h"
#include "USBCDCD.h"
#include "TM4CSPI.h"
#include "TM4CUART.h"
#include "TM4CTIMER.h"
//#include "TM4CADC.h"
#include "TM4CRTC.h"
#include "TM4CFlash.h"
#include "TM4CI2C.h"
#include "script_uploader.h"
#include "Initialization.h"
#include "Button.h"
#include "ErrorLog.h"
#include "DataFlash.h"
#include "EventLog.h"
#include "TM4CEEPROM.h"
#include "Main.h"
//==============================================================================
//  CONSTANTS, TYPEDEFS AND MACROS
//==============================================================================
#define DEFAULT_TASK_PRIORITY   5
#define DEFAULT_TASKSTACKSIZE   512
#define TEST_TASK_SIZE          16896
//==============================================================================
//  LOCAL DATA STRUCTURE DEFINITION
//==============================================================================
const unsigned char usbToSerialText[] = "Tiva USB ...\r\n";
Task_Handle taskWebserver;
Task_Handle taskBLE;
Task_Handle taskShell;
Task_Handle taskStatus;
Task_Handle taskNFC;
Task_Handle taskEventLog;
Task_Handle taskTCPClient;
Task_Handle taskWhisper;
Task_Handle taskParsing;
Task_Handle taskInitialization;
Task_Handle taskBattery;
Task_Handle taskUSBDataRecieve;

Event_Struct evtStruct;
Event_Handle evtHandle;

//==============================================================================
//  GLOBAL DATA DECLARATIONS
//==============================================================================

// Semaphore to be posted by button module to start init task
Semaphore_Struct buttonSemaphoreStruct;
Semaphore_Handle buttonSemaphoreHandle;
// Button timer handle
Timer_Handle buttonTimerHandle;

//==============================================================================
//  LOCAL DATA DECLARATIONS
//==============================================================================

static bool isDeviceInPeeking = false;
static bool isInitializationComplete = false;
//==============================================================================
//  LOCAL FUNCTION PROTOTYPES
//==============================================================================

void TaskInitialization(void);
void TaskWebserver(UArg arg0, UArg arg1);
void TaskBLE(UArg arg0, UArg arg1);
void TaskShell(void);
void TaskStatus(void);
void TaskNFC(void);
void TaskEventLog(void);
void TaskTCPClient(void);
void TaskWhisper(void);
void TaskParsing(void);
void TaskBattery(void);
void TaskIdle(void);
void SetIsDeviceInPeeking(bool);
bool GetIsDeviceInPeeking(void);

//==============================================================================
//  LOCAL FUNCTIONS IMPLEMENTATION
//==============================================================================

//------------------------------------------------------------------------------
//   TaskInitialization(void)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------

void TaskInitialization(void)
{
    // This variable is used to control the execution of initialization handler
    static unsigned char initTaskStatus = 0;
    // This variable is used to save the status of 
    static bool canInitializationTaskProceed = true;
    // This variable is used to control for how long initialization task sleeps
    uint32_t initTaskSleepValue = 0;
    
    // If button has not been pressed to start power-on
    if(ButtonGetCanStartInitTask() == false)
    {
      // Wait for semaphore to be posted
      Semaphore_pend(buttonSemaphoreHandle,BIOS_WAIT_FOREVER);
    }
    else
    {
      // Control shall never come here
    }
    
    while(1)
    {
      // If initialization task is not complete and last self-test step was successful
      if((initTaskStatus < INIT_STEP_LIM) && (canInitializationTaskProceed == true))
      {          
          // Continue with next step in initialization and self-test
          initTaskStatus = InitializationHandler(&initTaskSleepValue,&canInitializationTaskProceed);
          // Put the initialization task to sleep for some time for initialization to take effect
          Task_sleep(initTaskSleepValue);
      }
      // Otherwise
      else
      {
          // If initialization task completed successfully
          if(canInitializationTaskProceed == true)
          {
              // !Todo Post a semaphore, if necessary, indicating successful completion of initialization task              
              isInitializationComplete = true;
          }          
          else
          {
              // Activate system alarm indicating unsuccessful initialization              
          }
          // Exit the initialization task
          Task_exit();
      }      
    }
}
//------------------------------------------------------------------------------
//   TaskWebserver(void)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------

void TaskWebserver(UArg arg0, UArg arg1)
{    
    while(1) 
    {
        //System_printf("In TaskWebserver \n");     
        //System_flush();
        Task_sleep(100);
    }
}
//------------------------------------------------------------------------------
//   TaskBLE(UArg arg0, UArg arg1)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------

void TaskBLE(UArg arg0, UArg arg1)
{
    
    while(1)
    {
        //System_printf("In BLE \n");     
        //System_flush();
        Task_sleep(5);
    }
}
//------------------------------------------------------------------------------
//   TaskShell(void)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------

void TaskShell(void)
{
    
    while(1)
    {
        /* Block while the device is NOT connected to the USB */
        USBCDCD_waitForConnect(BIOS_WAIT_FOREVER);
        USBCDCD_sendData(usbToSerialText, sizeof(usbToSerialText), BIOS_WAIT_FOREVER);
        /* Send data periodically */
        //          //System_printf("In TaskShell \n");     
        //          //System_flush();
        Task_sleep(5);
        
    }
}

//------------------------------------------------------------------------------
//   TaskStatus(void)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------

void TaskStatus(void)
{
    static unsigned char isInit = 0u;   
    unsigned char dummy[16] = {0};
    unsigned short RSSI = 0u;
    bool isWriteCorrect = false;
    unsigned int dataD = 0u;
    while(1)
    {
      /*  if ( isInit == 0 )
        {
          (void) TM4CEEPROMInit();
           DataFlashInit();
           EventLogInit();
           //Write in EEPROM
           isInit = isInit + 1;
        }          
        else if ( isInit == 1 )
        {
           EventLogWriteInstrumentLostEvent(1);
           //isWriteCorrect = TM4CEEPROMWriteData(&dataD,(unsigned char) 0, (unsigned char) 0, (unsigned char) 1 );
           //isWriteCorrect = TM4CEEPROMWriteData(&dataD,(unsigned char) 0, (unsigned char)1, (unsigned char) 1 );
           isInit = isInit + 1;
        } 
        else if ( isInit == 2 )
        {
           EventLogWriteLeaveGroupEvent(1);
           isInit = isInit + 1;
        }
        else if ( isInit == 3 )
        {
           EventLogWriteManDownEvent(1);
           //EventLogShutDown();
           isInit = isInit + 1;
        }
        else if ( isInit == 4 )
        {
          // EventLogWriteManDownClearEvent(1);
           //EventLogInit();
           isInit = isInit + 1;
        }
        else if ( isInit == 5 )
        {
           EventLogWritePanicEvent(1);
           isInit = isInit + 1;
        }
        else if ( isInit == 6 )
        {
           EventLogWritePanicClearEvent(1);
           isInit = isInit + 1;
        }
        else if ( isInit == 7 )
        {
           EventLogWritePumpEvent(1);
           isInit = isInit + 1;
        }
        else if ( isInit == 8 )
        {
           EventLogWritePumpClearEvent(1);
           isInit = isInit + 1;
        }
        else if ( isInit == 9 )
        {
           EventLogWriteGasAlarmEvent(EVENTLOG_ID_HIGH_ALARM_EVENT ,1);
           isInit = isInit + 1;
        }
        else if ( isInit == 10 )
        {
           EventLogWriteGasAlarmEvent(EVENTLOG_ID_LOW_ALARM_EVENT  ,1);
           isInit = isInit + 1;
        }
        else if ( isInit == 11 )
        {
           EventLogWriteGasAlarmEvent(EVENTLOG_ID_STEL_ALARM_EVENT ,1);
           isInit = isInit + 1;
        }
        else if ( isInit == 12 )
        {
           EventLogWriteGasAlarmEvent(EVENTLOG_ID_TWA_ALARM_EVENT  ,1);
           isInit = isInit + 1;
        }
        else if ( isInit == 13 )
        {
           EventLogWriteGasAlarmClearEvent(1);
           isInit = isInit + 1;
        }
        else if ( isInit == 14 )
        {
           EventLogWriteInstrumentJoinEvent(1);
           isInit = isInit + 1;
        }
        else if ( isInit == 15 )
        {
           EventLogWriteUserUpdateEvent(1);
           isInit = isInit + 1;
        }
        else if ( isInit == 16 )
        {
           EventLogWriteSensorUpdateEvent(1);
           isInit = isInit + 1;
        }
        else if ( isInit == 17 )
        {
           EventLogWriteLPStatusEvent(EVENTLOG_ID_LPONLINE_EVENT);
           isInit = isInit + 1;
        }
        else if ( isInit == 18 )
        {
           EventLogWriteLPStatusEvent(EVENTLOG_ID_LPOFFLINE_EVENT);
           isInit = isInit + 1;
        }
        else if ( isInit == 19 )
        {
           EventLogWriteLPStatusEvent(EVENTLOG_ID_LP_KEEPALIVE_EVENT);
           isInit = isInit + 1;
        }
        else if ( isInit == 20 )
        {
           EventLogWriteLPBatteryEvent(EVENTLOG_ID_NO_LB_EVENT);
           isInit = isInit + 1;
        }
        else if ( isInit == 21 )
        {
           EventLogWriteLPBatteryEvent(EVENTLOG_ID_LP_KEEPALIVE_EVENT);
           isInit = isInit + 1;
        }
        else if ( isInit == 22 )
        {
           EventLogWriteLPSiteUpdateEvent();
           isInit = isInit + 1;
        }
        else if ( isInit == 23 )
        {
           EventLogWriteLPSGPSEvent(dummy);
           isInit = isInit + 1;
        }
        else if ( isInit == 24 )
        {
           EventLogWriteIMEIUpdateEvent(dummy);
           isInit = isInit + 1;
        }
        else if ( isInit == 25 )
        {
           EventLogWriteRSSIUpdateEvent(RSSI);
           isInit = isInit + 1;
        }
        else if ( isInit == 26 )
        {
           EventLogWriteErrorStatusEvent(RSSI);
           isInit = isInit + 1;
        }
        else if ( isInit == 27 )
        {
           EventLogWriteErrorStatusEvent(RSSI);
           isInit = isInit + 1;
        }
        else if ( isInit == 28 )
        {
           EventLogWriteErrorStatusEvent(RSSI);
           isInit = isInit + 1;
        }
        else if ( isInit == 29 )
        {
           EventLogWriteErrorStatusEvent(RSSI);
           isInit = isInit + 1;
        }
        else if ( isInit == 30 )
        {
           EventLogWriteErrorStatusEvent(RSSI);
           isInit = isInit + 1;
        }
        else if ( isInit == 31 )
        {
           EventLogWriteErrorStatusEvent(RSSI);
           isInit = isInit + 1;
        }
        else if ( isInit == 32 )
        {
           EventLogWriteErrorStatusEvent(RSSI);
           isInit = isInit + 1;
        }
        else if ( isInit == 33 )
        {
           //DataFlashReadWord((subsectorNumber - 1),0x00,&RSSI);
           //DataFlashReadSector((subsectorNumber - 1),eventLogReadArray);
           isInit = isInit + 1;
        }          
        else
        {
           //do nothing
        }*/
        Task_sleep(5);
    }
}

//------------------------------------------------------------------------------
//   TaskNFC(void)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------

void TaskNFC(void)
{
    
    while(1)
    {
        //System_printf("In TaskNFC \n");     
        //System_flush();
        Task_sleep(5);
    }
}
//------------------------------------------------------------------------------
//   TaskEventLog(void)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------

void TaskEventLog(void)
{
    while(1)
    {
        //System_printf("In TaskEventLog \n");     
        //System_flush();
        Task_sleep(5);
    }
}
//------------------------------------------------------------------------------
//   TaskTCPClient(void)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------

void TaskTCPClient(void)
{
    while(1)
    {
        //System_printf("In TaskTCPClient \n");     
        //System_flush();
        Task_sleep(5);
    }
}

//------------------------------------------------------------------------------
//   TaskWhisper(void)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------

void TaskWhisper(void)
{
    while(1)
    {
        //System_printf("In TaskWhisper \n");     
        //System_flush();
        Task_sleep(5);
    }
}

//------------------------------------------------------------------------------
//   TaskParsing(void)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------

void TaskParsing(void)
{
    while(1)
    {
        //System_printf("In TaskParsing \n");     
        //System_flush();
        Task_sleep(5);
    }
}

//------------------------------------------------------------------------------
//   TaskBattery(void)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------


void TaskBattery(void)
{ 
    // SPI test variable
    static bool init = false; 
    SPI_Handle      handle;
    SPI_Params      params;
    SPI_Transaction spiTransaction;
    UChar transmitBuffer[20] = {'a','B','G','T','N','s'};
    UChar receiveBuffer[20];    
    Bool transferOK;
    
    // ADC test variables        
    uint32_t adcReadBuffer[2] = {0,0};
    
    // I2C test variables
    I2C_Handle i2cHandle;
    I2C_Params i2cParams;    
    I2C_Transaction i2cTransaction;
    
    // Command to Set MCP_1 PORT_A47(0x00) and PORT_B03(0x01) as output 
    uint8_t setMcp1PortA47Out[2] = {0x00,0x0F};   
    uint8_t setMcp1PortB03Out[2] = {0x01,0xF0};
    
    // Command to toggle MCP_1 PORT_A4-PORT_A7(0x12) and PORT_B0-PORT_B3(0x13) LEDs
    uint8_t mcp1PortAToggle[2] = {0x12,0};
    uint8_t mcp1PortBToggle[2] = {0x13,0};
    
    // Command to set MCP_2 PORT_A0-PORT_A3 as input    
    uint8_t setMcp2PortA03In[2] = {0x00,0x0F};   
    uint8_t setMcp2PortB03Out[2] = {0x01,0xF0};
    
    // Variable to toggle LEDs on MCP_1
    uint8_t toggleLeds = 0;
    
    // MCP_2 PORT_A address to read from
    uint8_t mcp2PortAAddress = 0x12;
    // Variable to read buttons on MCP_2
    uint8_t mcp2ReadButtons = 0;
    // Write buffer for writing the read buttons value on PORT_B0-PORT_B3
    uint8_t mcp2WriteButtonsValue[2] = {0x13,0};
    
    // This variable is used to get the message from task's mailbox
    while(1)
    {
        //        if ( init == false)
        //        {
        //            init = true;
        //            // Setup SPI test
        //            SPI_Params_init(&params);
        //            params.transferMode = SPI_MODE_BLOCKING;
        //            handle = SPI_open(Board_SPI1, &params);
        //            if (!handle) 
        //            {
        //                //System_printf("SPI did not open");
        //            }
        //            // Setup I2C test setup
        //            I2C_Params_init(&i2cParams);
        //            i2cParams.transferMode = I2C_MODE_BLOCKING;
        //            i2cParams.bitRate = I2C_400kHz;
        //            i2cHandle = I2C_open(Board_I2C1,&i2cParams);
        //            
        //            // Setup I2C transaction
        //            // Set MCP_1 PORT_A4-PORT_A7 as output
        //            i2cTransaction.slaveAddress = 0x20;
        //            i2cTransaction.writeBuf = setMcp1PortA47Out;
        //            i2cTransaction.readBuf = NULL;
        //            i2cTransaction.writeCount = 2;
        //            i2cTransaction.readCount = 0;
        //            transferOK = I2C_transfer(i2cHandle, &i2cTransaction);            
        //            
        //            // Set MCP_1 PORT_B0-PORT_B3 as output
        //            i2cTransaction.slaveAddress = 0x20;
        //            i2cTransaction.writeBuf = setMcp1PortB03Out;
        //            i2cTransaction.readBuf = NULL;
        //            i2cTransaction.writeCount = 2;
        //            i2cTransaction.readCount = 0;
        //            transferOK = I2C_transfer(i2cHandle, &i2cTransaction);
        //            
        //            // Set MCP_2 PORT_A0-PORT_A3 as input
        //            i2cTransaction.slaveAddress = 0x27;
        //            i2cTransaction.writeBuf = setMcp2PortA03In;
        //            i2cTransaction.readBuf = NULL;
        //            i2cTransaction.writeCount = 2;
        //            i2cTransaction.readCount = 0;
        //            transferOK = I2C_transfer(i2cHandle, &i2cTransaction);
        //            
        //            // Set MCP_2 PORT_B0-PORT_B3 as output
        //            i2cTransaction.slaveAddress = 0x27;
        //            i2cTransaction.writeBuf = setMcp2PortB03Out;
        //            i2cTransaction.readBuf = NULL;
        //            i2cTransaction.writeCount = 2;
        //            i2cTransaction.readCount = 0;
        //            transferOK = I2C_transfer(i2cHandle, &i2cTransaction);
        //            
        //            
        //        }
        //        // Setup SPI transaction
        //        spiTransaction.count = 5;
        //        spiTransaction.txBuf = transmitBuffer;
        //        spiTransaction.rxBuf = receiveBuffer;        
        //        transferOK = SPI_transfer(handle, &spiTransaction);
        //        if (!transferOK) 
        //        {
        //        }
        //        
        //        toggleLeds++;
        //        
        //        mcp1PortAToggle[1] = toggleLeds * 16u;
        //        mcp1PortBToggle[1] = toggleLeds;
        //        
        //        // Toggle the LEDs on MCP_1 PORTA(4-7)
        //        i2cTransaction.slaveAddress = 0x20;
        //        i2cTransaction.writeBuf = mcp1PortAToggle;
        //        i2cTransaction.readBuf = NULL;
        //        i2cTransaction.writeCount = 2;
        //        i2cTransaction.readCount = 0;
        //        transferOK = I2C_transfer(i2cHandle, &i2cTransaction); 
        //        
        //        // Toggle the LEDs on MCP_1 PORTB(0-3)
        //        i2cTransaction.slaveAddress = 0x20;
        //        i2cTransaction.writeBuf = mcp1PortBToggle;
        //        i2cTransaction.readBuf = NULL;
        //        i2cTransaction.writeCount = 2;
        //        i2cTransaction.readCount = 0;
        //        transferOK = I2C_transfer(i2cHandle, &i2cTransaction);
        //        
        //        // Read MCP_1 PORT_A0-PORT_A3
        //        i2cTransaction.slaveAddress = 0x27;
        //        i2cTransaction.writeBuf = &mcp2PortAAddress;
        //        i2cTransaction.readBuf = &mcp2ReadButtons;
        //        i2cTransaction.writeCount = 1;
        //        i2cTransaction.readCount = 1;
        //        transferOK = I2C_transfer(i2cHandle, &i2cTransaction);
        //        
        //        // Place the read value in write buffer to be written on PORTB            
        //        mcp2WriteButtonsValue[1] = mcp2ReadButtons;
        //        
        //        // Write the read buttons value on PORT_B0-PORT_B3        
        //        i2cTransaction.slaveAddress = 0x27;
        //        i2cTransaction.writeBuf = &mcp2WriteButtonsValue;
        //        i2cTransaction.readBuf = NULL;
        //        i2cTransaction.writeCount = 2;
        //        i2cTransaction.readCount = 0;
        //        transferOK = I2C_transfer(i2cHandle, &i2cTransaction);
        //        
        //System_printf("In TaskBattery \n");     
        //System_flush();
        
        //Wait for 20 second
        Task_sleep(20000);        
//        ADCReadChannel(adcReadBuffer);
        if(isInitializationComplete == TRUE)
        {
            Event_post(evtHandle, Event_Id_00);         
        }
        
    }
}

//------------------------------------------------------------------------------
//   TaskUSBDataRecieve(void)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------

void TaskUSBDataRecieve(void)
{
    // This variable is used to get the message from task's mailbox
    unsigned char data[32];
    unsigned int received;
    while(1)
    {
        /* Block while the device is NOT connected to the USB */
        USBCDCD_waitForConnect(BIOS_WAIT_FOREVER);
        received = USBCDCD_receiveData(data, 31, BIOS_WAIT_FOREVER);
        data[received] = '\0';
        if (received) {
            System_printf("Received \"%s\" (%d bytes)\r\n", data, received);
            System_flush();
        }
    }
}
//------------------------------------------------------------------------------
//   TaskIdle(void)
//
//   Author:  Fehan Arif 
//   Date:    2016/11/17
//
//!
//------------------------------------------------------------------------------

void TaskIdle(void)
{
    
    while(1)
    {
        //System_printf("In TaskIdle \n");     
        //System_flush();
        Task_sleep(5);
    }
}

//------------------------------------------------------------------------------
//   GetIsDeviceInPeeking(void)
//
//   Author:  Muhammad Shuaib 
//   Date:    2016/06/12
//
//! This function is used by other modules to get the status of isDeviceInPeeking
//
//------------------------------------------------------------------------------

bool GetIsDeviceInPeeking(void)
{
  return isDeviceInPeeking;
}

//------------------------------------------------------------------------------
//   SetIsDeviceInPeeking(void)
//
//   Author:  Muhammad Shuaib 
//   Date:    2016/06/12
//
//! This function is used by other modules to set the status of isDeviceInPeeking
//
//------------------------------------------------------------------------------

void SetIsDeviceInPeeking(bool setIsDeviceInPeeking)
{
  isDeviceInPeeking = setIsDeviceInPeeking;
}
//------------------------------------------------------------------------------
//   main(void)
//
//   Author:  Fehan Arif
//   Date:    2016/11/17
//
//!  This is the main() function.
//
//------------------------------------------------------------------------------

extern int TcpEchoInit(void);
int main(void)
{
    // Tasks' parameters stucture
    Task_Params taskParams;
    // Semaphores' parameters stucture
    Semaphore_Params semParams;   
    // Error block 
    Error_Block eb;
    
    // Initialize semaphore parameters
    Semaphore_Params_init(&semParams);
    // Construct Button semaphore
    Semaphore_construct(&buttonSemaphoreStruct, 0, &semParams);
    // Create button semaphore handle
    buttonSemaphoreHandle = Semaphore_handle(&buttonSemaphoreStruct);
    // Check if semaphore was created successfully
    if(buttonSemaphoreHandle == NULL)
    {
        // Abort
        System_abort("Semaphore creation failed");
    }
    // Initialize erro
    Error_init(&eb);
    // Call board init functions 
    Board_initGeneral();
    // init GPIO functions    
    Board_initGPIO();
    // Initialize Ethernet driver
    Board_initEMAC();
    // Initialize the WiFi driver
    Board_initWiFi();
    // initialize timer
    RTOSTimerInit();
    // initialize DMA
    Board_initDMA();
    // Initialize Buttons
    ButtonInit();
    // Set-up RTC
    RTCSetup();
    //Set Default time
    RTCDateTimeDefaultSet();
    Board_initUSB(Board_USBDEVICE);
    // Init USBCDC function
    USBCDCD_init();
    // init DMA
    //Board_initDMA();  
    
    //create an Event object. All events are binary */
    evtHandle = Event_create(NULL, &eb);
    if (evtHandle == NULL) {
        System_abort("Event create failed");
    }
    
    
    Task_Params_init(&taskParams);
          
    // 10-Construct Task Webserver Task threads    
    taskParams.stackSize = 1024;
    taskParams.priority = DEFAULT_TASK_PRIORITY;
    taskInitialization = Task_create((Task_FuncPtr)TaskInitialization, &taskParams, &eb);
    if (taskInitialization == NULL)
    {
        System_abort("Task create failed");
    }
    
    // 1- Construct Task Webserver Task threads
    Task_Params_init(&taskParams);
    taskParams.stackSize = DEFAULT_TASKSTACKSIZE;
    taskParams.priority = DEFAULT_TASK_PRIORITY;
    taskWebserver = Task_create((Task_FuncPtr)TaskWebserver, &taskParams, &eb);
    if (taskWebserver == NULL)
    {
        System_abort("Task create failed");
    }
    // 2- Construct Task Webserver Task threads
    taskParams.stackSize = DEFAULT_TASKSTACKSIZE;
    taskParams.priority = DEFAULT_TASK_PRIORITY;
    taskBLE = Task_create((Task_FuncPtr)TaskBLE, &taskParams, &eb);
    if (taskBLE == NULL)
    {
        System_abort("Task create failed");
    }
    // 3-Construct TaskShell Task threads
    taskParams.stackSize = DEFAULT_TASKSTACKSIZE;
    taskParams.priority = DEFAULT_TASK_PRIORITY;
    taskShell = Task_create((Task_FuncPtr)TaskShell, &taskParams, &eb);
    if (taskShell == NULL)
    {
        System_abort("Task create failed");
    }
    // 4-Construct TaskStatus Task threads
    taskParams.stackSize = TEST_TASK_SIZE;
    taskParams.priority = DEFAULT_TASK_PRIORITY;
    taskStatus = Task_create((Task_FuncPtr)TaskStatus, &taskParams, &eb);
    if (taskStatus == NULL)
    {
        System_abort("Task create failed");
    }    
    // 5-Construct Task NFC Task threads
    
    taskParams.stackSize = DEFAULT_TASKSTACKSIZE;
    taskParams.priority = DEFAULT_TASK_PRIORITY;
    taskNFC = Task_create((Task_FuncPtr)TaskNFC, &taskParams, &eb);
    if (taskNFC == NULL)
    {
        System_abort("Task create failed");
    }  
    // 6-Construct TaskEventLog Task threads
    
    taskParams.stackSize = DEFAULT_TASKSTACKSIZE;
    taskParams.priority = DEFAULT_TASK_PRIORITY;
    taskEventLog = Task_create((Task_FuncPtr)TaskEventLog, &taskParams, &eb);
    if (taskEventLog == NULL)
    {
        System_abort("Task create failed");
    }
    // 7-Construct TaskTCPClient Task threads
    taskParams.stackSize = DEFAULT_TASKSTACKSIZE;
    taskParams.priority = DEFAULT_TASK_PRIORITY;
    taskTCPClient = Task_create((Task_FuncPtr)TaskTCPClient, &taskParams, &eb);
    if (taskTCPClient == NULL)
    {
        System_abort("Task create failed");
    }
    // 8-Construct Task Shell  Task threads
    taskParams.stackSize = DEFAULT_TASKSTACKSIZE;
    taskParams.priority = DEFAULT_TASK_PRIORITY;
    taskWhisper = Task_create((Task_FuncPtr)TaskWhisper, &taskParams, &eb);
    if (taskWhisper == NULL)
    {
        System_abort("Task create failed");
    }
    // 9-Construct TaskParsing  Task threads
    
    taskParams.stackSize = DEFAULT_TASKSTACKSIZE;
    taskParams.priority = DEFAULT_TASK_PRIORITY;
    taskParsing = Task_create((Task_FuncPtr)TaskParsing, &taskParams, &eb);
    if (taskParsing == NULL)
    {
        System_abort("Task create failed");
    }
    
    // 11-Construct TaskBattery  Task threads
    taskParams.stackSize = DEFAULT_TASKSTACKSIZE;
    taskParams.priority = DEFAULT_TASK_PRIORITY;
    taskBattery = Task_create((Task_FuncPtr)TaskBattery, &taskParams, &eb);
    if (taskBattery == NULL)
    {
        System_abort("Task create failed");
    }
    
     // 12-Construct TaskUSBDataRecieve  Task threads
    taskParams.stackSize = DEFAULT_TASKSTACKSIZE;
    taskParams.priority = DEFAULT_TASK_PRIORITY;
    taskUSBDataRecieve = Task_create((Task_FuncPtr)TaskUSBDataRecieve, &taskParams, &eb);
    if (taskUSBDataRecieve == NULL)
    {
        System_abort("Task create failed");
    }
    
    // Set the device mode to peeking
    isDeviceInPeeking = true;
    
    TcpEchoInit();
    
    BIOS_start();    /* Does not return */
    return(0);
}

//==============================================================================
//  End Of File
//==============================================================================