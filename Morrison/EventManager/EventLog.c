//==============================================================================
//
//  Eventlog.c
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
//  Source:        Eventlog.c
//
//  Project:       Morrison
//
//  Author:        Ali Zulqarnain Anjum
//
//  Date:          2016/12/14
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This module provides the functionality related to logging of error. It handles
//! different events of Morrison
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//  Revision: 1.0  2016/12/14  Ali Zulqarnain Anjum
//      This module deals with different events of Morrison device.
//
//==============================================================================

//==============================================================================
//  INCLUDES 
//==============================================================================

#include "EventLog.h"
#include "Dataflash.h"
#include "ErrorLog.h"
#include "TM4CRTC.h"
#include "TM4CEEPROM.h"
#include "TM4CRTC.h"

//==============================================================================
//  CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

#define SUBSECTOR_BLOCK_INTERNAL_EEPROM 0                                       //!< Subsector block number in the internal EEPROM
#define SUBSECTOR_WORD_INTERNAL_EEPROM 0                                        //!< Subsector word numbr in the internal EEPROM
#define EVENT_COUNT_BLOCK_INTERNAL_EEPROM 0                                     //!< event counter block number in the internal EEPROM
#define EVENT_COUNT_WORD_INTERNAL_EEPROM 1                                      //!< event counter word numbr in the internal EEPROM
#define FIRST_EVENTLOG_SUBSECTOR ((TOTAL_NUMBER_OF_ERRORS*2)+1)                 //!< First event log subsector
#define LAST_EVENTLOG_SUBSECTOR 4089                                            //!< Last event log subsector  
#define MORRISON_INSTRUMENT_TYPE 0xAAAA                                         //!< Morrison instrument type TODO: update it
#define INTERNAL_EVENT_LENGTH 97                                                //!< Internal event length TODO: update it
#define EXTERNAL_EVENT_LENGTH 97                                                //!< External event length TODO: update it

//==============================================================================
//  LOCAL DATA STRUCTURE DEFINITION
//==============================================================================
   
//==============================================================================
//  GLOBAL DATA DECLARATIONS
//==============================================================================

unsigned int subsectorNumber = 0u;                                              //!< Next subsector number in which the event log will be saved
static unsigned short ramArrayIndex = 0u;                                       //!< RAM array index for storing the event log
unsigned char eventLogWriteArray[EVENT_LOG_WRITE_ARRAY_LENGTH] = {0xFFu};       //!< Array for storing the event log in the RAM
unsigned char eventLogReadArray[EVENT_LOG_READ_ARRAY_LENGTH] = {0u};            //!< Array for reading the event log
static bool isEventLogInit = false;                                             //!< To track if the event log has been initialized
static unsigned int eventCount = 0u;                                           //!< For counting the event


