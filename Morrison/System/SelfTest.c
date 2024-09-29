//==============================================================================
// Start of file
//==============================================================================
//
//  SelfTest.c
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
//  Source:        SelfTest.c
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
//! This module contains functions that handle the instrument self-test  during
//! power-on
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
#include "Selftest.h"
#include <xdc/runtime/System.h>
#include "LEDs.h"
#include "Dataflash.h"
//
//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================

typedef struct
{
    unsigned short  descriptorId;
    unsigned long   appSize;
    unsigned long   appNotSize;
    unsigned short  appCrc;
    unsigned short  appNotCrc;
    void (*AppStart)(void);
    
} APPLICATION_DESCRIPTOR_REC_SRTUCT;

#define APPLICATION_DESCRIPTOR_ADDRESS  0x00000000      //!< !Todo Add application descriptor address
#define APPLICATION_DESCRIPTOR_ID       0xBADA          //!< !Todo Add application descriptor ID

const unsigned int crcTable[512] = {                                                                 //!< CRC table
    0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 
    0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x0u,  0xC1u, 0x81u, 0x40u, 
    0x1u,  0xC0u, 0x80u, 0x41u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x0u,  0xC1u,
    0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u,
    0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u,
    0x81u, 0x40u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u,
    0x1u,  0xC0u, 0x80u, 0x41u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x0u,  0xC1u,
    0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u,
    0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u,
    0x80u, 0x41u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x0u,  0xC1u, 0x81u, 0x40u,
    0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x1u,  0xC0u,
    0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u,
    0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u,
    0x81u, 0x40u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u,
    0x1u,  0xC0u, 0x80u, 0x41u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u,
    0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u,
    0x1u,  0xC0u, 0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u,
    0x80u, 0x41u, 0x0u,  0xC1u, 0x81u, 0x40u, 0x1u,  0xC0u, 0x80u, 0x41u, 0x1u,  0xC0u, 0x80u, 0x41u,
    0x0u,  0xC1u, 0x81u, 0x40u, 0x0u,  0xC0u, 0xC1u, 0x1u,  0xC3u, 0x3u,  0x2u,  0xC2u, 0xC6u, 0x6u,
    0x7u,  0xC7u, 0x5u,  0xC5u, 0xC4u, 0x4u,  0xCCu, 0xCu,  0xDu,  0xCDu, 0xFu,  0xCFu, 0xCEu, 0xEu,
    0xAu,  0xCAu, 0xCBu, 0xBu,  0xC9u, 0x9u,  0x8u,  0xC8u, 0xD8u, 0x18u, 0x19u, 0xD9u, 0x1Bu, 0xDBu,
    0xDAu, 0x1Au, 0x1Eu, 0xDEu, 0xDFu, 0x1Fu, 0xDDu, 0x1Du, 0x1Cu, 0xDCu, 0x14u, 0xD4u, 0xD5u, 0x15u, 
    0xD7u, 0x17u, 0x16u, 0xD6u, 0xD2u, 0x12u, 0x13u, 0xD3u, 0x11u, 0xD1u, 0xD0u, 0x10u, 0xF0u, 0x30u, 
    0x31u, 0xF1u, 0x33u, 0xF3u, 0xF2u, 0x32u, 0x36u, 0xF6u, 0xF7u, 0x37u, 0xF5u, 0x35u, 0x34u, 0xF4u, 
    0x3Cu, 0xFCu, 0xFDu, 0x3Du, 0xFFu, 0x3Fu, 0x3Eu, 0xFEu, 0xFAu, 0x3Au, 0x3Bu, 0xFBu, 0x39u, 0xF9u, 
    0xF8u, 0x38u, 0x28u, 0xE8u, 0xE9u, 0x29u, 0xEBu, 0x2Bu, 0x2Au, 0xEAu, 0xEEu, 0x2Eu, 0x2Fu, 0xEFu, 
    0x2Du, 0xEDu, 0xECu, 0x2Cu, 0xE4u, 0x24u, 0x25u, 0xE5u, 0x27u, 0xE7u, 0xE6u, 0x26u, 0x22u, 0xE2u, 
    0xE3u, 0x23u, 0xE1u, 0x21u, 0x20u, 0xE0u, 0xA0u, 0x60u, 0x61u, 0xA1u, 0x63u, 0xA3u, 0xA2u, 0x62u, 
    0x66u, 0xA6u, 0xA7u, 0x67u, 0xA5u, 0x65u, 0x64u, 0xA4u, 0x6Cu, 0xACu, 0xADu, 0x6Du, 0xAFu, 0x6Fu, 
    0x6Eu, 0xAEu, 0xAAu, 0x6Au, 0x6Bu, 0xABu, 0x69u, 0xA9u, 0xA8u, 0x68u, 0x78u, 0xB8u, 0xB9u, 0x79u, 
    0xBBu, 0x7Bu, 0x7Au, 0xBAu, 0xBEu, 0x7Eu, 0x7Fu, 0xBFu, 0x7Du, 0xBDu, 0xBCu, 0x7Cu, 0xB4u, 0x74u, 
    0x75u, 0xB5u, 0x77u, 0xB7u, 0xB6u, 0x76u, 0x72u, 0xB2u, 0xB3u, 0x73u, 0xB1u, 0x71u, 0x70u, 0xB0u, 
    0x50u, 0x90u, 0x91u, 0x51u, 0x93u, 0x53u, 0x52u, 0x92u, 0x96u, 0x56u, 0x57u, 0x97u, 0x55u, 0x95u, 
    0x94u, 0x54u, 0x9Cu, 0x5Cu, 0x5Du, 0x9Du, 0x5Fu, 0x9Fu, 0x9Eu, 0x5Eu, 0x5Au, 0x9Au, 0x9Bu, 0x5Bu, 
    0x99u, 0x59u, 0x58u, 0x98u, 0x88u, 0x48u, 0x49u, 0x89u, 0x4Bu, 0x8Bu, 0x8Au, 0x4Au, 0x4Eu, 0x8Eu, 
    0x8Fu, 0x4Fu, 0x8Du, 0x4Du, 0x4Cu, 0x8Cu, 0x44u, 0x84u, 0x85u, 0x45u, 0x87u, 0x47u, 0x46u, 0x86u, 
    0x82u, 0x42u, 0x43u, 0x83u, 0x41u, 0x81u, 0x80u, 0x40u
};

