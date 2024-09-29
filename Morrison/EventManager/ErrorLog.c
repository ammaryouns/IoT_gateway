//==============================================================================
//
//  ErrorLog.c
//
//  Copyright (C) 2016 by Industrial Scientific
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
//  Source:        ErrorLog.c
//
//  Project:       Morrison
//
//  Author:        Ali Zulqarnain Anjum
//
//  Date:          2016/12/06
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This module provides the functionality related to logging of error. It writes the 
//! error log entry to the flash memory it can also read the error log from the flash.
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//  Revision: 1.0  2016/12/06 Ali Zulqarnain Anjum
//      This module is taken from Vaughan. Initially, the error codes are given 
//      Temporary values.
//
//==============================================================================

//==============================================================================
//  INCLUDES 
//==============================================================================

#include "ErrorLog.h"
#include "Dataflash.h"
#include "TM4CRTC.h"

//==============================================================================
//  CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================


//==============================================================================
//  LOCAL DATA STRUCTURE DEFINITION
//==============================================================================
typedef struct
{
   unsigned char errorNumber;
   ERRORCODE_ENUM errorCode;
   unsigned char entryIndex;
   unsigned short errorIdentifier;
} ERROR_INFORMATION_STRUCT;
   
//==============================================================================
//  GLOBAL DATA DECLARATIONS
//==============================================================================



//==============================================================================
//  LOCAL DATA DECLARATIONS
//==============================================================================
static unsigned short errorLogDataFlashPage = 0u;   //!< This variable contains current dataflash page for errorlog 
static bool isErrorLogInit=false;                                          //!<This falg is used to indicate that Error log has been initialized.
static bool isErrorLogClearAcknowledged = false;                                //!< This variable is used to acknowledge that error log clear is compeleted
static ERRORCODE_ENUM currentErrorCode = ERRORCODE_ENUM_NO_ERROR;               //!< Error code for current error
static bool isErrorLogDueForWDTReset = false;                                   //!< Error code for watchdog reset
static ERROR_INFORMATION_STRUCT errorInformation[TOTAL_NUMBER_OF_ERRORS] = 
{
  { 0 , ERRORCODE_ENUM_WHISPER_DEVICE_ERROR, 0, 0},
  { 1 , ERRORCODE_ENUM_ETHERNET_COMMUNICATION_ERROR, 0, 0},
  { 2 , ERRORCODE_ENUM_WIFI_COMMUNICATION_ERROR, 0, 0},
  { 3 , ERRORCODE_ENUM_CELLULAR_COMMUNICATION_ERROR, 0, 0},
  { 4 , ERRORCODE_ENUM_DATAFLASH_READ_WRITE_ERROR, 0, 0},
  { 5 , ERRORCODE_ENUM_NETWORK_COMMUNICATION_ERROR, 0, 0},
  { 6 , ERRORCODE_ENUM_NFC_COMMUNICATION_ERROR, 0, 0},
  { 7 , ERRORCODE_ENUM_BTLE_COMMUNICATION_ERROR, 0, 0},
  { 8 , ERRORCODE_ENUM_UNDEFINED_MODBUS_REGISTER, 0, 0}, //Using this error, All except above 8 are the legacy error
};
// This variable contains the location in memory where the error is logged 
unsigned char errorLogIndex = 0U;
//==============================================================================
//  LOCAL FUNCTION PROTOTYPES
//==============================================================================

static void FindEntryOfIdentifier(unsigned short seekIdentifier, unsigned char *entryIndexNumber, ERRORCODE_ENUM errorCode);
static void IsDataFlashError(ERRORCODE_ENUM errorCode, bool *isFlashError);
static unsigned short ErrorIndexToPage(unsigned char entryIndex, ERRORCODE_ENUM errorCode );
static unsigned short ErrorNumberToAddress(unsigned char entryIndex);

