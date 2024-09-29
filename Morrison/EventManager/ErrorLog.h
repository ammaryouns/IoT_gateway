//==============================================================================
//
//  ErrorLog.h
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
//  Source:        ErrorLog.h
//
//  Project:       Morrison
//
//  Author:        Ali Zulqarnain Anjum
//
//  Date:          2016/12/01
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This file declares the global functions and constant of the module
//! these functions are used to read and write the errors to the flash memory.
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//  Revision: 1.0  2016/12/01  Ali Zulqarnain Anjum
//      This module is taken from Vaughan. Initially, the error codes are given
//      temporary values.
//
//==============================================================================

#ifndef __ERRORLOG_H__
#define __ERRORLOG_H__

//==============================================================================
//  INCLUDES
//==============================================================================

#include <stdbool.h>
//==============================================================================
//  GLOBAL CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

#define ERRLOG_CODE_OFFSET                                 0x0001u          //!< Offset for error code
#define ERRLOG_TIME1_OFFSET                                0x0002u          //!< Offset for month and date
#define ERRLOG_TIME2_OFFSET                                0x0003u          //!< Offset for year
#define ERRLOG_TIME3_OFFSET                                0x0004u          //!< Offset for hours and minutes
#define ERRLOG_TIME4_OFFSET                                0x0005u          //!< Offset for seconds
#define ERRLOG_IDENTIFIER_OFFSET      0x0000U                                   //!< Offset for error identifier
#define WORDS_PER_ERRLOG              6U                                        //!< Code for words per error log
#define INVALID_ENTRY_NUMBER          255U                                      //!< Code for invalid entry number
#define INVALID_ERROR_IDENTIFIER      0xFFFFU                                   //!< Code for invalid error identifier
#define MAX_VALID_ERROR_IDENTIFIER    9999U                                     //!< Code for maximum error identifier
#define FIRST_DATAFLASH_ERROR         500U                                      //!< Offset for first location of error log in dataflash
#define LAST_DATAFLASH_ERROR          599U                                      //!< Offset for last location of error log in dataflash
#define ERRLOG_ENTRIES_PER_PAGE       20U                                       //!< Code for entries per page
#define PAGES_PER_ERROR_LOG           2U                                                 //!< Define for number of pages in Event Log
#define TOTAL_ERROR_ENTRIES           ((PAGES_PER_ERROR_LOG) * ERRLOG_ENTRIES_PER_PAGE ) //!< Total number of event log entries in all Event Log pages
#define ERRORLOG_CLEAR_REQUEST        4u                                                 //!< Identifier for Error log clear request 
#define TOTAL_NUMBER_OF_ERRORS        9u                                        //!< Total Number of Errors for logging
#define WHISPER_DEVICE_ERROR_SUBSECTOR      1u                                  //!< Subsector for whisper device error
#define ETHERNET_COMMUNICATION_ERROR_SUBSECTOR      2u                          //!< Subsector for ethernet device error
#define WIFI_COMMUNICATION_ERROR_SUBSECTOR     3u                               //!< Subsector for wifi device error
#define CELLULAR_COMMUNICATION_ERROR_SUBSECTOR     4u                           //!< Subsector for cellular device error
#define DATAFLASH_READ_WRITE_ERROR_SUBSECTOR     5u                             //!< Subsector for DATAFLASH device error
#define NETWORK_COMMUNICATION_ERROR_SUBSECTOR      6u                           //!< Subsector for network device error
#define NFC_COMMUNICATION_ERROR_SUBSECTOR      7u                               //!< Subsector for NFC device error
#define BTLE_COMMUNICATION_ERROR_SUBSECTOR      8u                              //!< Subsector for BTLE device error
#define LEGACY_ERRORS_SUBSECTOR      9u                                         //!< Subsector for legacy device errors
#define BACKUP_SUBSECTOR_OFFSET       TOTAL_NUMBER_OF_ERRORS                    //!< Offset for the back-up subsector

//==============================================================================
//  GLOBAL DATA STRUCTURES DEFINITION
//==============================================================================

//! Error Types may be Wanning or Critical
typedef enum
{
    ERRORTYPE_ENUM_INVALID                                 = 0x00,          //!< Code for invalid error type
    ERRORTYPE_ENUM_WARNING                                 = 0x01,          //!< Code for warning type error
    ERRORTYPE_ENUM_CRITICAL                                = 0x02,          //!< Code for critical type error
    
} ERRORTYPE_ENUM; 