// Self-Test results' masks
#define DATAFLASH_DIAG_RESULT_MASK      0x0001
#define BATTERY_DIAG_RESULT_MASK        0x0002
#define FLASH_CRC_DIAG_RESULT_MASK      0x0004
#define INSTRUM_PARAM_DIAG_RESULT_MASK  0x0008
#define LENS_DIAG_RESULT_MASK           0x0010
#define ETH_HARD_DIAG_RESULT_MASK       0x0020
#define MODEM_HARED_DIAG_RESULT_MASK    0x0040
#define WIFI_HARD_DIAG_RESULT_MASK      0x0080
#define CORE_CONN_DIAG_RESULT_MASK      0x0100
#define NFC_HARD_DIAG_RESULT_MASK       0x0200
#define BTLE_HARD_DIAG_RESULT_MASK      0x0400
#define GPS_HARD_DIAG_RESULT_MASK       0x0800



//==============================================================================
// LOCAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
// LOCAL DATA DEFINITIONS
//==============================================================================
static unsigned short selfTestResult = 0u;
//==============================================================================
// GLOBAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
// GLOBAL DATA DEFINITIONS
//==============================================================================

//==============================================================================
// LOCAL FUNCTIONS PROTOTYPES
//==============================================================================
//
static bool DiagnosticsDataflash(void);
static bool DiagnosticsBattery(void);
static bool DiagnosticsFlashCRC(void);
static bool DiagnosticsInstrumentParameters(void);
static bool DiagnosticsLENCommunication(void);
static bool DiagnosticsCoreConnectivityPeripherals(void);
static bool DiagnosticsNetworkConnectivity(void);
static bool DiagnosticsNFC(void);
static bool DiagnosticsBTLE(void);
static bool DiagnosticsTime(void);
static bool DiagnosticsGPS(void);
//
//==============================================================================
// LOCAL FUNCTIONS IMPLEMENTATIONS
//==============================================================================