//==============================================================================
//  LOCAL FUNCTIONS IMPLEMENTATION
//==============================================================================
//------------------------------------------------------------------------------
//   ErrorIndexToPage(unsigned char entryIndex, ERRORCODE_ENUM errorCode )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/09
//
//!  This function finds the page number correponding to the entry index
//
//------------------------------------------------------------------------------

unsigned short ErrorIndexToPage(
                                  unsigned char entryIndex,                     //!< Error Entry Index
                                  ERRORCODE_ENUM errorCode                      //!< Error Code Correponsing to entry index
                               )
{
   //Dataflash Page Numer
   unsigned short pageNumber = 0u;
   //Page Offset
   unsigned char pageOffset = 0u;
   //Calculate the page offset
   pageOffset = (unsigned char) entryIndex/ERRLOG_ENTRIES_PER_PAGE;
   //Get the correct page Number
   switch(errorCode)
   {
     case ERRORCODE_ENUM_WHISPER_DEVICE_ERROR:
        pageNumber = (pageOffset == 0)?WHISPER_DEVICE_ERROR_SUBSECTOR:(WHISPER_DEVICE_ERROR_SUBSECTOR+BACKUP_SUBSECTOR_OFFSET);
        break;
     case ERRORCODE_ENUM_ETHERNET_COMMUNICATION_ERROR:
        pageNumber = (pageOffset == 0)?ETHERNET_COMMUNICATION_ERROR_SUBSECTOR:(ETHERNET_COMMUNICATION_ERROR_SUBSECTOR+BACKUP_SUBSECTOR_OFFSET);
        break;
     case ERRORCODE_ENUM_WIFI_COMMUNICATION_ERROR:
        pageNumber = (pageOffset == 0)?WIFI_COMMUNICATION_ERROR_SUBSECTOR:(WIFI_COMMUNICATION_ERROR_SUBSECTOR+BACKUP_SUBSECTOR_OFFSET);
        break;
     case ERRORCODE_ENUM_CELLULAR_COMMUNICATION_ERROR:
        pageNumber = (pageOffset == 0)?CELLULAR_COMMUNICATION_ERROR_SUBSECTOR:(CELLULAR_COMMUNICATION_ERROR_SUBSECTOR+BACKUP_SUBSECTOR_OFFSET);
        break;
     case ERRORCODE_ENUM_DATAFLASH_READ_WRITE_ERROR:
        pageNumber = (pageOffset == 0)?DATAFLASH_READ_WRITE_ERROR_SUBSECTOR:(DATAFLASH_READ_WRITE_ERROR_SUBSECTOR+BACKUP_SUBSECTOR_OFFSET);
        break;
     case ERRORCODE_ENUM_NETWORK_COMMUNICATION_ERROR:
        pageNumber = (pageOffset == 0)?NETWORK_COMMUNICATION_ERROR_SUBSECTOR:(NETWORK_COMMUNICATION_ERROR_SUBSECTOR+BACKUP_SUBSECTOR_OFFSET);
        break;
     case ERRORCODE_ENUM_NFC_COMMUNICATION_ERROR:
        pageNumber = (pageOffset == 0)?NFC_COMMUNICATION_ERROR_SUBSECTOR:(NFC_COMMUNICATION_ERROR_SUBSECTOR+BACKUP_SUBSECTOR_OFFSET);
        break;
     case ERRORCODE_ENUM_BTLE_COMMUNICATION_ERROR:
        pageNumber = (pageOffset == 0)?BTLE_COMMUNICATION_ERROR_SUBSECTOR:(BTLE_COMMUNICATION_ERROR_SUBSECTOR+BACKUP_SUBSECTOR_OFFSET);
        break;
     default:
        pageNumber = (pageOffset == 0)?LEGACY_ERRORS_SUBSECTOR:(LEGACY_ERRORS_SUBSECTOR+BACKUP_SUBSECTOR_OFFSET);
        break;
   }
   //Return the calculated value
   return pageNumber;
}
//------------------------------------------------------------------------------
//   ErrorNumberToAddress(unsigned char entryIndex)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/09
//
//!  This function finds the page number correponding to the entry index
//
//------------------------------------------------------------------------------
unsigned short ErrorNumberToAddress(
                                     unsigned char entryIndex                   //!< Error entry index
                                   )
{
   //For address
   unsigned short address = 0u;
   //Calculate the address
   address = ( (unsigned short) (entryIndex % ERRLOG_ENTRIES_PER_PAGE) ) * WORDS_PER_ERRLOG;
   //return the calculated value
   return address;
   
}
//------------------------------------------------------------------------------
//   FindEntryOfIdentifier(unsigned short seekIdentifier, unsigned char *entryIndexNumber,ERRORCODE_ENUM errorCode)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function finds the entry number in memory location for the error
//
//------------------------------------------------------------------------------

