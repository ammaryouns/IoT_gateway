//==============================================================================
//
//  EventLog.h
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
//  Source:        EventLog.h
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
//! This file declares the global functions and constant of the module
//! these functions are used to handle different events of Morrison
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//  Revision: 1.0  2016/12/14  Ali Zulqarnain Anjum
//      Initial version
//
//==============================================================================

#ifndef __EVENTLOG_H__
#define __EVENTLOG_H__

//==============================================================================
//  INCLUDES
//==============================================================================

#include <stdbool.h>

//==============================================================================
//  GLOBAL CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

#define FRAMING_CHARACTER_LENGTH 2                                              //!< Length of framing characters
#define DEVICE_ID_LENGTH 16                                                     //!< Length of device ID
#define USER_SITE_NAME_LENGTH 16                                                //!< Length of user and site name
#define TOTAL_NUMBER_OF_SENSORS 7                                               //!< Total Number of sensors (maximum 7 for Zappa)
#define SENSOR_READING_LENGTH 2                                                 //!< Sensor reading length
#define TIME_STAMP_LENGTH 6                                                     //!< Time Stamp Length
#define EVENT_LOG_WRITE_ARRAY_LENGTH 4096                                       //!< Write Array Length of event log (4k as index starts from zero)
#define EVENT_LOG_READ_ARRAY_LENGTH  4096                                       //!< READ Array Length of event log
#define RESERVED_FOR_FUTURE_USE      37                                         //!< Bytes Reserved for future use
#define NO_PEER_NUMBER    -1                                                    //!< Dummy for no peer number
#define ONE_EVENT_SIZE 128                                                      //!< Size for one event

//==============================================================================
//  GLOBAL DATA STRUCTURES DEFINITION
//==============================================================================
//Enumeration got event type
typedef enum
{
   EVENT_TYPE_ENUM_INTERNAL = 0,
   EVENT_TYPE_ENUM_PEER = 1,
}EVENT_TYPE_ENUM;
//structure of the sensor status to be saved in event log
typedef struct
{
   unsigned char sensorType;                                                    //!< Sensor Type
   unsigned char sensorUnits;                                                   //!< Sensor Units
   unsigned char senorStatusAndDecimal;                                         //!< Sensor Status and decimal Places
   unsigned char sensorReading[SENSOR_READING_LENGTH];                          //!< Sensor Reading
   
}SENSOR_STATUS_STRUCT;  
//Structure for entry in the event log
typedef struct 
{
   unsigned char eventID;                                                       //!< event ID
   unsigned char timeStamp[TIME_STAMP_LENGTH];                                  //!< Time for the event
   unsigned char eventLength;                                                   //!< event Length
   unsigned char deviceID[DEVICE_ID_LENGTH];                                    //!< Device ID
   unsigned char userName[USER_SITE_NAME_LENGTH];                               //!< User Name
   unsigned char siteName[USER_SITE_NAME_LENGTH];                               //!< site Name
   SENSOR_STATUS_STRUCT sensorStatus[TOTAL_NUMBER_OF_SENSORS];                  //!< Sensor Status
   unsigned char spareForFuture[37];                                             //!< Reserved for future use
}EVENT_LOG_ENTRY_STRUCT;
//Enumeration for the Event IDs
typedef enum
{
   EVENTLOG_ID_NO_EVENT = 0u,                                                   //!< No event
   EVENTLOG_ID_INST_LOST_EVENT = 1u,                                            //!< Instrument lost event
   EVENTLOG_ID_LPONLINE_EVENT = 2u,                                             //!< Morrison online event
   EVENTLOG_ID_LPOFFLINE_EVENT = 3u,                                            //!< Morrison offline event
   EVENTLOG_ID_LB_CREATE_EVENT = 4u,                                            //!< Morrison low battery event
   EVENTLOG_ID_NO_LB_EVENT = 5u,                                                //!< Morrison low battery clear event
   EVENTLOG_ID_GPS_UPDATE_EVENT = 6u,                                           //!< Morrison GPS Location update event
   EVENTLOG_ID_LP_KEEPALIVE_EVENT = 7u,                                         //!< Morrison keep alive event
   EVENTLOG_ID_SITE_UPDATE_EVENT = 8u,                                          //!< Morrison site update event
   EVENTLOG_ID_IMEI_UPDATE_EVENT = 9u,                                          //!< Morrison IMEI update event
   EVENTLOG_ID_CELL_RSSI_EVENT = 10u,                                           //!< Morrison cell tower RSSI update event
   EVENTLOG_ID_ERROR_STATUS_EVENT = 11u,                                        //!< Morrison error Status event
   EVENTLOG_ID_LEAVE_GROUP_EVENT = 12u,                                         //!< Morrison leave group event
   EVENTLOG_ID_JOIN_GROUP_EVENT = 13u,                                          //!< Morrison join group event
   EVENTLOG_ID_MAN_DOWN_EVENT = 14u,                                            //!< Morrison man down event
   EVENTLOG_ID_MANDOWN_CLEAR_EVENT = 15u,                                       //!< Morrison man down clear event
   EVENTLOG_ID_USER_UPDATE_EVENT = 16u,                                         //!< Morrison user update event
   EVENTLOG_ID_PANIC_ALARM_EVENT = 17u,                                         //!< Morrison panic alarm event
   EVENTLOG_ID_PANIC_CLEAR_EVENT = 18u,                                         //!< Morrison panic alarm clear event
   EVENTLOG_ID_SENSOR_UPDATE_EVENT = 19u,                                       //!< Morrison sensor update event
   EVENTLOG_ID_PUMP_ERROR_EVENT = 20u,                                          //!< Morrison pump error event
   EVENTLOG_ID_PUMP_READY_EVENT = 21u,                                          //!< Morrison pump error clear event
   EVENTLOG_ID_HIGH_ALARM_EVENT = 22u,                                          //!< Morrison high gas alarm event
   EVENTLOG_ID_LOW_ALARM_EVENT = 23u,                                           //!< Morrison low gas alarm event
   EVENTLOG_ID_STEL_ALARM_EVENT = 24u,                                          //!< Morrison STEL gas alarm event
   EVENTLOG_ID_TWA_ALARM_EVENT = 25u,                                           //!< Morrison twa gas alarm event
   EVENTLOG_ID_GAS_ALARM_CLEAR_EVENT = 26u,                                     //!< Morrison gas alarm clear event
}EVENTLOG_ID_ENUM;
//
//==============================================================================
//  GLOBAL DATA
//==============================================================================

