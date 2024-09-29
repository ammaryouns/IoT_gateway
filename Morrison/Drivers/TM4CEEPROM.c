//==============================================================================
//
//  TM4CEEPROM.c
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
//  Source:        TM4CEEPROM.c
//
//  Project:       Morrison
//
//  Author:        Ali Zulqarnain Anjum
//
//  Date:          2016/11/28
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This module contains global and local functions for reading and writing 
//! and writing the TM4C EEPROM
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//   Revision: 1.0    2016/11/24  Ali Zulqarnain Anjum
//
//==============================================================================
//  INCLUDES 
//==============================================================================

#include "TM4CEEPROM.h"
#include "driverlib/eeprom.h"
#include "driverlib/sysctl.h"

//==============================================================================
//    CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

//==============================================================================
//  LOCAL DATA STRUCTURE DEFINITION
//==============================================================================

//==============================================================================
//  GLOBAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
//  LOCAL DATA DECLARATIONS
//==============================================================================
               

//==============================================================================
//  LOCAL FUNCTION PROTOTYPES
//==============================================================================

//==============================================================================
//    LOCAL FUNCTION IMPLIMENTATION
//==============================================================================

//==============================================================================
//  GLOBAL FUNCTIONS IMPLEMENTATION
//==============================================================================
//------------------------------------------------------------------------------
//   TM4CEEPROMInit(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/28
//
//!  This function initializes the EEPROM and returns the status whether the 
//!  EEOROM is initialized properly or not
//------------------------------------------------------------------------------

bool TM4CEEPROMInit(void)
{
    //To return the staus
    bool initStatusReturn = false;
    //To get the status from the EEPROM initialize
    unsigned int initStusGet = false;
    // EEPROM activate
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0); 
    // Intialize the eeprom
    initStusGet = EEPROMInit();
    // Check the initialize status and set return status accordingly
    if ( initStusGet == EEPROM_INIT_OK )
    {
         initStatusReturn = true;
    }
    else
    {
         initStatusReturn = false;
    }
    return initStatusReturn;
}
//------------------------------------------------------------------------------
//   TM4CEEPROMGetSize(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/28
//
//!  This function returns the EEPROM size in number of bytes
//------------------------------------------------------------------------------
unsigned int TM4CEEPROMGetSise(void)
{
    //For the EEPROM size
    unsigned int sieEEPROM = 0u;
    //Get the size of EEPROM
    sieEEPROM = EEPROMSizeGet();
    //Return the size
    return sieEEPROM;
}
//------------------------------------------------------------------------------
//   TM4CEEPROMGetBlockCount(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/28
//
//!  This function returns the number of EEPROM blocks
//------------------------------------------------------------------------------
unsigned int TM4CEEPROMGetBlockCount(void)
{
    //For the number of blocks
    unsigned int numberOfBlocks = 0u;
    //Get the number of blocks
    numberOfBlocks = EEPROMBlockCountGet();
    //Return the number of blocks
    return numberOfBlocks;
}
//------------------------------------------------------------------------------
//   TM4CEEPROMReadData(unsigned int *data, unsigned char blockNumber, unsigned char startWordNumber, unsigned char numberOfWords)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/28
//
//!  This function is used to read the data from the TM4C EEPROM
//------------------------------------------------------------------------------
bool TM4CEEPROMReadData(unsigned int *data, unsigned char blockNumber, unsigned char startWordNumber, unsigned char numberOfWords)
{
    //For checking the data
    bool isDataCorrect = false;
    //For number of bytes
    unsigned int numberOfBytes = 0u;
    //For calcultating the address
    unsigned int startAddressCalculated = 0u;
    //Check the correctness of given data
    if (  (blockNumber >= FIRST_BLOCK) && (blockNumber <= LAST_BLOCK) &&\
          (startWordNumber >= FIRST_WORD) && (startWordNumber <= LAST_WORD) &&\
          (numberOfWords <= TOTAL_NUMBER_OF_WORDS) )
    {
        isDataCorrect = true;
    }
    else
    {
       //Do nothing
    }
    if ( isDataCorrect == true )
    {
        //Get the total number of bytes
        numberOfBytes = numberOfWords * 4;
        //Calculate the address
        startAddressCalculated = (BLOCK_NUMBER_OF_WORDS * blockNumber) + (startWordNumber * 4);
        //Read the data
        EEPROMRead(data, startAddressCalculated, numberOfBytes);
    }
    else
    {
        //Do nothing
    }
    return isDataCorrect;
}
//------------------------------------------------------------------------------
//   TM4CEEPROMWriteData(unsigned int *data, unsigned char blockNumber, unsigned char startWordNumber, unsigned char numberOfWords)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/28
//
//!  This function is used to write the data in the TM4C EEPROM
//------------------------------------------------------------------------------
bool TM4CEEPROMWriteData(unsigned int *data, unsigned char blockNumber, unsigned char startWordNumber, unsigned char numberOfWords)
{
    //For checking the data
    bool isDataCorrect = false;
    //For getting the write status
    unsigned int writeStatus = 0u;
    //For number of bytes
    unsigned int numberOfBytes = 0u;
    //For calcultating the address
    unsigned int startAddressCalculated = 0u;
    //Check the correctness of given data
    if (  (blockNumber >= FIRST_BLOCK) && (blockNumber <= LAST_BLOCK) &&\
          (startWordNumber >= FIRST_WORD) && (startWordNumber <= LAST_WORD) &&\
          (numberOfWords <= TOTAL_NUMBER_OF_WORDS) )
    {
        isDataCorrect = true;
    }
    else
    {
       //Do nothing
    }
    if ( isDataCorrect == true )
    {
        //Get the total number of bytes
        numberOfBytes = numberOfWords * 4;
        //Calculate the address
        startAddressCalculated = (BLOCK_NUMBER_OF_WORDS * blockNumber) + (startWordNumber * 4);
        //Read the data
        writeStatus = EEPROMProgram(data, startAddressCalculated, numberOfBytes);
    }
    else
    {
        //Do nothing
    }
    if ( writeStatus == 0 )
    {
       isDataCorrect = false;
    }
    else
    {
       //Do nothing
    }
    return isDataCorrect;
}
//------------------------------------------------------------------------------
//   TM4CEEPROMErase(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/28
//
//!  This function takes the contents of EEPROM to factory state
//------------------------------------------------------------------------------
bool TM4CEEPROMErase(void)
{
   //To return the status
   bool eraseStatusReturn = false;
   //To get the status
   unsigned int eraseStatsGet = 0u;
   // Erase the EEPROM
   eraseStatsGet = EEPROMMassErase();
   //Set the return status
   if ( eraseStatsGet == 0 )
   { 
      eraseStatusReturn = false;
   }
   else
   {
      eraseStatusReturn = true;
   }
   //Return the status
   return eraseStatusReturn;
}
//==============================================================================
//  END OF FILE
//==============================================================================