static void FindEntryOfIdentifier(
                                   unsigned short seekIdentifier,         //!< This variable contains the identifier to be searched in error log
                                   unsigned char *entryIndexNumber,        //!< This variable contains the location of error in memory
                                   ERRORCODE_ENUM errorCode                      //!< Error Code Correponsing to entry index
                                 )
{
    // Identifier is the error number 
    unsigned short errorLogNumber = 0U; 
    unsigned int dataFlashPage;
    unsigned short errorPagePosition;
    // This variable contains the status is error number found or not
    bool isIdentifierFound = false ;
    // For calculating the address
    unsigned short address = 0u;
    *entryIndexNumber = 0U;
    isIdentifierFound = false ;
   //Get the correct page Number
   switch(errorCode)
   {
     case ERRORCODE_ENUM_WHISPER_DEVICE_ERROR:
        dataFlashPage = WHISPER_DEVICE_ERROR_SUBSECTOR;
        break;
     case ERRORCODE_ENUM_ETHERNET_COMMUNICATION_ERROR:
        dataFlashPage = ETHERNET_COMMUNICATION_ERROR_SUBSECTOR;
        break;
     case ERRORCODE_ENUM_WIFI_COMMUNICATION_ERROR:
        dataFlashPage = WIFI_COMMUNICATION_ERROR_SUBSECTOR;
        break;
     case ERRORCODE_ENUM_CELLULAR_COMMUNICATION_ERROR:
        dataFlashPage = CELLULAR_COMMUNICATION_ERROR_SUBSECTOR;
        break;
     case ERRORCODE_ENUM_DATAFLASH_READ_WRITE_ERROR:
        dataFlashPage = DATAFLASH_READ_WRITE_ERROR_SUBSECTOR;
        break;
     case ERRORCODE_ENUM_NETWORK_COMMUNICATION_ERROR:
        dataFlashPage = NETWORK_COMMUNICATION_ERROR_SUBSECTOR;
        break;
     case ERRORCODE_ENUM_NFC_COMMUNICATION_ERROR:
        dataFlashPage = NFC_COMMUNICATION_ERROR_SUBSECTOR;
        break;
     case ERRORCODE_ENUM_BTLE_COMMUNICATION_ERROR:
        dataFlashPage = BTLE_COMMUNICATION_ERROR_SUBSECTOR;
        break;
     default:
        dataFlashPage = LEGACY_ERRORS_SUBSECTOR;
        break;
   }
    while ( (isIdentifierFound != true) && ((*entryIndexNumber) < TOTAL_ERROR_ENTRIES) )
    {  
        errorPagePosition = (unsigned short)(*entryIndexNumber) % ERRLOG_ENTRIES_PER_PAGE ;
        // If it is the start of a new page, erase the page
        if ( errorPagePosition == 0U )
        {
            dataFlashPage = ErrorIndexToPage(*entryIndexNumber,errorCode);
        }
        // Get the address
        address = ErrorNumberToAddress(*entryIndexNumber);
        // Read the error identifier from entry number   
        DataFlashReadGeneralWord( (unsigned short)dataFlashPage, 
            (address + ERRLOG_IDENTIFIER_OFFSET), &errorLogNumber );
        if ( (errorLogNumber <= MAX_VALID_ERROR_IDENTIFIER) && (errorLogNumber == seekIdentifier) )
        {
            isIdentifierFound = true ;
        }
        else
        {
            (*entryIndexNumber)++ ;
        }
    }
    
    if ( isIdentifierFound != true )
    {
        *entryIndexNumber = INVALID_ENTRY_NUMBER ;
    }
}