//==============================================================================
//
//  DiagnosticsDataflash(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function handles the daignostics of dataflash 

//
//=============================================================================


static bool DiagnosticsDataflash(void)
{
    // Variable to save dataflash diagnositcs status
    bool dataflashDiagnosticsStatus = false;
    // Variable to save installed dataflash type
    unsigned char installedDataflashType = 0u;
    
    // Read the installed dataflash type ID
    installedDataflashType = GetDataflashID();
    // If any of the supported dataflash types is installed
    if(installedDataflashType != 0u)
    {
        // Set the dataflash diagnostics result to true
        dataflashDiagnosticsStatus = true;
         // Save self-test result
        selfTestResult|= 1u;
    }
    // Return dataflash diagnostics status
    //todo_fehan forcefully validated dataflash 
    // return dataflashDiagnosticsStatus;
     return true;
}

//==============================================================================
//
//  DiagnosticsBattery(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function handles the daignostics of instrument battery.
//
//=============================================================================

static bool DiagnosticsBattery(void)
{
    // Variable to sotre battery diagnostics status
    bool batteryDiagnosticsStatus = false;
    // Variable to store the battery type validation status
    bool isBatteryTypeValid = false;
    // Variable to save battery voltage status
    bool isBatteryVoltageValid = false;
    
    //!Todo Vealidate installed battery type
    isBatteryTypeValid = true;        
    // If valid battery type is installed
    if(isBatteryTypeValid == true)
    {
        //!Todo Validate battery voltage and charging status
        isBatteryVoltageValid = true;
        
        // If battery voltage is below critical value and it is not being charged
        if(isBatteryVoltageValid == false)
        {
            // Log critical battery voltage error
            ErrorLogWrite(ERRORCODE_ENUM_MICRO_VDD_ERROR,ERRORTYPE_ENUM_CRITICAL);            
        }        
    }
    // If illegal battery type is installed
    else
    {
        // Log illegal battery type error
        ErrorLogWrite(ERRORCODE_ENUM_ILLEGAL_BATTERY_TYPE,ERRORTYPE_ENUM_CRITICAL);        
    }
    // If valid battery type is installed and battery voltage is valid
    if((isBatteryTypeValid == true) && (isBatteryVoltageValid == true))
    {
        // Set the diagnostics status to true
        batteryDiagnosticsStatus = true;
        // Save self-test result
        selfTestResult|= (1u << 1u);
    }
    // Return battery diagnostics status
    return batteryDiagnosticsStatus;
}

//==============================================================================
//
//  DiagnosticsFlashCRC(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function handles the daignostics of application code CRC.
//
//=============================================================================

