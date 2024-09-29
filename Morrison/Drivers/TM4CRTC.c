//==============================================================================
//
//  TM4CRTC.c
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
//  Source:        TM4CRTC.c
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
//! This module contains global and local functions for RTC.
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//   Revision: 1.0    2016/11/09  Ali Zulqarnain Anjum
//
//==============================================================================
//  INCLUDES 
//==============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "inc/hw_gpio.h"
#include "inc/hw_hibernate.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/hibernate.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "TM4CRTC.h"

//==============================================================================
//   CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================
      
#define MONTHS_PER_YEAR                  12u          //!< This define is used as days per month array length
#define MAX_VALID_SECONDS               59U           //!< Maximum value of seconds   
#define MAX_VALID_MINUTES               59U           //!< Maximum value of minutes
#define MAX_VALID_HOURS                 23U           //!< Maximum value of hours
#define MAX_VALID_DATE                  31U           //!< Maximum value of date
#define MAX_VALID_MONTHS                12U           //!< Maximum value of months
#define MAX_VALID_YEARS                 99U           //!< Maximum value of years
#define MIN_VALID_YEARS                 14U           //!< Minimum value of years
#define SECONDS_PER_MINUTE               60u             //!< Define for seconds in a minutes
#define MINUTES_PER_HOUR                 60u             //!< Define for minutes in a hour   
#define HOURS_PER_DAY                    24u             //!< Define for hours in a day
#define DAYS_PER_YEAR                    365u            //!< Define for Days in a year
#define DAYS_PER_MONTH                   30u             //!< Define for days in a month   
#define SECONDS_PER_HOUR    (SECONDS_PER_MINUTE * MINUTES_PER_HOUR)                             //!< Define for seconds in a hour
#define SECONDS_PER_DAY     ((unsigned long)HOURS_PER_DAY * (unsigned long)SECONDS_PER_HOUR)    //!< Define for seconds in a day

//==============================================================================
//    LOCAL DATA STRUCTURE DEFINITION
//==============================================================================

//==============================================================================
//    GLOBAL DATA DECLARATIONS
//==============================================================================

bool isDateTimeSet = false;                                                     //!< TODO: Make Boolean afterwards Flag for checking if the date and time is set and are valid                                              
uint32_t ui32SysClock = 0u;                                                     //!< Variable for system clock TODO: Remove it afterwards
const unsigned char daysPerMonth[MONTHS_PER_YEAR+1U] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};  //!< Array for number of days per month

//==============================================================================
//    LOCAL DATA DECLARATIONS
//==============================================================================

static DATE_TIME_STRUCT currentDateTime;

//==============================================================================
//    LOCAL FUNCTION PROTOTYPES
//==============================================================================

void DateTimeSet(void);                                                         //!< Set the time and date
bool DateTimeUpdateGet(void);                                                   //!< Get the update values of date and time

//==============================================================================
//    LOCAL FUNCTIONS IMPLEMENTATION
//==============================================================================
//------------------------------------------------------------------------------
//   bool DateTimeGet(struct tm *sTime)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/15
//
//! This function reads the current date and time from the calendar logic of the
//! hibernate module.  Return status indicates the validity of the data read.
//! If the received data is valid, the 24-hour time format is converted to
//! 12-hour format.
//  
//------------------------------------------------------------------------------
bool DateTimeGet(struct tm *sTime)
{
  //TODO: use BOOLEAN
    bool isDataValid = false;
    // Get the latest time.
    HibernateCalendarGet(sTime);
    // Is valid data read?
    if(((sTime->tm_sec < 0) || (sTime->tm_sec > 59)) ||
       ((sTime->tm_min < 0) || (sTime->tm_min > 59)) ||
       ((sTime->tm_hour < 0) || (sTime->tm_hour > 23)) ||
       ((sTime->tm_mday < 1) || (sTime->tm_mday > 31)) ||
       ((sTime->tm_mon < 0) || (sTime->tm_mon > 11)) ||
       ((sTime->tm_year < 100) || (sTime->tm_year > 199)))
    {
        // No - Let the application know the same by returning relevant
        // message.
        isDataValid =  false;
    }
    else
    {
       // Return that new data is available so that it can be displayed
       isDataValid = true;
    }
    return isDataValid;
}
//------------------------------------------------------------------------------
//   void DateTimeSet(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/15
//
//!  This function writes the requested date and time to the calendar logic of
//!  hibernation module.
//  
//------------------------------------------------------------------------------
void DateTimeSet(void)
{
    struct tm sTime;
    // Get the latest date and time.  This is done here so that unchanged
    // parts of date and time can be written back as is.
    HibernateCalendarGet(&sTime);
    // Set the date and time values that are to be updated.
    sTime.tm_sec = (int) currentDateTime.secondId;
    sTime.tm_hour = (int) currentDateTime.hourId;
    sTime.tm_min = (int) currentDateTime.minId;
    sTime.tm_mon = (int) currentDateTime.monthId;
    sTime.tm_mday = (int) currentDateTime.dayId;
    sTime.tm_year = (int) (100 + currentDateTime.yearId);
    // Update the calendar logic of hibernation module with the requested data.
    HibernateCalendarSet(&sTime);
}

