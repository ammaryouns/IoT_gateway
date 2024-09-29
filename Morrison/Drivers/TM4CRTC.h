//==============================================================================
//
//  TM4CRTC.h
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
//  Source:        TM4CRTC.h
//
//  Project:       Morrison
//
//  Author:        Ali Zulqarnain Anjum
//
//  Date:          2016/11/09
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! The header file contains the functions required for managing RTC
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//  Revision: 1.0  2016/11/09  Ali Zulqarnain Anjum
//      Initial Revision 
//
//==============================================================================

#ifndef __RTC_H__
#define __RTC_H__

//==============================================================================
//  INCLUDES 
//==============================================================================

#include <stdint.h>

//==============================================================================
//  GLOBAL CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

//==============================================================================
//  GLOBAL DATA STRUCTURES DEFINITION
//==============================================================================
typedef struct
{
   unsigned char monthId;                                                       //!< Current Month
   unsigned char dayId;                                                         //!< Current Date
   unsigned short yearId;                                                       //!< Current Year
   unsigned char hourId;                                                        //!< Current Hour                                              
   unsigned char minId;                                                         //!< Current Minutes
   unsigned char secondId;                                                      //!< Current Year
}DATE_TIME_STRUCT;
//==============================================================================
//  GLOBAL DATA
//==============================================================================

extern bool isDateTimeSet;                                                      //!< TODO: Make Boolean afterwards Flag for checking if the date and time is set and are valid    
extern uint32_t ui32SysClock;                                                   //!< Variable for system clock TODO: Remove it afterwards
extern uint32_t g_ui32SecondIdx;                                                
//==============================================================================
//  EXTERNAL OR GLOBAL FUNCTIONS
//==============================================================================
//------------------------------------------------------------------------------
//   RTCSetup(void)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/15
//
//!  This function sets up the RTC
//  
//------------------------------------------------------------------------------

void RTCSetup(void);

//------------------------------------------------------------------------------
//   void RTCDateTimeDefaultSet(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/15
//
//!  This function sets the time to the default system time.
//  
//------------------------------------------------------------------------------

void RTCDateTimeDefaultSet(void);

//------------------------------------------------------------------------------
//  RTCConvertDateAndTimeIntoSecondsSince2016(unsigned long *secondsSince2016)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2014/10/15
//! \brief
//! \version 1.0
//!  This function takes date [ year, month, day ] and time [ hour, minute, second ] 
//!  and converts them into timestamp in form of seconds since 2000.
//------------------------------------------------------------------------------
void RTCConvertDateAndTimeIntoSecondsSince2016(
                                                  unsigned long *secondsSince2016 //!< nNmber of seconds converted from the date and time 
                                              );
//------------------------------------------------------------------------------
//  RTCGetCurrentDateTime(DATE_TIME_STRUCT *dateTime)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2014/10/15
//! \brief
//! \version 1.0
//!  This function returns the current date and time
//------------------------------------------------------------------------------
void RTCGetCurrentDateTime(
                                DATE_TIME_STRUCT *dateTime                      //!< nNmber of seconds converted from the date and time 
                           );
#endif /* __DATALOGSESSION_H__ */
//==============================================================================
//  End Of File
//==============================================================================