static bool DiagnosticsFlashCRC(void)
{
    // Checks for a valid application descriptor. 
    // This variable is used to check valid descriptor
    bool  validDescriptor = true;
    // This variable is used for application descriptor REC
    APPLICATION_DESCRIPTOR_REC_SRTUCT *pDescriptor;
    // This variable is used to calculate checksum
    unsigned short checksum;
    // This variable is used to calculate checksum
    unsigned short notNotCrc;
    // This variable is used for application checksum
    unsigned short applicationCRC;
    
    // Application Descriptor Address in flash
    pDescriptor = (APPLICATION_DESCRIPTOR_REC_SRTUCT *)APPLICATION_DESCRIPTOR_ADDRESS ;
    // Complement of application crc
    notNotCrc = ~( pDescriptor->appNotCrc  ) ;
    
    // First, make sure the descriptor ID says "0xBADA" - it always should.
    if ( pDescriptor->descriptorId != APPLICATION_DESCRIPTOR_ID )
    {
        validDescriptor = false;
    }
    // Next, make sure the application size and the complement of the application
    // size in the descriptor match up with each other.
    else if (pDescriptor->appSize != ~( pDescriptor->appNotSize ) )
    {
        validDescriptor = false;
    }
    // Next, make sure the application CRC and the complement of the application
    // CRC in the descriptor match up with each other.
    else if ( pDescriptor->appCrc != notNotCrc )
    {
        validDescriptor = false;
    }
    else
    {
        validDescriptor = true;
    }
    
    // If all of the above check out OK, we can go on to the most important check - 
    // perform the CRC of the entire application, and make sure it matches the stored CRC.  
    if ( ( int )validDescriptor == true )
    {
        // Read the Stored CRC value.
        applicationCRC = pDescriptor->appCrc;
        checksum = ( unsigned short )CalculateFlashCrc( ( unsigned char * )pDescriptor->AppStart,\
            ( unsigned int  ) pDescriptor->appSize );
        if (checksum != applicationCRC )
        {
            validDescriptor = false;
        }
    }
    // Check if valid decscriptor is not found
    if ( ( int )validDescriptor == false )
    {
        // Throw a critical error
        ErrorLogWrite(ERRORCODE_ENUM_FLASH_CRC_FAILURE,ERRORTYPE_ENUM_CRITICAL);
    }
    else
    {
    }
    
    //! Todo Set descriptor to true for now
    validDescriptor = true;
    
    // If descriptor is valid
    if(validDescriptor == true)
    {
        // Save self-test result
        selfTestResult|= (1u << 2u);
    }
    
    // Return flash CRC diagnostics results
    return validDescriptor;
}

//==============================================================================
//
//  DiagnosticsInstrumentParameters(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function handles the daignostics of instrument parameters CRC.
//
//=============================================================================

static bool DiagnosticsInstrumentParameters(void)
{
    // Variable to save the status of Instrument Parameters CRC validation
    bool instrumetParametersValid = false;
    
    // !Todo Instrument parameters CRC check goes here
    instrumetParametersValid = true;
    // If instrument parameters CRC is invalid
    if(instrumetParametersValid == false)
    {
        // Log the error
        ErrorLogWrite(ERRORCODE_ENUM_INSTRUMENT_PARAM_CHECKSUM_ERROR,ERRORTYPE_ENUM_WARNING);
        // !Todo Apply default instrument parameters
    }
    else
    {
        // Save self-test result
        selfTestResult|= (1u << 3u);
    }
    // Return instrument parameters validation status
    return instrumetParametersValid;
}

//==============================================================================
//
//  DiagnosticsLENCommunication(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function handles the daignostics of LENS communication.
//
//=============================================================================

static bool DiagnosticsLENCommunication(void)
{
    // Variable to save the status of LENS hardware validation
    bool lensDiagnosticsStatus = false;
    
    // !Todo LENS diagnostics i.e. OS Version verification,
    // script version verification goes here
    lensDiagnosticsStatus = true;    
    // If LENS validation failed
    if(lensDiagnosticsStatus == false)
    {
        // Log the error
        ErrorLogWrite(ERRORCODE_ENUM_WHISPER_DEVICE_ERROR,ERRORTYPE_ENUM_WARNING);
        // ! Todo Disable whisper related functions
    }
    else
    {
        // Save self-test result
        selfTestResult|= (1u << 4u);
    }
    // Return LENS self-test status
    return lensDiagnosticsStatus;
    
}

//==============================================================================
//
//  DiagnosticsCoreConnectivityPeripherals(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/17/12
//
//! This function handles the daignostics of core network connectivity peripherals
//
//=============================================================================