//------------------------------------------------------------------------------
//   bool DateTimeUpdateGet(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/15
//
//!  This function updates individual buffers with valid date and time to be
//!  displayed on the date screen so that the date and time can be updated.
//  
//------------------------------------------------------------------------------
bool DateTimeUpdateGet(void)
{
    struct tm sTime;
    //TODO: Make it boolean
    bool isUpdateRequired = false;
    //TODO: Make it boolean
    bool isDateTimeCorrect = true;
    // Get the latest date and time and check the validity.
    if(DateTimeGet(&sTime) == false)
    {
        // Invalid - Return here with false as no information to update.  So
        // use default values.
        RTCDateTimeDefaultSet();
        isDateTimeCorrect =  false;
    }
    else
    {
        isDateTimeCorrect =  true;
    }
    // If date and time is valid, copy the date and time values into respective
    // indexes
    if ( isDateTimeCorrect == true )
    {
       currentDateTime.secondId = (unsigned char) sTime.tm_sec;
       currentDateTime.monthId = (unsigned char) sTime.tm_mon;
       currentDateTime.dayId = (unsigned char) sTime.tm_mday;
       currentDateTime.yearId = (unsigned short) (sTime.tm_year - 100);
       currentDateTime.hourId = (unsigned char) sTime.tm_hour;
       currentDateTime.minId = (unsigned char) sTime.tm_min;
       // If the month is February
       if ( currentDateTime.monthId == 2 )
       {
           // If leap year date extends 29
           if( ((currentDateTime.yearId % 4) == 0) && (currentDateTime.dayId > 29) )
           {
                //Reset the date to 1
                currentDateTime.dayId = 1;
                //Increment the month
                currentDateTime.monthId = currentDateTime.monthId + 1;
                isUpdateRequired = true;
           }
           //for non leap year if date extends 28
           else if ( ((currentDateTime.yearId % 4) != 0) && (currentDateTime.dayId > 28) )
           {
               //Reset the date to 1
               currentDateTime.dayId = 1;
               //Increment the month
               currentDateTime.monthId = currentDateTime.monthId + 1;
               isUpdateRequired = true;
           }
           else 
           {
               //Do nothing
           }
       }
       // For April, Juna, September and November make sure that 
       // Date doesn't exceeds 30
       else if ( ((currentDateTime.monthId == 4) || (currentDateTime.monthId == 6) || \
                 (currentDateTime.monthId == 9) || (currentDateTime.monthId == 11)) && \
                 (currentDateTime.dayId > 30) )
       {
            //Reset the date to 1
            currentDateTime.dayId = 1;
            //Increment the month
            currentDateTime.monthId = currentDateTime.monthId + 1;
            isUpdateRequired = true;
       }
       else
       {
           //Do nothing
       }
                
    }
    else 
    {
       //Do nothing
    }
    if ( isUpdateRequired == true )
    {
       // Set the new date time
       DateTimeSet();
    }
    else
    {
       //Do nothing
    }
    // Return true to indicate new information has been updated
    return isDateTimeCorrect;
}

