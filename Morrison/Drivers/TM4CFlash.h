//==============================================================================
//
//  TM4CFlash.h
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
//  Source:        TM4CFlash.h
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
//! The header file contains the functions required for reading and writing the 
//! the internal flash
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//  Revision: 1.0  2016/11/24  Ali Zulqarnain Anjum
//      Initial Revision 
//
//==============================================================================

#ifndef __TM4CFLASH_H__
#define __TM4CFLASH_H__

//==============================================================================
//  INCLUDES 
//==============================================================================

#include <stdbool.h>
#include <stdint.h>

//==============================================================================
//  GLOBAL CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

//==============================================================================
//  GLOBAL DATA STRUCTURES DEFINITION
//==============================================================================

//==============================================================================
//  GLOBAL DATA
//==============================================================================

//==============================================================================
//  EXTERNAL OR GLOBAL FUNCTIONS
//==============================================================================

//------------------------------------------------------------------------------
//   FlashWrite(unsigned long byteStartAddress, unsigned int * puiBuff, , unsigned short length)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/24
//
//!  This function 
//
//------------------------------------------------------------------------------
bool FlashWrite(unsigned int *data, unsigned long startAdress, unsigned long dataSize);

//------------------------------------------------------------------------------
//   FlashEraseMemoryRegion(unsigned long startAddr)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/24
//
//! This function erases the microprocessor's Flash memory with specified address
//
//------------------------------------------------------------------------------
bool FlashEraseMemoryRegion(unsigned long startAddr);

//------------------------------------------------------------------------------
//   TM4CFlashCrc(unsigned char *src,int len )
//
//   Author:  Ali Zulqarnain Anjum
//   Date:    2016/11/24
//
//!  This function calculates crc for complete application code.
//
//------------------------------------------------------------------------------

unsigned short TM4CFlashCrc(
                        unsigned char *src,
                        int len 
                        );

#endif /* __TM4CFLASH_H__ */
//==============================================================================
//  End Of File
//==============================================================================