//! Predefine error code table (TODO: Get the exact error codes)
typedef enum 
{
    ERRORCODE_ENUM_NO_ERROR                                 = 0u,               //!< No error code
     // Normal instrument errors: 4xx
    ERRORCODE_ENUM_UNDEFINED_MODBUS_REGISTER                = 401U,          //!< Error code for an undefined modbus register condition
    ERRORCODE_ENUM_INSTRUMENT_PARAM_CHECKSUM_ERROR          = 404U,          //!< Error code for instrument parameters checksum invalid
    ERRORCODE_ENUM_I2C_LOCKUP_ERROR                         = 420U,          //!< Error code for I2C lock up error
    ERRORCODE_ENUM_SPI_ERROR                                = 451U,          //!< Error code if SPI error
    ERRORCODE_ENUM_ILLEGAL_BATTERY_TYPE                     = 470U,          //!< Error code if Alkaline battery is detected in VentisPro    
    ERRORCODE_ENUM_DEVICE_RESET_BY_EXT_WDT                  = 471u,          //!< Error code if device is reset by external WDT    
    // Dataflash errors: 5xx
    ERRORCODE_ENUM_DFLASH_TIMEOUT_OCCURRED                  = 501U,          //!< Error code if dataflash timeout occurred during read write operations
    ERRORCODE_ENUM_DFLASH_ERASE_FAILURE                     = 503U,          //!< Error code if dataflash erase operation fails    
    ERRORCODE_ENUM_DFLASH_BUF_COPY_FAIL                     = 504U,          //!< Error code if dataflash copy operation fails
    ERRORCODE_ENUM_DATALOG_POINTER_CORRUPT                  = 506U,          //!< Error code if dataflash point found in a corrupt condition
    ERRORCODE_ENUM_SECTOR_PROTECTION_ERROR                  = 507U,          //!< Error code if dataflash sector protection is failed
    ERRORCODE_ENUM_LAST_DATAFLASH_ERROR                     = 599U,          //!< Extra added for future extensibility
    // Selftest errors: 6xx    
    ERRORCODE_ENUM_RAM_FAIL_ALL_FF                          = 601u,          //!< Error code for self test of RAM failure (all FF)
    ERRORCODE_ENUM_RAM_FAIL_ALL_00                          = 602u,          //!< Error code for self test of RAM failure (all 00)
    ERRORCODE_ENUM_RAM_FAIL_ALL_AA                          = 603u,          //!< Error code for self test of RAM failure (all AA)
    ERRORCODE_ENUM_RAM_FAIL_ALL_55                          = 604u,          //!< Error code for self test of RAM failure (all 55)
    ERRORCODE_ENUM_FLASH_CRC_FAILURE                        = 606u,          //!< Error code for self test of flash CRC failure
    ERRORCODE_ENUM_WHISPER_SRAM_FAILURE                     = 610u,          //!< Error code for self test of Whisper SRAM
    ERRORCODE_ENUM_INVALID_BUTTON_PRESS                     = 618u,          //!< Error code for INVALID button press after five minutes are passed
    ERRORCODE_ENUM_LOW_BATTERY_WARNING                      = 692u,          //!< Error code for detection of low battery
    ERRORCODE_ENUM_BATTERY_BACKUP_EXPIRED                   = 691u,          //!< Error code for detection of expiration of battery backup 
    ERRORCODE_ENUM_SPI_PORT_NOT_DEFINED                     = 811u,
    ERRORCODE_ENUM_INSTRUMENT_PARAM_BACKUP_ERROR            = 850u,          //!< Error code for instrument parameters checksum invalid
    ERRORCODE_ENUM_MICRO_VDD_ERROR                          = 851u,          //!< Error code for detection of Micro VDD voltage abnormalities
    // todo - These codes are added temporary - and needs to be confirmed later on
    ERRORCODE_ENUM_WL_INCOMING_MB_CHECKSUM_ERROR            = 901u,
    ERRORCODE_ENUM_WL_INCOMING_MB_PARSING_ERROR             = 902u,
    ERRORCODE_ENUM_WL_INCOMING_MB_UNKNOWN_MSGS              = 903u,
    ERRORCODE_ENUM_WL_REGISTERS_NOT_POPULATED               = 904u, 
    ERRORCODE_ENUM_WL_REQUEST_GRANT_SCHEME_ERROR            = 905u, 
    ERRORCODE_ENUM_WL_POWER_OFF_GAS_MESSAGE_READ_ERROR      = 919u, 
    ERRORCODE_ENUM_WL_POWER_OFF_SLEEP_STATUS_ERROR          = 920u,
    ERRORCODE_ENUM_WL_SLEEP_TO_OFF_AIR_MODE_ERROR           = 921u,
    ERRORCODE_ENUM_WL_SCRIPT_UPLOAD_FAILED_ERROR            = 922u,
    ERRORCODE_ENUM_WL_OFF_AIR_TO_SLEEP_MODE_ERROR           = 923u,          //!< Wireless Off Air to Sleep Mode Error
    ERRORCODE_ENUM_BLE_NOT_RESPONDING_ERROR                 = 960u,          //!< BLE chip not responding
    ERRORCODE_ENUM_BLE_HARD_FAULT_ERROR                     = 961u,          //!< BLE chip is in error i.e. Hard Fault
    ERRORCODE_ENUM_BLE_FIRMWARE_UPLOAD_ERROR                = 962u,          //!< BLE Firmware uploaded failed
    //Morrison Errors
    ERRORCODE_ENUM_WHISPER_DEVICE_ERROR                     = 971U,               //!< Error code for local whisper device error
    ERRORCODE_ENUM_ETHERNET_COMMUNICATION_ERROR             = 972U,               //!< Error code for ethernet communication error
    ERRORCODE_ENUM_WIFI_COMMUNICATION_ERROR                 = 973U,               //!< Error code for wifi communication error
    ERRORCODE_ENUM_CELLULAR_COMMUNICATION_ERROR             = 974U,               //!< Error code for cellular communication error
    ERRORCODE_ENUM_DATAFLASH_READ_WRITE_ERROR               = 975U,               //!< Error code for data flash read write error (TODO: Different for dataflash read and write)
    ERRORCODE_ENUM_NETWORK_COMMUNICATION_ERROR              = 976U,               //!< Error code for device communication error
    ERRORCODE_ENUM_NFC_COMMUNICATION_ERROR                  = 977U,               //!< Error code for NFC communication error
    ERRORCODE_ENUM_BTLE_COMMUNICATION_ERROR                 = 978U,               //!< Error code for BLE communication error
} ERRORCODE_ENUM;