//==============================================================================
//    GLOBAL FUNCTIONS IMPLEMENTATION
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
void RTCSetup(void)
{
  //Test Code: Remove it afterwards
    unsigned int ui32Status;
     ui32Status = HibernateIsActive();
     if (HibernateIsActive())
     {
        HibernateDisable();
     }

    // Enable the hibernate module.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
    // Wait for enabling of the port
    SysCtlDelay(10);   
    // Configure the module clock source.
    HibernateClockConfig(HIBERNATE_OSC_DISABLE | HIBERNATE_OSC_LOWDRIVE);
    // Wait for enabling of the port
    SysCtlDelay(10);   
    HibernateEnableExpClk(SysCtlClockGet()); 
    // Load the value
    HibernateRTCSet(0);
    // Trim the RTC
    HibernateRTCTrimSet(0x7FFF);
    // Enable RTC mode.
    HibernateRTCEnable();
    // Configure the hibernate module counter to 24-hour calendar mode.
    HibernateCounterMode(HIBERNATE_COUNTER_24HR);
    // If hibernation count is very large, it may be that there was already
    // a value in the hibernate memory, so reset the count.
    //ui32HibernateCount = (ui32HibernateCount > 10000) ? 0 : ui32HibernateCount;
}
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
                                              )
{
    // This variable is used to contains the total seconds
    unsigned long totalSeconds = 0u;
    // This variable stores the days Total days calculated since 2000 
    unsigned long totalDays = 0u;
    // This variable is Used as Index of for loop
    unsigned char monthCounter = 1u;
    // Get the current date and time
    (void) DateTimeUpdateGet();
    // Calculate days of years
    totalDays = currentDateTime.dayId + ((unsigned long)( 16 - currentDateTime.yearId) * DAYS_PER_YEAR ) - 1u; 
    totalDays = totalDays + (((unsigned long)( 16 - currentDateTime.yearId) + 3u)/4u);
    // Calculate total days of the months
    for( monthCounter = 1u; monthCounter < currentDateTime.monthId; monthCounter++ )
    {
        // Leap year is considered if month is February
        if( ((currentDateTime.yearId % 4u) == 0u) && (monthCounter == 2u) )
        {   
            // Special case for days in a leap-year February
            totalDays = totalDays + (unsigned long) 29;
        }
        else
        {
            // Add whole months, taking into account days per month if not leap year 
            totalDays = totalDays + (unsigned long) daysPerMonth[monthCounter];
        }
    }
    totalSeconds = totalDays * SECONDS_PER_DAY; 
    // Calculate total seconds since 2000 for the hours
    totalSeconds = totalSeconds + (unsigned long)currentDateTime.hourId * SECONDS_PER_HOUR;
    // Calculate total seconds since 2000 for the minutes
    totalSeconds = totalSeconds + (unsigned long)currentDateTime.minId * SECONDS_PER_MINUTE;
    totalSeconds = totalSeconds + currentDateTime.secondId;
    
    *secondsSince2016 = totalSeconds;
}
//------------------------------------------------------------------------------
//   void RTCDateTimeDefaultSet(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/15
//
//!  This function sets the time to the default system time.
//  
//------------------------------------------------------------------------------

void RTCDateTimeDefaultSet(void)
{
    // Reset to default settings
    currentDateTime.monthId = 2;
    currentDateTime.dayId = 25;
    currentDateTime.yearId = 16;
    currentDateTime.hourId = 23;
    currentDateTime.minId = 59;
    currentDateTime.secondId = 0;
    //Save RTC Settings
    DateTimeSet();
}

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
                           )
{
    // Get the updated date and time
    (void) DateTimeUpdateGet();
    // Send the updated value
    dateTime->monthId = currentDateTime.monthId;
    dateTime->dayId = currentDateTime.dayId;
    dateTime->yearId = currentDateTime.yearId;
    dateTime->hourId = currentDateTime.hourId;
    dateTime->minId = currentDateTime.minId;
    dateTime->secondId = currentDateTime.secondId;
    
}
                      
//==============================================================================
//  End Of File
//==============================================================================