static bool DiagnosticsCoreConnectivityPeripherals(void)
{
    // Variable to save the status of Ethernet diagnostics
    bool ethernetDiagnosticsStatus = false;
    // Variable to save the status of Cellular modem diagnostics
    bool cellularModemDiagnosticsStatus = false;
    // Variable to save the status of Wi-Fi diagnostics
    bool wifiModuleDiagnosticsStatus = false;
    // Variable to store the status of core connectivity peripherals status
    bool coreConnPeriphDiagnosticsStatus = false;
    
    //!Todo Ethernet hardware diagnostics function goes here
    ethernetDiagnosticsStatus = true;
    // If Ethernet hardware diagnostics failed
    if(ethernetDiagnosticsStatus == false)
    {
        // !Todo Log error for Ethernet hardware diagnostics failure
        // ErrorLogWrite(ERRORCODE_ENUM_ETHERNET_COMMUNICATION_ERROR,ERRORTYPE_ENUM_WARNING);
    }
    else
    {
        // Save self-test result
        selfTestResult|= (1u << 5u);
    }
    //!Todo Cellular modem diagnostics function goes here
    cellularModemDiagnosticsStatus = true;
    // If Cellular modem hardware diagnostics failed
    if(cellularModemDiagnosticsStatus == false)
    {
        // !Todo Log error for Cellular modem hardware diagnostics failure
        // ErrorLogWrite(ERRORCODE_ENUM_CELLULAR_COMMUNICATION_ERROR,ERRORTYPE_ENUM_WARNING);
    }
    else
    {
        // Save self-test result
        selfTestResult|= (1u << 6u);
    }
    //!Todo Wi-Fi module hardware diagnostics function goes here
    wifiModuleDiagnosticsStatus = true;
    // If Wi-Fi module hardware diagnostics failed
    if(wifiModuleDiagnosticsStatus == false)
    {
        // !Todo Log error for Wi-Fi module hardware diagnostics failure
        // ErrorLogWrite(ERRORCODE_ENUM_WIFI_COMMUNICATION_ERROR,ERRORTYPE_ENUM_WARNING);
    }
    else
    {
        // Save self-test result
        selfTestResult|= (1u << 7u);
    }    
    // Check if all of the core network connectivity peripherals diagnostics failed and 
    // LENS self-test also failed
    if( ((ethernetDiagnosticsStatus == false) && (cellularModemDiagnosticsStatus == false) && \
         (wifiModuleDiagnosticsStatus == false)) && (!(selfTestResult & LENS_DIAG_RESULT_MASK)) )
    {
        // !Todo Log error for core connectivity peripherals diagnostics failure
        // ErrorLogWrite(ERRORCODE_ENUM_NETWORK_COMMUNICATION_ERROR,ERRORTYPE_ENUM_CRITICAL);
    }
    else
    {
        // Set the status of core connectivity peripherals' diagnostics to true
        // if any of the peripherals passed the self-test and can be used for
        // core server connectivity
        coreConnPeriphDiagnosticsStatus = true;
    }
    
    // Return the status of core connectivity peripherals' diagnostics
    return coreConnPeriphDiagnosticsStatus;
}

//==============================================================================
//
//  DiagnosticsNetworkConnectivity(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function handles the daignostics of network connectivity.
//
//=============================================================================

static bool DiagnosticsNetworkConnectivity(void)
{
    // Variable to store the status of core connectivity status
    bool coreConnectivityStatus = false;
    
    // !Todo Establish core network connection via available communication medium
    coreConnectivityStatus = true;
    // If connection was unsuccessful
    if(coreConnectivityStatus ==false)
    {
        // If LENS self-test has already failed
        if(!(selfTestResult & LENS_DIAG_RESULT_MASK))
        {
            //!Todo Log it as critical error because without LENS and core connectivity,
            // the instrument cannot perform any task
            ErrorLogWrite(ERRORCODE_ENUM_NETWORK_COMMUNICATION_ERROR,ERRORTYPE_ENUM_CRITICAL);
        }
        else if(selfTestResult & LENS_DIAG_RESULT_MASK)
        {
            //!Todo Log it as warning
            //ErrorLogWrite(ERRORCODE_ENUM_NETWORK_COMMUNICATION_ERROR,ERRORTYPE_ENUM_WARNING);
        }        
    }
    else
    {
        // Save self-test result
        selfTestResult|= (1u << 8u);
    }
    
    // Return core network connectivity status
    return coreConnectivityStatus;
}