//==============================================================================
//  LOCAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
//  LOCAL FUNCTION PROTOTYPES
//==============================================================================
static unsigned char GetEventLength(EVENTLOG_ID_ENUM eventID);
void CommitBufferToDataflash();
void CopyDataToBuffer();
//==============================================================================
//  LOCAL FUNCTIONS IMPLEMENTATION
//==============================================================================
//------------------------------------------------------------------------------
//   GetEventLength(EVENTLOG_ID_ENUM eventID)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/14
//
//!  This function returns the length of the given ID
//
//------------------------------------------------------------------------------
static unsigned char GetEventLength(
                                       EVENTLOG_ID_ENUM eventID                 //!< Event ID
                                   )
{
   //For event length
   unsigned char eventLength = 0u;
   //Get the Event length based on the given ID
   switch(eventID)
   {
     case EVENTLOG_ID_INST_LOST_EVENT:
     eventLength = 24;
     break;
     case EVENTLOG_ID_LPONLINE_EVENT:
     eventLength = 8;
     break;
     case EVENTLOG_ID_LPOFFLINE_EVENT:
     eventLength = 8;
     break;
     case EVENTLOG_ID_LB_CREATE_EVENT:
     eventLength = 8;
     break;
     case EVENTLOG_ID_NO_LB_EVENT:
     eventLength = 8;
     break;
     case EVENTLOG_ID_GPS_UPDATE_EVENT:
     eventLength = 16;
     break;
     case EVENTLOG_ID_LP_KEEPALIVE_EVENT:
     eventLength = 8;
     break;
     case EVENTLOG_ID_SITE_UPDATE_EVENT:
     eventLength = 24;
     break;
     case EVENTLOG_ID_IMEI_UPDATE_EVENT:
     eventLength = 23;
     break;
     case EVENTLOG_ID_CELL_RSSI_EVENT:
     eventLength = 10;
     break;
     case EVENTLOG_ID_ERROR_STATUS_EVENT:
     eventLength = 10;
     break;
     case EVENTLOG_ID_LEAVE_GROUP_EVENT:
     eventLength = 24;
     break;
     case EVENTLOG_ID_JOIN_GROUP_EVENT:
     eventLength = 91;
     break;
     case EVENTLOG_ID_MAN_DOWN_EVENT:
     eventLength = 24;
     break;
     case EVENTLOG_ID_MANDOWN_CLEAR_EVENT:
     eventLength = 24;
     break;
     case EVENTLOG_ID_USER_UPDATE_EVENT:
     eventLength = 40;
     break;
     case EVENTLOG_ID_PANIC_ALARM_EVENT:
     eventLength = 24;
     break;
     case EVENTLOG_ID_PANIC_CLEAR_EVENT:
     eventLength = 24;
     break;
     case EVENTLOG_ID_SENSOR_UPDATE_EVENT:
     eventLength = 59;
     break;
     case EVENTLOG_ID_PUMP_ERROR_EVENT:
     eventLength = 24;
     break;
     case EVENTLOG_ID_PUMP_READY_EVENT:
     eventLength = 24;
     break;
     case EVENTLOG_ID_HIGH_ALARM_EVENT:
     eventLength = 59;
     break;
     case EVENTLOG_ID_LOW_ALARM_EVENT:
     eventLength = 59;
     break;
     case EVENTLOG_ID_STEL_ALARM_EVENT:
     eventLength = 59;
     break;
     case EVENTLOG_ID_TWA_ALARM_EVENT :
     eventLength = 59;
     break;
     case EVENTLOG_ID_GAS_ALARM_CLEAR_EVENT:
     eventLength = 59;
     break;
   default:
     break;
   }
   //Return the length
   return eventLength;
}
//------------------------------------------------------------------------------
//  CommitBufferToDataflash(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function commits the RAM buffer to dataflash 
//
//------------------------------------------------------------------------------
void CommitBufferToDataflash(void)
{
   //For indexing the loop
   unsigned short loopIndex = 0;
   //To check if the EEPROM write is correct
   bool isWriteCorrect = false;
   //Write data to dataflash
   DataFlashCommitBuffer(eventLogWriteArray,subsectorNumber);
   //Next subsector to be written in
   subsectorNumber = subsectorNumber + 1;
   //Check the range
   if ( subsectorNumber > LAST_EVENTLOG_SUBSECTOR )
   {
      subsectorNumber = FIRST_EVENTLOG_SUBSECTOR;
   }
   //Write in EEPROM
   isWriteCorrect = TM4CEEPROMWriteData(&subsectorNumber,(unsigned char) SUBSECTOR_BLOCK_INTERNAL_EEPROM, (unsigned char) SUBSECTOR_WORD_INTERNAL_EEPROM, (unsigned char) 1 );
   //Now Reset the write array
   while( loopIndex < EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
       eventLogWriteArray[loopIndex] = 0xFF;
       loopIndex++;
   }
   //Reset the index
   ramArrayIndex = 0u;
}
//------------------------------------------------------------------------------
//  CopyDataToBuffer(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/19
//
//!  This function copies data from incomplete sub-sector to RAM
//
//------------------------------------------------------------------------------
void CopyDataToBuffer()
{
   //For Getting the number of events
   unsigned int numberOfEventsSaved = 0u;
   //Get the number of events saved in the sub-sector
   numberOfEventsSaved = eventCount%32;
   //Read the data from the given sub-sector
   DataFlashReadSector(subsectorNumber,eventLogReadArray);
   //Copy the data to the RAM Array
   for( ramArrayIndex = 0; ramArrayIndex < (numberOfEventsSaved*128); ramArrayIndex++ )
   {
      //Copy data to RAM buffer
      eventLogWriteArray[ramArrayIndex] = eventLogReadArray[ramArrayIndex];
   }
}
//==============================================================================
//  GLOBAL FUNCTIONS IMPLEMENTATION
//==============================================================================
//------------------------------------------------------------------------------
//   EventLogInit()
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/14
//
//!  This function initializes the event log
//
//------------------------------------------------------------------------------
void EventLogInit()
{
   //To check if the EEPROM read is correct
   bool isReadCorrect = false;
   //Check if the event log has not been initialized yet
   if ( isEventLogInit == false )
   {
      isReadCorrect = TM4CEEPROMReadData( &subsectorNumber, (unsigned char) SUBSECTOR_BLOCK_INTERNAL_EEPROM, (unsigned char) SUBSECTOR_WORD_INTERNAL_EEPROM, (unsigned char) 1 );
      //If there is no error 
      if ( isReadCorrect == true )
      {
         if ( (subsectorNumber < FIRST_EVENTLOG_SUBSECTOR) || (subsectorNumber > LAST_EVENTLOG_SUBSECTOR) )
         {
             subsectorNumber = FIRST_EVENTLOG_SUBSECTOR;
         }
         else
         {
            //Do nothig
         }
      }
      else
      {
         //Initialize with default value
         subsectorNumber = FIRST_EVENTLOG_SUBSECTOR;
      }
      //Now read the event counter
      isReadCorrect = TM4CEEPROMReadData(&eventCount, (unsigned char) EVENT_COUNT_BLOCK_INTERNAL_EEPROM, (unsigned char) EVENT_COUNT_WORD_INTERNAL_EEPROM, (unsigned char) 1 );
      //If there is no error 
      if ( isReadCorrect == true )
      {
        //Do nothing TODO: Maximum value check
      }
      else
      {
         //Initialize with default value
         eventCount = 0u;
      }
      //If complete sub-sector of the event log was written then we don't need it in 
      //our RAM
      if( (eventCount%32) == 0 )
      {
         //Reset the index
         ramArrayIndex = 0u;     
      }
      else
      {
         //Decrement in Sub-sector number as  data will be written to previous sub-sector
         if ( subsectorNumber != FIRST_EVENTLOG_SUBSECTOR )
         {
            subsectorNumber = subsectorNumber - 1;
         }
         else
         {
            //Do nothing
         }
         //Copy the incomplete subsector data to RAM buffer
         CopyDataToBuffer();
      }
      isEventLogInit = true;
    }
    else
    {
       //Do nothing as eventLog has already been initialized
    }
}
//------------------------------------------------------------------------------
//   EventLogWriteInstrumentLostEvent( unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/14
//
//!  This function writes instrument lost event
//
//------------------------------------------------------------------------------
void EventLogWriteInstrumentLostEvent( 
                                        unsigned short peerNumber               //!< Peer Number
                                     )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_INST_LOST_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_INST_LOST_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
    //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteLeaveGroupEvent( unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function writes leave group event
