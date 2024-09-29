//==============================================================================
//
//  TM4CEEPROM.h
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
//  Source:        TM4CEEPROM.h
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
//! The header file contains the functions required for reading and writing the 
//! the TM4C EEPROM
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//  Revision: 1.0  2016/11/28  Ali Zulqarnain Anjum
//      Initial Revision 
//
//==============================================================================

#ifndef __TM4CEEPROM_H__
#define __TM4CEEPROM_H__

//==============================================================================
//  INCLUDES 
//==============================================================================

#include "TM4CFlash.h"

//==============================================================================
//  GLOBAL CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

#define BLOCK_NUMBER_OF_WORDS    16                                             //!< 16 words in a block
#define FIRST_BLOCK    0                                                        //!< Block number will be start from 0
#define LAST_BLOCK     95                                                       //!< Total of 96 blocks in the EERPOM
#define FIRST_WORD     0                                                        //!< Block number will be start from 0
#define LAST_WORD      15                                                       //!< Total of 16 words in one block of EEPROM
#define TOTAL_NUMBER_OF_WORDS   1536                                            //!< Total Number of words in the EEPROM


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
//   TM4CEEPROMInit(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/28
//
//!  This function initializes the EEPROM and returns the status whether the 
//!  EEOROM is initialized properly or not
//------------------------------------------------------------------------------
bool TM4CEEPROMInit(void);
//------------------------------------------------------------------------------
//   TM4CEEPROMGetSize(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/28
//
//!  This function returns the EEPROM size in number of bytes
//------------------------------------------------------------------------------
unsigned int TM4CEEPROMGetSise(void);
//------------------------------------------------------------------------------
//   TM4CEEPROMGetBlockCount(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/28
//
//!  This function returns the number of EEPROM blocks
//------------------------------------------------------------------------------
unsigned int TM4CEEPROMGetBlockCount(void);
//------------------------------------------------------------------------------
//   TM4CEEPROMReadData(unsigned int *data, unsigned char blockNumber, unsigned char startWordNumber, unsigned char numberOfWords)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/28
//
//!  This function is used to read the data from the TM4C EEPROM
//------------------------------------------------------------------------------
bool TM4CEEPROMReadData(unsigned int *data, unsigned char blockNumber, unsigned char startWordNumber, unsigned char numberOfWords);
//------------------------------------------------------------------------------
//   TM4CEEPROMWriteData(unsigned int *data, unsigned char blockNumber, unsigned char startWordNumber, unsigned char numberOfWords)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/28
//
//!  This function is used to write the data in the TM4C EEPROM
//------------------------------------------------------------------------------
bool TM4CEEPROMWriteData(unsigned int *data, unsigned char blockNumber, unsigned char startWordNumber, unsigned char numberOfWords);
//------------------------------------------------------------------------------
//   TM4CEEPROMErase(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/11/28
//
//!  This function takes the contents of EEPROM to factory state
//------------------------------------------------------------------------------
bool TM4CEEPROMErase(void);

#endif /* __TM4CEEPROM_H__ */
//==============================================================================
//  End Of File
//==============================================================================