//------------------------------------------------------------------------------
//   IsDataFlashError(ERRORCODE_ENUM errorCode, BOOLEAN *isFlashError)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function checks the dataflash error
//
//------------------------------------------------------------------------------

static void IsDataFlashError(
                             ERRORCODE_ENUM errorCode,      //!< This variable constant error codes of a structure errorCodeENUM 
                             bool *isFlashError          //!< This variable constant dataflash error status True or False
                             )
{
    // Error code is of dataflash error
    if ( ((unsigned int)errorCode > FIRST_DATAFLASH_ERROR) && (((unsigned int) errorCode) <= LAST_DATAFLASH_ERROR) )
    {
        *isFlashError = true;
    }
    else
    {
        *isFlashError = false;
    }
}


//==============================================================================
//  GLOBAL FUNCTIONS IMPLEMENTATION
//==============================================================================
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
                                  )
{
    // It represents the location in memory where error is logged
    unsigned char errorLogEntryNumber= 0U;  
    // Identifier is the error number
    unsigned short errorLogNumber = 0U; 
    // Address in memory to start reading from
    unsigned short wordAddress = 0U;  
    // Temporary variable contains the address in the page to start reading from
    unsigned short  tempWordAddress;
    // This variable is used to update the new secotor ID.
    unsigned short dataFlashPage;
    //This variable is used to check if the page boundry has reached or not       
    unsigned short errorPagePosition;
        //Get the correct page Number
        switch(errorCode)
        {
        case ERRORCODE_ENUM_WHISPER_DEVICE_ERROR:
        dataFlashPage = WHISPER_DEVICE_ERROR_SUBSECTOR;
        break;
        case ERRORCODE_ENUM_ETHERNET_COMMUNICATION_ERROR:
        dataFlashPage = ETHERNET_COMMUNICATION_ERROR_SUBSECTOR;
        break;
        case ERRORCODE_ENUM_WIFI_COMMUNICATION_ERROR:
        dataFlashPage = WIFI_COMMUNICATION_ERROR_SUBSECTOR;
        break;
        case ERRORCODE_ENUM_CELLULAR_COMMUNICATION_ERROR:
        dataFlashPage = CELLULAR_COMMUNICATION_ERROR_SUBSECTOR;
        break;
        case ERRORCODE_ENUM_DATAFLASH_READ_WRITE_ERROR:
        dataFlashPage = DATAFLASH_READ_WRITE_ERROR_SUBSECTOR;
        break;
        case ERRORCODE_ENUM_NETWORK_COMMUNICATION_ERROR:
        dataFlashPage = NETWORK_COMMUNICATION_ERROR_SUBSECTOR;
        break;
        case ERRORCODE_ENUM_NFC_COMMUNICATION_ERROR:
        dataFlashPage = NFC_COMMUNICATION_ERROR_SUBSECTOR;
        break;
       case ERRORCODE_ENUM_BTLE_COMMUNICATION_ERROR:
        dataFlashPage = BTLE_COMMUNICATION_ERROR_SUBSECTOR;
        break;
        default:
        dataFlashPage = LEGACY_ERRORS_SUBSECTOR;
        break;
        }
        // Scan the error log, look for the highest error identifier
        for ( errorLogEntryNumber = 0U; errorLogEntryNumber < TOTAL_ERROR_ENTRIES; errorLogEntryNumber++ )
        {
            errorPagePosition = (unsigned short)errorLogEntryNumber % ERRLOG_ENTRIES_PER_PAGE ;
            // If it is the start of a new page, erase the page
            if ( errorPagePosition == 0U )
            {
                dataFlashPage = ErrorIndexToPage(errorLogEntryNumber,errorCode);
            }
            // Address of entry number 
            wordAddress = ErrorNumberToAddress(errorLogEntryNumber) + ERRLOG_IDENTIFIER_OFFSET;
            tempWordAddress = wordAddress;
            // Read the entry number to get the error identifier
            DataFlashReadGeneralWord( dataFlashPage, tempWordAddress, &errorLogNumber );
            if ( (errorLogNumber > (*highestIdentifier)) &&
                (errorLogNumber <= MAX_VALID_ERROR_IDENTIFIER) )
            {
                *highestIdentifier = errorLogNumber ;
            }
        }        
}
//------------------------------------------------------------------------------
//   void ErrorLogInit(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function initializes the Errorlog parameters.
//
//------------------------------------------------------------------------------