//
//------------------------------------------------------------------------------
void EventLogWriteLeaveGroupEvent( 
                                     unsigned short peerNumber                  //!< Peer Number
                                 )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_LEAVE_GROUP_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_LEAVE_GROUP_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteManDownEvent( unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function writes man down event
//
//------------------------------------------------------------------------------
void EventLogWriteManDownEvent( 
                                  unsigned short peerNumber                     //!< Peer Number
                              )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_MAN_DOWN_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_MAN_DOWN_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteManDownClearEvent( unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function writes man down clear event
//
//------------------------------------------------------------------------------
void EventLogWriteManDownClearEvent( 
                                      unsigned short peerNumber                 //!< Peer Number
                                   )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_MANDOWN_CLEAR_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_MANDOWN_CLEAR_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWritePanicEvent( unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function writes panic event
//
//------------------------------------------------------------------------------
void EventLogWritePanicEvent( 
                                unsigned short peerNumber                       //!< Peer Number
                            )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_PANIC_ALARM_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_PANIC_ALARM_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWritePanicClearEvent( unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function writes panic clear event
//
//------------------------------------------------------------------------------
void EventLogWritePanicClearEvent( 
                                     unsigned short peerNumber                  //!< Peer Number
                                 )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_PANIC_CLEAR_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_PANIC_CLEAR_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWritePumpEvent( unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function writes pump alarm event
//
//------------------------------------------------------------------------------
void EventLogWritePumpEvent( 
                               unsigned short peerNumber                        //!< Peer Number
                           )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_PUMP_ERROR_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_PUMP_ERROR_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWritePumpClearEvent( unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function writes pump alarm clear event
//
//------------------------------------------------------------------------------
void EventLogWritePumpClearEvent( 
                                   unsigned short peerNumber                    //!< Peer Number
                                )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_PUMP_READY_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_PUMP_READY_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteGasAlarmEvent( EVENTLOG_ID_ENUM eventID, unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function writes gas alarm event
//
//------------------------------------------------------------------------------
void EventLogWriteGasAlarmEvent( 
                                  EVENTLOG_ID_ENUM eventID,                     //!< event ID
                                  unsigned short peerNumber                     //!< Peer Number
                               )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //For Sensor Information (TODO: this is temp. and will be replaced with the actual data afterwards
   SENSOR_STATUS_STRUCT sensorStaus[TOTAL_NUMBER_OF_SENSORS];
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) eventID;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(eventID);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Reset the datalength
   dataLength = 0;
   //Save the sensor information
   while ( dataLength < TOTAL_NUMBER_OF_SENSORS )
   {
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorType; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorUnits; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].senorStatusAndDecimal; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorReading[0]; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorReading[1]; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in datalength
      dataLength = dataLength + 1;
   }    
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteGasAlarmClearEvent(unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function writes pump alarm clear event
//
//------------------------------------------------------------------------------
void EventLogWriteGasAlarmClearEvent( 
                                       unsigned short peerNumber                //!< Peer Number
                                    )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //For Sensor Information (TODO: this is temp. and will be replaced with the actual data afterwards
   SENSOR_STATUS_STRUCT sensorStaus[TOTAL_NUMBER_OF_SENSORS];
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_GAS_ALARM_CLEAR_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_GAS_ALARM_CLEAR_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Reset the datalength
   dataLength = 0;
   //Save the sensor information
   while ( dataLength < TOTAL_NUMBER_OF_SENSORS )
   {
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorType; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorUnits; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].senorStatusAndDecimal; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorReading[0]; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorReading[1]; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in datalength
      dataLength = dataLength + 1;
   }    
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteInstrumentJoinEvent(unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function writes instrument join event
//
//------------------------------------------------------------------------------
void EventLogWriteInstrumentJoinEvent( 
                                        unsigned short peerNumber                //!< Peer Number
                                     )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //For Sensor Information (TODO: this is temp. and will be replaced with the actual data afterwards
   SENSOR_STATUS_STRUCT sensorStaus[TOTAL_NUMBER_OF_SENSORS];
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_JOIN_GROUP_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_JOIN_GROUP_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Reset the datalength
   dataLength = 0;
   //Save the instrument user name
   while ( dataLength < USER_SITE_NAME_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = 0xAA; //TODO: add peer's user name here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Reset the datalength
   dataLength = 0;
   //Save the instrument serial number
   while ( dataLength < USER_SITE_NAME_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = 0xBB; //TODO: add peer's site name here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Reset the datalength
   dataLength = 0;
   //Save the sensor information
   while ( dataLength < TOTAL_NUMBER_OF_SENSORS )
   {
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorType; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorUnits; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].senorStatusAndDecimal; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorReading[0]; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorReading[1]; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in datalength
      dataLength = dataLength + 1;
   }    
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteUserUpdateEvent( unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function writes user update event
//
//------------------------------------------------------------------------------
void EventLogWriteUserUpdateEvent( 
                                     unsigned short peerNumber                  //!< Peer Number
                                  )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_USER_UPDATE_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_USER_UPDATE_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Reset the datalength
   dataLength = 0;
   //Save the instrument user name
   while ( dataLength < USER_SITE_NAME_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = 0xAA; //TODO: add peer's user name here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteSensorUpdateEvent(unsigned short peerNumber )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/16
//
//!  This function writes sensor update event
//
//------------------------------------------------------------------------------
void EventLogWriteSensorUpdateEvent( 
                                       unsigned short peerNumber                //!< Peer Number
                                    )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //For Sensor Information (TODO: this is temp. and will be replaced with the actual data afterwards
   SENSOR_STATUS_STRUCT sensorStaus[TOTAL_NUMBER_OF_SENSORS];
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_SENSOR_UPDATE_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_SENSOR_UPDATE_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument serial number
   while ( dataLength < DEVICE_ID_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = eventLength; //TODO: add peer's SN here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Reset the datalength
   dataLength = 0;
   //Save the sensor information
   while ( dataLength < TOTAL_NUMBER_OF_SENSORS )
   {
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorType; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorUnits; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].senorStatusAndDecimal; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorReading[0]; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      eventLogWriteArray[ramArrayIndex] = sensorStaus[dataLength].sensorReading[1]; //TODO: add peer's sensor information here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in datalength
      dataLength = dataLength + 1;
   }    
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteLPStatusEvent( EVENTLOG_ID_ENUM eventID )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/14
//
//!  This function writes LP online/offline/keep alive events lost event
//
//------------------------------------------------------------------------------
void EventLogWriteLPStatusEvent( 
                                   EVENTLOG_ID_ENUM eventID                     //!< Event ID
                               )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) eventID;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(eventID);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteLPBatteryEvent( EVENTLOG_ID_ENUM eventID )
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/14
//
//!  This function writes LP battery create/clear event
//
//------------------------------------------------------------------------------
void EventLogWriteLPBatteryEvent( 
                                    EVENTLOG_ID_ENUM eventID                    //!< Event ID
                                )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) eventID;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(eventID);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteLPSiteUpdateEvent(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/14
//
//!  This function writes LP site update event
//
//------------------------------------------------------------------------------
void EventLogWriteLPSiteUpdateEvent(void)
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_SITE_UPDATE_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_SITE_UPDATE_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument site name
   while ( dataLength < USER_SITE_NAME_LENGTH )
   {
      eventLogWriteArray[ramArrayIndex] = 0xBB; //TODO: add LP's site name here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteLPSGPSEvent(unsigned char *GPSByte)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/14
//
//!  This function writes LP GPS event
//
//------------------------------------------------------------------------------
void EventLogWriteLPSGPSEvent(
                                unsigned char *GPSByte                          //!< GPS Data                           
                             )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_GPS_UPDATE_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_GPS_UPDATE_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument GPS Data
   while ( dataLength < 8 )
   {
      eventLogWriteArray[ramArrayIndex] = 0xDD; //TODO: add LP's GPS Bytes here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteIMEIUpdateEvent(unsigned char *IMEIInfo)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/14
//
//!  This function writes LP IMEI event
//
//------------------------------------------------------------------------------
void EventLogWriteIMEIUpdateEvent(
                                    unsigned char *IMEIInfo                     //!< IMEI Infor                         
                                 )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_IMEI_UPDATE_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_IMEI_UPDATE_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument GPS Data
   while ( dataLength < 15 )
   {
      eventLogWriteArray[ramArrayIndex] = 0xCC; //TODO: add LP's IMEI Bytes here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteRSSIUpdateEvent(unsigned short RSSI)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/14
//
//!  This function writes LP RSSI Update event
//
//------------------------------------------------------------------------------
void EventLogWriteRSSIUpdateEvent(
                                    unsigned short RSSI                         //!< IMEI Infor                         
                                 )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_CELL_RSSI_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_CELL_RSSI_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument GPS Data
   while ( dataLength < 2 )
   {
      eventLogWriteArray[ramArrayIndex] = 0xEE; //TODO: add LP's RSSI Bytes here
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogWriteErrorStatusEvent(unsigned short currentError)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/14
//
//!  This function writes LP RSSI Update event
//
//------------------------------------------------------------------------------
void EventLogWriteErrorStatusEvent(
                                     unsigned short currentError                //!< current error                         
                                  )
{
   //For indexing the loop
   unsigned char loopIndex = 0;
   //For event length
   unsigned char eventLength = 0u;
   //For checking the data length
   unsigned char dataLength = 0u;
   //Date/time structure
   DATE_TIME_STRUCT currentDateTime;
   // Get the Updated real time
   RTCGetCurrentDateTime(&currentDateTime);
   //Save the event Index byte
   eventLogWriteArray[ramArrayIndex] = (unsigned char) EVENTLOG_ID_ERROR_STATUS_EVENT;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current month
   eventLogWriteArray[ramArrayIndex] = currentDateTime.monthId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current day
   eventLogWriteArray[ramArrayIndex] = currentDateTime.dayId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current year
   eventLogWriteArray[ramArrayIndex] = currentDateTime.yearId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current hour
   eventLogWriteArray[ramArrayIndex] = currentDateTime.hourId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current minutes
   eventLogWriteArray[ramArrayIndex] = currentDateTime.minId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the current seconds
   eventLogWriteArray[ramArrayIndex] = currentDateTime.secondId;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Get the event length
   eventLength = GetEventLength(EVENTLOG_ID_ERROR_STATUS_EVENT);
   //Save the event length
   eventLogWriteArray[ramArrayIndex] = eventLength;
   //Increment in arrayIndex
   ramArrayIndex = ramArrayIndex + 1;
   //Increment in loopIndex
   loopIndex = loopIndex + 1;
   //Save the instrument GPS Data
   while ( dataLength < 2 )
   {
      eventLogWriteArray[ramArrayIndex] = 0xBC; //TODO: add LP's current error
      //Increment in arrayIndex
      ramArrayIndex = ramArrayIndex + 1;
      //Increment in loopIndex
      loopIndex = loopIndex + 1;
      //Increment in data length index
      dataLength = dataLength+1;
   }
   //Fill the reminder of the event with zeroes
   while( loopIndex < ONE_EVENT_SIZE )
   {
       eventLogWriteArray[ramArrayIndex] = 0x00;
       //Increment in arrayIndex
       ramArrayIndex = ramArrayIndex + 1;
       //Increment in loopIndex
       loopIndex = loopIndex + 1;
   }
   //increment in event log counter
    eventCount++;
   //Check if the array has reached its limits
   if ( ramArrayIndex >=  EVENT_LOG_WRITE_ARRAY_LENGTH )
   {
      CommitBufferToDataflash();
   }   
}
//------------------------------------------------------------------------------
//   EventLogGetNumberOfEvents(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/19
//
//!  This function returns the current number of events to be sent to 
//! iNet server
//------------------------------------------------------------------------------
unsigned int EventLogGetNumberOfEvents(void)
{
   //Return the current number of events
   return eventCount;
}
//------------------------------------------------------------------------------
//   EventLogShutDown(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/19
//
//!  This function handles the error log with respect to the system shutdown
//
//------------------------------------------------------------------------------
void EventLogShutDown(void)
{
    bool isWriteCorrect = false;
   //Commit the available buffer to dataflash
   CommitBufferToDataflash();
   //Save the current event count in the EEPROM
   isWriteCorrect = TM4CEEPROMWriteData(&eventCount,(unsigned char) EVENT_COUNT_BLOCK_INTERNAL_EEPROM, (unsigned char) EVENT_COUNT_WORD_INTERNAL_EEPROM, (unsigned char) 1 );
   
   isEventLogInit = false;
   
}
//==============================================================================
//  End Of File
//==============================================================================