extern unsigned char eventLogWriteArray[EVENT_LOG_WRITE_ARRAY_LENGTH];       //!< Array for storing the event log in the RAM
extern unsigned char eventLogReadArray[EVENT_LOG_READ_ARRAY_LENGTH];            //!< Array for reading the event log
extern unsigned int subsectorNumber;

//==============================================================================
//  EXTERNAL OR GLOBAL FUNCTIONS
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
void EventLogInit();
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
                                     );
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
                                 );
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
                              );
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
                                   );
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
                            );
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
                                 );
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
                           );
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
                                );
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
                                  EVENTLOG_ID_ENUM eventID,                      //!< event ID
                                  unsigned short peerNumber                     //!< Peer Number
                               );
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
                                    );
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
                                     );
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
                                  );
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
                                    );
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
                               );
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
                                );
//------------------------------------------------------------------------------
//   EventLogWriteLPSiteUpdateEvent(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/14
//
//!  This function writes LP site update event
//
//------------------------------------------------------------------------------
void EventLogWriteLPSiteUpdateEvent(void);
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
                             );
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
                                 );
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
                                    unsigned short RSSI                         //!< RSSI Infor                         
                                 );
//------------------------------------------------------------------------------
//   EventLogWriteErrorStatusEvent(unsigned short currentError)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/14
//
//!  This function writes error status Update event
//
//------------------------------------------------------------------------------
void EventLogWriteErrorStatusEvent(
                                     unsigned short currentError                //!< current error                         
                                  );
//------------------------------------------------------------------------------
//   EventLogGetNumberOfEvents(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/19
//
//!  This function returns the current number of events to be sent to 
//! iNet server
//------------------------------------------------------------------------------
unsigned int EventLogGetNumberOfEvents(void);
//------------------------------------------------------------------------------
//   EventLogShutDown(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/19
//
//!  This function handles the error log with respect to the system shutdown
//
//------------------------------------------------------------------------------
void EventLogShutDown(void);

#endif /* __ERRORLOG_H__ */
//==============================================================================
//  End Of File
//==============================================================================