void ErrorLogInit(void)
{
    //For indexing the loop
    unsigned loopIndex = 0u;
    if(isErrorLogInit == false)
    {
        while ( loopIndex < TOTAL_NUMBER_OF_ERRORS )
        {
            // Get the latest error identifier in the error log
            ErrorLogFindLatestIdentifier( &errorInformation[loopIndex].errorIdentifier, errorInformation[loopIndex].errorCode ) ;
            // Get the entry number for the error
            FindEntryOfIdentifier( errorInformation[loopIndex].errorIdentifier, &errorInformation[loopIndex].entryIndex, errorInformation[loopIndex].errorCode ) ;
            //Increment the loop
            loopIndex++;
        }
        // Set flag to indicate ErrorLog has been initialized
        isErrorLogInit = true;
    }
}

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
                   ERRORCODE_ENUM errorCode,          //!< Predefined constant error codes of a structure errorCodeENUM 
                   ERRORTYPE_ENUM errorType           //!< Error type associated with error code
                   )
{
    // Identifier for the next error
  unsigned short nextErrorIdentifier = 1U;           
  // This variable contains the data flash error status True or False
  bool isDataflashError = false;          
  //This variable is used to check if the sector change is required or not
  unsigned short errorPagePosition = 0u;   
  //TODO: Add after the battery module implementation
  bool isCriticalBatteryIndicationOn = false;
  //For indexing the loop
  unsigned char loopIndex = 0u;
  //For checking if the correct index is found
  bool isIndexFound = false;
  //Date/time structure
  DATE_TIME_STRUCT currentDateTime;
  //For the current Position
  unsigned short currentPosition = 0u;
  //For saving the error Type
  ERRORCODE_ENUM currentErrorType = ERRORCODE_ENUM_NO_ERROR;
  currentErrorType = errorCode;
  //Find the correct Index
  //Check if the correct error code is found, otherwise it will be dealt as 
  //Legacy error
  while ( (loopIndex < TOTAL_NUMBER_OF_ERRORS) && (isIndexFound == false) )
  {
      if( errorInformation[loopIndex].errorCode == errorCode )
      {
         isIndexFound = true;
      }
      else
      {
         loopIndex++;
      }
  }
  //If index was not found then this means that loop has got
  //out of bounds so get the index back to last valid index
  if ( isIndexFound == false )
  {
     loopIndex = loopIndex - 1;
  }
  // Critical Battery level 
  if(isCriticalBatteryIndicationOn == false)
  {
    // Get the data flash error status
    IsDataFlashError( errorCode, &isDataflashError );
    // make sure before logging any error, ErrorLog is properly Initiazled
    if ( isErrorLogInit == false)
    {
        ErrorLogInit();
    }
    if ( isDataflashError == false )
      // Don't try to write an error to the dataflash with this error, because the dataflash isn't working correctly anyway
    {
      if ( (errorInformation[loopIndex].errorIdentifier == 0U) || (errorInformation[loopIndex].errorIdentifier > MAX_VALID_ERROR_IDENTIFIER) )
      { 
        // Make error entry number zero      
        errorInformation[loopIndex].entryIndex = 0U ;
        errorInformation[loopIndex].errorIdentifier = 1U;
        nextErrorIdentifier = 1U ;
        // Get the dataflash page number
        errorLogDataFlashPage = ErrorIndexToPage(errorInformation[loopIndex].entryIndex, errorInformation[loopIndex].errorCode);
      }
      else
      {
        nextErrorIdentifier = errorInformation[loopIndex].errorIdentifier + 1;
        //Increment in error identifier
        errorInformation[loopIndex].errorIdentifier = errorInformation[loopIndex].errorIdentifier + 1;
        if ( nextErrorIdentifier > MAX_VALID_ERROR_IDENTIFIER )
        {
          nextErrorIdentifier = 1U ;
        }
        // Overwrite the entry following the latest
        errorInformation[loopIndex].entryIndex = errorInformation[loopIndex].entryIndex + 1 ;
        if ( errorInformation[loopIndex].entryIndex >= TOTAL_ERROR_ENTRIES )
        {
          errorInformation[loopIndex].entryIndex = 0U;
        }             
        errorPagePosition = (unsigned short)errorInformation[loopIndex].entryIndex % ERRLOG_ENTRIES_PER_PAGE ;
        // Get the dataflash page number
        errorLogDataFlashPage = ErrorIndexToPage(errorInformation[loopIndex].entryIndex, errorInformation[loopIndex].errorCode);;
        // If it is the start of a new page, erase the page
        if ( errorPagePosition == 0U )
        {
           DataFlashErasePage( errorLogDataFlashPage ) ;
        }
      }
      // Update real time
      RTCGetCurrentDateTime(&currentDateTime); 
      //Get the current Position
      currentPosition = ErrorNumberToAddress(errorInformation[loopIndex].entryIndex);
      // Write the error identifier to the empty entry number
      DataFlashWriteGeneralWord( errorLogDataFlashPage, (currentPosition + ERRLOG_IDENTIFIER_OFFSET), nextErrorIdentifier );
      // Write the error code of the error occurred to entry number
      DataFlashWriteGeneralWord( errorLogDataFlashPage, (currentPosition + ERRLOG_CODE_OFFSET), currentErrorType );
      // Write the month and date of the error occurred to entry number
      DataFlashWriteGeneralWord( errorLogDataFlashPage, (currentPosition + ERRLOG_TIME1_OFFSET), ((unsigned short)currentDateTime.monthId << 8) + currentDateTime.dayId );
      // Write the year of the error occurred to entry number
      DataFlashWriteGeneralWord( errorLogDataFlashPage, (currentPosition + ERRLOG_TIME2_OFFSET), (unsigned short)currentDateTime.yearId );
      // Write the hour and minutes of the error occurred to entry number
      DataFlashWriteGeneralWord( errorLogDataFlashPage, (currentPosition + ERRLOG_TIME3_OFFSET), ((unsigned short)currentDateTime.hourId << 8) + currentDateTime.minId );
      // Write the seconds of the error occurred to entry number
      DataFlashWriteGeneralWord( errorLogDataFlashPage, (currentPosition + ERRLOG_TIME4_OFFSET), (unsigned short)currentDateTime.secondId );
    } 
  }
}