//==============================================================================
//
//  DiagnosticsNFC(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function handles the daignostics of NFC.
//
//=============================================================================

static bool DiagnosticsNFC(void)
{
    // Variable to store the status NFC diagnostics
    bool nfcDiagnosticsStatus= false;
    
    //!Todo Perform NFC diagnostics
    nfcDiagnosticsStatus = true;
    // If NFC diagnostics was not successful
    if(nfcDiagnosticsStatus == false)
    {
        // Log NFC error
        ErrorLogWrite(ERRORCODE_ENUM_NFC_COMMUNICATION_ERROR,ERRORTYPE_ENUM_WARNING);
        // !Todo disable NFC functions
    }
    else
    {
        // Save self-test result
        selfTestResult|= (1u << 9u);
    }
    // Retunrn NFC diagnostics status
    return nfcDiagnosticsStatus;    
}

//==============================================================================
//
//  DiagnosticsBTLE(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function handles the daignostics of BTLE
//
//=============================================================================

static bool DiagnosticsBTLE(void)
{
    // Variable to store the status BTLE diagnostics
    bool btleDiagnosticsStatus= false;
    
    //!Todo Perform BTLE diagnostics
    btleDiagnosticsStatus = true;
    // If BTLE diagnostics was not successful
    if(btleDiagnosticsStatus == false)
    {
        // Log BTLE error
        ErrorLogWrite(ERRORCODE_ENUM_BTLE_COMMUNICATION_ERROR,ERRORTYPE_ENUM_WARNING);
        // !Todo disable BTLE functions
    }
    else
    {
        // Save self-test result
        selfTestResult|= (1u << 10u);
    }
    // Retunrn NFC diagnostics status
    return btleDiagnosticsStatus;    
}

//==============================================================================
//
//  DiagnosticsGPS(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function handles the daignostics of GPS
//
//=============================================================================

static bool DiagnosticsGPS(void)
{
    // Variable to store the status GPS diagnostics
    bool gpsDiagnosticsStatus= false;
    
    //!Todo Perform GPS diagnostics
    gpsDiagnosticsStatus = true;
    // If GPS diagnostics was not successful
    if(gpsDiagnosticsStatus == false)
    {
        // Log GPS error
        // ErrorLogWrite(ERRORCODE_ENUM_BTLE_COMMUNICATION_ERROR,ERRORTYPE_ENUM_WARNING);        
    }
    else
    {
        //! Todo update location
        // Save self-test result
        selfTestResult|= (1u << 11u);
    }
    // Retunrn GPS diagnostics status
    return gpsDiagnosticsStatus;
}

//==============================================================================
//
//  DiagnosticsTime(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function handles the daignostics of instrument time
//
//=============================================================================

static bool DiagnosticsTime(void)
{
    // Variable to store the status of time update
    bool timeUpdateStatus= false;
    
    //!Todo update time
    timeUpdateStatus = true;
    // If time update was not successful
    if(timeUpdateStatus == false)
    {
        // Log time update error
        // ErrorLogWrite(ERRORCODE_ENUM_BTLE_COMMUNICATION_ERROR,ERRORTYPE_ENUM_WARNING);        
    }
    // Retunrn time update status
    return timeUpdateStatus;    
}

//==============================================================================
// GLOBAL  FUNCTIONS IMPLEMENTATAIONS
//==============================================================================

//==============================================================================
//
//  SelfTestHandler(SELFTEST_STEP_ENUM selfTestStep)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/30/11
//
//! This function handles the self-test of peripherals during power-on and on
//! user command
//
//==============================================================================