//==============================================================================
//  GLOBAL DATA
//==============================================================================



//==============================================================================
//  EXTERNAL OR GLOBAL FUNCTIONS
//==============================================================================

//------------------------------------------------------------------------------
//   ErrorLogWrite(ERRORCODE_ENUM errorCode, ERRORTYPE_ENUM errorType)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function writes a new error to error log entry
//
//------------------------------------------------------------------------------

void ErrorLogWrite(
                        ERRORCODE_ENUM errorCode,                               //!< Predefined constant error codes of a structure errorCodeENUM 
                        ERRORTYPE_ENUM errorType                                //!< Error type associated with error code
                   );

//------------------------------------------------------------------------------
//   ErrorLogRead(unsigned int *data, unsigned int dataOffset )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function reads the error code and time byte of the selected error form the log
//
//------------------------------------------------------------------------------

void ErrorLogRead(
                  unsigned short *data,             //!< This variable contains the data which is read
                  ERRORCODE_ENUM errorCode          //!< Error Code for download, 0 for all errorLog Download
                 );

//------------------------------------------------------------------------------
//   ErrorLogFindLatestIdentifier(unsigned int *highestIdentifier,ERRORCODE_ENUM errorCode)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function finds the latest error identifier in the error log
//
//------------------------------------------------------------------------------

void ErrorLogFindLatestIdentifier(
                                      unsigned short *highestIdentifier,         //!< This argument identifies the recent error 
                                      ERRORCODE_ENUM errorCode                      //!< Error Code Correponsing to entry index
                                  );

//------------------------------------------------------------------------------
//   ErrorLogSelectIdentifierForReadback(unsigned short errorLogNumber)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function selects the error identifier for read process and also clears the error log
//
//------------------------------------------------------------------------------

void ErrorLogSelectIdentifierForReadback(
                                            unsigned short errorLogNumber      //!< Error log number
                                        );

//------------------------------------------------------------------------------
//   ErrorLogClear(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function writes a new error to error log entry
//
//------------------------------------------------------------------------------

void ErrorLogClear(void);

//------------------------------------------------------------------------------
//   void ErrorLogInit(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function initializes the Errorlog parameters.
//
//------------------------------------------------------------------------------

void ErrorLogInit(void);

//------------------------------------------------------------------------------
//   void ErrorLogSetClearAcknowledge(bool value)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function sets the errorlog clearing variable.
//
//------------------------------------------------------------------------------

void ErrorLogSetClearAcknowledge( 
                                    bool value                                      //!< Value for the acknowledgement
                                );
//------------------------------------------------------------------------------
//   void ErrorLogSetLogDueWatchdog(bool value)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function sets the condition for logging the watchdog reset
//
//------------------------------------------------------------------------------

void ErrorLogSetLogDueWatchdog( 
                                bool value                                      //!< Value for the log due
                             );
//------------------------------------------------------------------------------
//   void ErrorLogSetCurrentErrorCode(ERRORCODE_ENUM  errorCode)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function sets the current error code
//
//------------------------------------------------------------------------------

void ErrorLogSetCurrentErrorCode( 
                                     ERRORCODE_ENUM  errorCode                  //!< Value for the current error code
                                 );
//------------------------------------------------------------------------------
//   bool ErrorLogGetClearAcknowledge(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function return the errorlog clearing variable.
//
//------------------------------------------------------------------------------

bool ErrorLogGetClearAcknowledge(void);

//------------------------------------------------------------------------------
//   bool ErrorLogGetLogDueWatchdog(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function returns the condition for logging the watchdog reset
//
//------------------------------------------------------------------------------

bool ErrorLogGetLogDueWatchdog(void);

//------------------------------------------------------------------------------
//   ERRORCODE_ENUM ErrorLogGetCurrentErrorCode(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function returns the current error code
//
//------------------------------------------------------------------------------

ERRORCODE_ENUM ErrorLogGetCurrentErrorCode(void);

#endif /* __ERRORLOG_H__ */
//==============================================================================
//  End Of File
//==============================================================================