//------------------------------------------------------------------------------
//   ErrorLogRead(unsigned int *data, ERRORCODE_ENUM errorCode )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/10
//
//!  This function reads the error code and time byte of the selected error form the log
//
//------------------------------------------------------------------------------

void ErrorLogRead(
                   unsigned short *data,            //!< This variable contains the data which is read
                   ERRORCODE_ENUM errorCode        //!< Data type to be read error code or time byte
                  )
{
    unsigned short dataFlashPageReadId = 0u;
    //For indexing the loop
    unsigned char loopIndex = 0u;
    //For checking if the correct index is found
    bool isIndexFound = false;
    //For the current address
    unsigned short currentAddress = 0u;
    //For the number of errors download tracking
    char trackDownload = 0u;
    //For the entry index
    unsigned char entryIndex = 0u;
    //Initialize the errorLog Just in case it has not been initialized earlier
    ErrorLogInit();
    //Get the correct index
    //Find the correct Index
   //Check if the correct error code is found, otherwise it will be dealt as 
   //Legacy error
    if ( errorCode != ERRORCODE_ENUM_NO_ERROR )
    {
       while ( (loopIndex < TOTAL_NUMBER_OF_ERRORS) && (isIndexFound == false) )
       {
           if( errorInformation[loopIndex].errorCode == errorCode )
           {
              isIndexFound = true;
           }
           else
           {
              loopIndex++;
           }
       }
    }
    else
    {
       //Do not loop through to find the correct index as user has asked for full
       //ErrorLog Download
    }
    //if the loop Index has gone to maximum value then this mean that it is 
    //a legacy error is being read so correct the index value here
    if( isIndexFound == false )
    {
       loopIndex = loopIndex -1;
    }
    if ( errorCode == ERRORCODE_ENUM_NO_ERROR )
    {
       for ( loopIndex = 0; loopIndex < TOTAL_NUMBER_OF_ERRORS; loopIndex++ )
       {
          entryIndex = errorInformation[loopIndex].entryIndex;
          for( trackDownload = 0; trackDownload < 20; trackDownload++ )
          {
              if ( entryIndex == INVALID_ENTRY_NUMBER )
              {
                 //break the process as we have nothing good here
                 break;
              }
              dataFlashPageReadId = ErrorIndexToPage(errorInformation[loopIndex].entryIndex, errorInformation[loopIndex].errorCode);
              currentAddress = ErrorNumberToAddress(errorInformation[loopIndex].entryIndex);
              // Read the data of the selected error
              DataFlashReadGeneralWord( dataFlashPageReadId, (currentAddress + ERRLOG_CODE_OFFSET), data);
              data++;
              DataFlashReadGeneralWord( dataFlashPageReadId, (currentAddress + ERRLOG_TIME1_OFFSET), data);
              data++;
              DataFlashReadGeneralWord( dataFlashPageReadId, (currentAddress + ERRLOG_TIME2_OFFSET), data);
              data++;
              DataFlashReadGeneralWord( dataFlashPageReadId, (currentAddress + ERRLOG_TIME3_OFFSET), data);
              data++;
              DataFlashReadGeneralWord( dataFlashPageReadId, (currentAddress + ERRLOG_TIME4_OFFSET), data);
              data++;
              entryIndex = entryIndex - 1;
          }
       }
     
    }
    else
    {
       entryIndex = errorInformation[loopIndex].entryIndex;
       for( trackDownload = 0; trackDownload < 20; trackDownload++ )
       {
           if ( entryIndex == INVALID_ENTRY_NUMBER )
           {
              //break the process as we have nothing good here
               break;
           }
           dataFlashPageReadId = ErrorIndexToPage(errorInformation[loopIndex].entryIndex, errorInformation[loopIndex].errorCode);
           currentAddress = ErrorNumberToAddress(errorInformation[loopIndex].entryIndex);
           // Read the data of the selected error
           DataFlashReadGeneralWord( dataFlashPageReadId, (currentAddress + ERRLOG_CODE_OFFSET), data);
           data++;
           DataFlashReadGeneralWord( dataFlashPageReadId, (currentAddress + ERRLOG_TIME1_OFFSET), data);
           data++;
           DataFlashReadGeneralWord( dataFlashPageReadId, (currentAddress + ERRLOG_TIME2_OFFSET), data);
           data++;
           DataFlashReadGeneralWord( dataFlashPageReadId, (currentAddress + ERRLOG_TIME3_OFFSET), data);
           data++;
           DataFlashReadGeneralWord( dataFlashPageReadId, (currentAddress + ERRLOG_TIME4_OFFSET), data);
           data++;
           entryIndex = entryIndex - 1;
       }
    }
}
//------------------------------------------------------------------------------
//   ErrorLogClear(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function clears the error log
//
//------------------------------------------------------------------------------