bool SelfTestHandler(SELFTEST_STEP_ENUM selfTestStep)
{  
    // Variable to save the status of peripheral diagnostics
    bool selfTestStatus = false;
    
    switch(selfTestStep)
    {
        // Dataflash diagnostics
    case SELFTEST_STEP_DATAFLASH:
        selfTestStatus = DiagnosticsDataflash();
        break;        
        
    case SELFTEST_STEP_BATTERY:        
        // Battery diagnostics
        selfTestStatus = DiagnosticsBattery();;
        break;
        
    case SELFTEST_STEP_FLASHCRC:
        // Application code CRC diagnostics
        selfTestStatus = DiagnosticsFlashCRC();;
        break;
        
    case SELFTEST_STEP_INSTRUMENT_PARAM:
        // Instrument Parameters diagnostics
        selfTestStatus = DiagnosticsInstrumentParameters();
        // !Todo Instrument parameters diagnostics does not cause system fault
        selfTestStatus = true;
        
        break;
                
    case SELFTEST_STEP_LENS:
        // LENS communication diagnostics        
        selfTestStatus = DiagnosticsLENCommunication();        
        // Todo LENS diagnostics alone cannot cause system fault
        selfTestStatus = true;
        break;
        
    case SELFTEST_STEP_CORE_CONN_PERIPH:
        // Core network connectivity peripherals diagnostics
        selfTestStatus = DiagnosticsCoreConnectivityPeripherals();
        break;
        
    case SELFTEST_STEP_CORE_CONN:
        // Core network connectivity test
        selfTestStatus = DiagnosticsNetworkConnectivity();
        break;
        
    case SELFTEST_STEP_NFC_HARD:
        // NFC diagnostics
        selfTestStatus = DiagnosticsNFC();
        // !Todo NFC diagnostics cannot cause system fault
        selfTestStatus = true;
        break;
        
    case SELFTEST_STEP_BTLE_HARD:
        // BTLE diagnostics
        selfTestStatus = DiagnosticsBTLE();
        // !Todo NFC diagnostics cannot cause system fault
        selfTestStatus = true;
        break;
        
    case SELFTEST_STEP_GPS_HARD:
        // GPS diagnostics
        selfTestStatus = DiagnosticsGPS();
        // !Todo GPS diagnostics cannot cause system fault
        selfTestStatus = true;
        break;
        
    case SELFTEST_STEP_TIME_UPDATE:
        // Time update
        selfTestStatus = DiagnosticsTime();
        // !Todo GPS diagnostics cannot cause system fault
        selfTestStatus = true;
        
        break;
        
    default:
        break;        
    }
    
    // Return the peripheral diagnostics status
    return selfTestStatus;
    
}

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
                                )
{
    // If LED needs to be activated
    if(isIndicatorActivated == true)
    {
        // Activate LED
        LEDOn(ledNumber);
    }
    // Otherwise
    else
    {
        // Activate LED
        LEDOff(ledNumber);
    }
    
}

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

void DiagnosticsLEDsAllOff(void)
{
    // Turn off all LEDs
    LEDOff(LED_NUMBER_2);
    LEDOff(LED_NUMBER_3);
    LEDOff(LED_NUMBER_4);
    LEDOff(LED_NUMBER_5);
    LEDOff(LED_NUMBER_6);    
}

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
                                   )
{
    // This variable is used for index of for loop
    unsigned int loopIndex = 0u;
    // This variable is used for CRC table index
    unsigned int index = 0u;
    // This variable is used for low byte of CRC
    unsigned int crcLow = 0xFFU;
    // This variable is used for high byte of CRC
    unsigned int crcHigh = 0xFFU;
    
    // Calculate CRC
    for ( loopIndex = 0U; loopIndex < len; loopIndex++ )
    {
        index = crcHigh ^ ( *( src + loopIndex ) );
        crcHigh = crcLow ^ crcTable[index];
        crcLow = ( unsigned char )crcTable[index+ 256U];
    }
    
    return (crcLow * 256U + crcHigh);
}

//==============================================================================
//  End Of File
//==============================================================================