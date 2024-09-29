//==============================================================================
//
//  TM4CFlash.c
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
//  Source:        TM4CFlash.c
//
//  Project:       Morrison
//
//  Author:        Ali Zulqarnain Anjum
//
//  Date:          2016/11/24
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This module contains global and local functions for reading and writing 
//! and writing the internal flash
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//   Revision: 1.0    2016/11/24  Ali Zulqarnain Anjum
//
//==============================================================================
//  INCLUDES 
//==============================================================================

#include "TM4CFlash.h"
#include "driverlib/flash.h"

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
//   FlashEraseMemoryRegion()
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/24
//
//! This function erases the microprocessor's Flash memory with specified address
//
//------------------------------------------------------------------------------
bool FlashEraseMemoryRegion(unsigned long startAddr)
{
    // Current Status of flash protection
    tFlashProtection currentFlashProtection;
    // To check if the Memory region is erased successfully
    bool isErasedSuccessfully = false;
    // To check if we can erase the region
    bool canBankErased = false;
    // To get the staus of flash erase
    signed int eraseStatus = -2;
    // First get the protection level of the flash
    currentFlashProtection = FlashProtectGet(startAddr);
    // Check if the region is write protected
    if ( (currentFlashProtection == FlashReadOnly) || (currentFlashProtection == FlashExecuteOnly) )
    {
        canBankErased = false;
    }
    else
    {
        canBankErased = true;
    }
    //Only Go for the erase if the bank can be erased
    if ( canBankErased == true )
    {
        eraseStatus = FlashErase(startAddr);
    }
    else
    {
       //Do nothing
    }
    if( eraseStatus == 0 )
    {
        isErasedSuccessfully = true;
    }
    else
    {
        isErasedSuccessfully = false;
    }
    return isErasedSuccessfully;
}

////------------------------------------------------------------------------------
////   FlashCrc(unsigned char *src,int len )
////
////   Author:   Ali Zulqarnain Anjum
////   Date:     2016/11/24
////
////!  This function calculates crc for complete application code.
////
////------------------------------------------------------------------------------
//
//unsigned short FlashCrc(
//                        unsigned char *src,
//                        int len 
//                       )
//{	 
//    int i;
//    int index;
//    int crc_Low = 0xFF;
//    int crc_High = 0xFF;
//    for (i = 0; i<len; i++ )
//    {
//            index = crc_High ^ (*(src+i));
//            crc_High = crc_Low ^ crcTable[index];
//            crc_Low = (unsigned char)crcTable[index+ 256];    
//            
//    }
//    return crc_Low*256+crc_High; 
//}

//------------------------------------------------------------------------------
//   FlashWrite(unsigned long byteStartAddress, unsigned int * puiBuff, , unsigned short length)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/24
//
//!  This function 
//
//------------------------------------------------------------------------------
bool FlashWrite(unsigned int *data, unsigned long startAdress, unsigned long dataSize)
{    
   // To check if the write was successful
   bool isWriteSuccessful = false;
   // To check if the current flash protection state
   tFlashProtection currentFlashProtection;
   // To check if the write data is possible
   bool isDataWriteAllowed = false;
   // The write Status
   signed int writeStatus = -2;
   // Data size should be a multiple of 4
   if ( (dataSize%4) == 0 )
   {
      isDataWriteAllowed = true;
   }
   else
   {
      isDataWriteAllowed = false;
   }
   // check if protection state of dataflash
   if ( isDataWriteAllowed == true )
   {
      // First get the protection level of the flash
      currentFlashProtection = FlashProtectGet(startAdress);
      if ( (currentFlashProtection == FlashReadOnly) || (currentFlashProtection == FlashExecuteOnly) )
      {
          isDataWriteAllowed = false;
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
   //Write the data in the flash if the write is allowed
   if ( isDataWriteAllowed == true )
   {
       writeStatus = FlashProgram( data, startAdress, dataSize );
   }
   else
   {
      writeStatus = -1;
   }
   // Set the write success status
   if ( writeStatus == 0 )
   {
      isWriteSuccessful = true;
   }
   else
   {
      isWriteSuccessful = false;
   }
   return isWriteSuccessful;
}

//==============================================================================
//  END OF FILE
//==============================================================================