void ErrorLogClear(void)
{
//    // This variable contains the location in memory where the error is logged 
//    unsigned char errorLogIndex = 0U;
    // This variable contains the address in the page to start writing to 
    unsigned short wordAddress = 0U;
    // Temporary variable contains the address in the page to start writing to
    unsigned short tempWordAddress;
    // This variable is used to get error log page position 
    unsigned int errorPagePosition = 0u;
    // This variable is used for indexing the loop
    unsigned char loopIndex = 0;
    // Go through all the errors
    for ( loopIndex = 0; loopIndex < TOTAL_NUMBER_OF_ERRORS; loopIndex++ )
    {
       for ( errorLogIndex = 0U; errorLogIndex < TOTAL_ERROR_ENTRIES; errorLogIndex++ )
       {
           errorPagePosition = (unsigned int)errorLogIndex % ERRLOG_ENTRIES_PER_PAGE ;
           // If it is the start of a new page, erase the page
           if ( errorPagePosition == 0U )
           {
               // Get the dataflash page number
               errorLogDataFlashPage = ErrorIndexToPage(errorLogIndex,errorInformation[loopIndex].errorCode);
               DataFlashErasePage( errorLogDataFlashPage ) ;
           }
           // Address of entry number 
           wordAddress = ErrorNumberToAddress(errorLogIndex) + ERRLOG_IDENTIFIER_OFFSET;
           tempWordAddress = wordAddress;
           // Clear the log by setting the default values
           DataFlashWriteGeneralWord( errorLogDataFlashPage, tempWordAddress, INVALID_ERROR_IDENTIFIER );
       }
       errorInformation[loopIndex].errorIdentifier=0U;
       errorInformation[loopIndex].entryIndex=INVALID_ENTRY_NUMBER;
    }
    
}
//
////------------------------------------------------------------------------------
////   ErrorLogSelectIdentifierForReadback(unsigned short errorLogNumber)
////
////   Author:   Ali Zulqarnain Anjum
////   Date:     2016/12/01
////
////!  This function selects the error identifier for read process and also clears the error log
////
////------------------------------------------------------------------------------
//
//void ErrorLogSelectIdentifierForReadback(
//                                            unsigned short errorLogNumber      //!< Error log number
//                                        )
//{
//  //TODO:
//    bool isInstrumentConfigured = true;
//  
//    // If instrument is put in factory unconfigured mode and error log is already cleared then don't clear it on explicit command
//    if(((errorLogNumber == 0U ) && (isInstrumentConfigured == true)))
//    {
//        // All logs are cleared by setting identifier to dispatch the Modbus response immediately  
//        //logClearRequested |= ERRORLOG_CLEAR_REQUEST;
//        isErrorLogClearAcknowledged = false;
//    }
//    else
//    {
//        selectedErrorEntry = (unsigned char)((errorLogNumber - 1U) % TOTAL_ERROR_ENTRIES);
//    }
//}
//

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
                                 )
{
    //Set the variable
    isErrorLogClearAcknowledged = value;
}
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
                             )
{
     //Set the variable
     isErrorLogDueForWDTReset = value;
}
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
                                 )
{
    //Set the variable
    currentErrorCode = errorCode;
}
//------------------------------------------------------------------------------
//   bool ErrorLogGetClearAcknowledge(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function return the errorlog clearing variable.
//
//------------------------------------------------------------------------------

bool ErrorLogGetClearAcknowledge(void)
{
   //return the value
   return isErrorLogClearAcknowledged;
}

//------------------------------------------------------------------------------
//   bool ErrorLogGetLogDueWatchdog(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function returns the condition for logging the watchdog reset
//
//------------------------------------------------------------------------------

bool ErrorLogGetLogDueWatchdog(void)
{
   //retirn the value
   return isErrorLogDueForWDTReset;
}

//------------------------------------------------------------------------------
//   ERRORCODE_ENUM ErrorLogGetCurrentErrorCode(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/01
//
//!  This function returns the current error code
//
//------------------------------------------------------------------------------

ERRORCODE_ENUM ErrorLogGetCurrentErrorCode(void)
{
   //return the value
   return currentErrorCode;
}
//==============================================================================
//  End Of File
//==============================================================================