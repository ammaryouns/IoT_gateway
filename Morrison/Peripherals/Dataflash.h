//==============================================================================
//
//  Dataflash.h
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
//  Source:        Dataflash.h
//
//  Project:       Morrison
//
//  Author:        Ali Zulqarnain Anjum
//
//  Date:          2016/12/02
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This file declares the global functions and constants of the DataFlash module. 
//!
//! \par The External Data Flash memory 0x0000 - 0x03FF Range of DataFlash (1024 
//! subsectors, 4 Kbytes each) shall be written or read using this module functions.
//! All communication between microcontroller and Data Flash memory is carried 
//! out using SPI protocol.
//!
//! \par The presence of the Flash memory is also checked before writing or 
//! reading the Flash memory. The page address of the Flash is provided to read 
//! or write the data to the Data Flash memory.
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//  Revision: 1.0    2016/12/02  Ali Zulqarnain Anjum
//      This module is taken from Vaughan 
//
//==============================================================================

#ifndef  __DATAFLASH_H__
#define  __DATAFLASH_H__

//==============================================================================
//  INCLUDES 
//==============================================================================

#include <stdbool.h>
#include "ErrorLog.h"

//==============================================================================
//  GLOBAL CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

#define BITS_PER_BYTE               8u                                          //!< Number of bits in a byte        
#define HIBYTE_WORD16(param)        (unsigned char)((param) >> 8)               //!< Macro to get higher byte (8 MSB bits) from a 16 bit word
#define LOBYTE_WORD16(param)        (unsigned char)(param)                      //!< Macro to get lower byte (8 LSB bits) from a 16 bit word
#define HIWORD_WORD32(param)        (unsigned short)((param) >> 16)             //!< Macro to get higher word (16 MSB bits) from a 32 bit word
#define LOWORD_WORD32(param)        (unsigned short)(param)                     //!< Macro to get lower word (16 LSB bits) from a 32 bit word
#define CLEARBITFIELD(var, field)   ((var) &= ~(field))                         //!< Macro to clear a particular bit from a number
#define CHECKBIT(var,bit)           (((var) >> (bit)) & (unsigned long) 0x01)   //!< Macro to read a particular bit of a number
#define SETBIT(var,bit)             ((var) |= ((unsigned long) 0x01 << (bit)))  //!< Macro to set a particular bit of a number
#define WRITEBIT(var,bit,val)       ((var) = (val) ? ((var) | ((unsigned long) 0x01 << (bit))) : ((var) & (~((unsigned long) 0x01 << (bit))))) //!< write a 0 or 1 to the specific bit position of a number
#define CLEARBIT(var,bit)            ((var) &= (~((unsigned long) 0x01 << (bit))))


// -----------------------------------------------------------------------------
//  DataFlash Subsector Definitions
//------------------------------------------------------------------------------

#define FIRST_DATAFLASH_SUBSECTOR                    0x0000U                //!< Address of the first subsector in dataflash 
#define LAST_DATAFLASH_SUBSECTOR                     0x03FFU                //!< 4096 subsectors in the dataflash, 4Kbyte each
#define PAGES_PER_SUBSECTOR                          16u                    //!< Number of pages per subsector
#define FIRST_DATAFLASH_PAGE                         0x0000U                //!< Address of first page in dataflash 
#define LAST_DATAFLASH_PAGE                          (((LAST_DATAFLASH_SUBSECTOR + 1u)*PAGES_PER_SUBSECTOR) - 1u)  //!< Address of last page in dataflash 
#define DATAFLASH_PAGE_SIZE                          256U                   //!< Bytes in one page of dataflash
#define NO_DATAFLASH_SUBSECTOR                       (LAST_DATAFLASH_SUBSECTOR + 1u)   //!< Last subsector in dataflash
#define DATAFLASH_SUBSECTOR                          (LAST_DATAFLASH_SUBSECTOR + 1u) //!< Total number of subsector in dataflash   
#define DATAFLASH_SUBSECTOR_SIZE                     4096U                  //!< Number of bytes in a subsector (16 x 256bytes)
#define DATAFLASH_DATALOG_BLOCK_SIZE                 DATAFLASH_SUBSECTOR_SIZE   //!< Datalog memory block size in bytes 
#define GENERAL_BUFFER_LENGTH                        256U                   //!< Define for array length
#define INITIAL_CRC_VALUE                            0xFFFFU                //!< Default value before calculation
#define CRC_SIZE_IN_BYTES                            2U                     //!< Datalog CRC size is 2 bytes
#define DATAFLASH_LAST_WORD_ADDRESS                  0x0FEu                 //!< Last location or word address in a dataflash subsector
#define MASK                                         0x000000FFU            //!< Mask for retention register
#define IGNORED_NBYTES                               255U                   //!< Define for number of bytes to be ignored
#define WRITE_IN_PROGRESS_BIT                        0x01U                  //!< Write in progress bit(WIP) indicates the memory is busy or not
#define DATALOG_PACKET_SIZE                          33U                    //!< Code for number of bytes per packet
#define BURAM_DATALOG_OFFSET                        18u                      //!< This define is used for starting register for datalog written in BURAM

// -----------------------------------------------------------------------------
//  Datalog subsectors [ 0x0000 - 0x03D9 ]  (1058 subsectors)
//      Headers: [ 0x0001 - 0x0027 ]  (39 subsectors)
//      Datalog: [ 0x0028 - 0x03D9 ]  (1018 subsectors)
//------------------------------------------------------------------------------

#define FIRST_HEADER_PAGE                            0x0001U                //!< Address of first header table subsector
#define LAST_HEADER_PAGE                             0x0027U                //!< Address of last header subsector
#define FIRST_RECORD_SUBSECTOR                       0x0028U                //!< Address of first datalog subsector

#ifdef DATALOG_QA_TEST
#define LAST_RECORD_SUBSECTOR                        0x002FU                //!< Greatly reduced datalog record size for testing datalog rollover
#else
#define LAST_RECORD_SUBSECTOR                        0x03D9U                //!< Address of last datalog subsector
#endif

#define FIRST_RECORD_PAGE       (FIRST_RECORD_SUBSECTOR * 16U)              //!< Frist record page number
#define LAST_RECORD_PAGE         (LAST_RECORD_SUBSECTOR * 16U)              //!< Last record page number
#define TOTAL_RECORD_PAGES \
    ((LAST_RECORD_PAGE) - (FIRST_RECORD_PAGE) + 1U)                         //!< Total number of pages for general data logging
#define TOTAL_RECORD_SUBSECTORS \
    ((LAST_RECORD_SUBSECTOR) - (FIRST_RECORD_SUBSECTOR) + 1U)               //!< Total number of subsector for general data logging

// -----------------------------------------------------------------------------
//  General subsectors [ 0x03DA]
//------------------------------------------------------------------------------

#define FIRST_GENERAL_SUBSECTOR                      0x03DAU                //!< Address of the first general subsector in dataflash

// -----------------------------------------------------------------------------
//  Reserved subsectors [ 0x0FDB - 0x0FE4 ] (10 subsectors)
//------------------------------------------------------------------------------

#define RESERVED_FIRST_SUBSECTOR                    0x03DBU                 
#define RESERVED_LAST_SUBSECTOR                     0x03E4U                 

// -----------------------------------------------------------------------------
//  BumpCal log subsectors [ 0x03E5 - 0x03E8 ] (4 subsectors)
//------------------------------------------------------------------------------

#define BOTTOM_BUMP_CAL_LOG_PAGE                     0x03E5u                //!< Bottom bump/cal log Subsector number
#define TOP_BUMP_CAL_LOG_PAGE                        0x03E8u                //!< Top bump/cal log Subsector number  

// -----------------------------------------------------------------------------
//  Event log subsectors [ 0x03E9 - 0x03EB ] (3 subsectors)
//------------------------------------------------------------------------------

#define BOTTOM_USER_EVENT_DATAFLASH_PAGE             0x03E9U                //!< Start offset of Event Log                                          
#define TOP_USER_EVENT_DATAFLASH_PAGE                0x03EBU                //!< End offset of Event Log

// -----------------------------------------------------------------------------
//  Error log subsector [ 0x03EC - 0x03ED ] (2 subsectors)
//------------------------------------------------------------------------------

#define BOTTOM_USER_ERRORLOG_DATAFLASH_PAGE          0x03ECU                //!< Start offset of Event Log                                          
#define TOP_USER_ERRORLOG_DATAFLASH_PAGE             0x03EDU                //!< End offset of Event Log

// -----------------------------------------------------------------------------
//  Custom PID factors subsector [ 0x03EE ] (1 subsector)
//------------------------------------------------------------------------------

#define CUSTOM_PID_FACTOR_PAGE                       0x03EEU                //!< Subsector for custom PID factors

// -----------------------------------------------------------------------------
//  Sensors backup subsectors [ 0x03EF - 0x03F6 ] (8 subsectors)
//------------------------------------------------------------------------------

#define SENSOR_BACKUP_1_SUBSECTOR                    0x03EFU                //!< Subsector for backup data of sensor at slot#1 
#define SENSOR_BACKUP_2_SUBSECTOR                    0x03F0U                //!< Subsector for backup data of sensor at slot#2 
#define SENSOR_BACKUP_3_SUBSECTOR                    0x03F1U                //!< Subsector for backup data of sensor at slot#3 
#define SENSOR_BACKUP_4_SUBSECTOR                    0x03F2U                //!< Subsector for backup data of sensor at slot#4 
#define SENSOR_BACKUP_5_SUBSECTOR                    0x03F3U                //!< Subsector for backup data of second sensor of COSH at slot#1
#define SENSOR_BACKUP_6_SUBSECTOR                    0x03F4U                //!< Subsector for backup data of second sensor of COSH at slot#2
#define SENSOR_BACKUP_7_SUBSECTOR                    0x03F5U                //!< Subsector for backup data of second sensor of COSH at slot#3
#define SENSOR_BACKUP_8_SUBSECTOR                    0x03F6U                //!< Subsector for backup data of second sensor of COSH at slot#4

// -----------------------------------------------------------------------------
//  Sensors backup subsectors [0x03F7 ] (1 subsectors)
//------------------------------------------------------------------------------

#define SENSOR_ALARM_MESSAGE_SUBSECTOR               0x03F7U                //!< Subsector for all 5 sensor types alarm message strings
#define BACKUP_FAILURE_COUNT_WORD_ADDR               0x00FDU                //!< Backup failure count will be read and write using this offset into sensor EEPROM backup page
#define RETURN_TO_ISC_OPTION                         203u                   //!< Retrun to ISC option (saved separately)
// -----------------------------------------------------------------------------
//  Shutdown successful subsector [ 0x03F8 ] (1 subsector)
//------------------------------------------------------------------------------

#define SHUTDOWN_DATAFLASH_BLOCK                     0x03F8U                //!< Address of the shutdown successful page, 0x1F01 -- needs to be outside of the datalog record area
#define SHUTDOWN_WORD_ADDRESS                        0x01U                  //!< Offset where shutdown status value is written to shutdown successful subsector
#define SYSTEM_RESET_WORD_ADDRESS                    0x05U                  //!< Offset where shutdown status value is written to shutdown successful subsector
#define SHUTDOWN_VALUE                               0xCAFEU                //!< Successful shutdown value
#define SYSTEM_RESET_VALUE                           0xFACEU                //!< System soft reset value

// -----------------------------------------------------------------------------
//  Instrument parameters subsector [ 0x03F9 - 0x03FC ] (4 subsectors)
//------------------------------------------------------------------------------

#define INST_PARAM_DATAFLASH_BLOCK_1                 0x03F9U       //!< Subsector for the Instrument parameters main page
#define INST_PARAM_DATAFLASH_MAGIC_1_BLOCK           0x03FAU       //!< Subsector for the Instrument parameters main magic number 
#define INST_PARAM_DATAFLASH_BLOCK_2                 0x03FBU       //!< Subsector for the Instrument parameters backup page
#define INST_PARAM_DATAFLASH_MAGIC_2_BLOCK           0x03FCU       //!< Subsector for the Instrument parameters backup magic number 

// -----------------------------------------------------------------------------
//  Sensor serial numnber subsector [ 0x03FD ] (1 subsector)
//------------------------------------------------------------------------------

#define SENSOR_SERNUM_DATAFLASH_BLOCK                0x03FDU                //!< Page for sensor serial number, used for new sensor check
#define SENSOR_SERNUM_BASE_ADDRESS(position)         ((position) * (CHARS_PER_SERIAL_NUMBER))  //!< Offset where serial number of the sensor is stored in dataflash's sensor serial number subsector

// -----------------------------------------------------------------------------
//  User site number subsector [ 0x03FE ] (1 subsector)
//------------------------------------------------------------------------------

#define USER_SITE_DATAFLASH_BLOCK                    0x03FEU                //!< Subsector for user site information used during event log

// -----------------------------------------------------------------------------
//  ATM parameters subsector [ 0x03FF ] (1 subsector)
//------------------------------------------------------------------------------

#define ATM_PARAM_PAGE                               0x03FFU                //!< Page for parameters used solely for ATM testing

//==============================================================================
//  GLOBAL DATA STRUCTURES DEFINITION
//==============================================================================

//! DataFlash command codes
typedef enum
{
    GENERAL_BUFFER_TO_DATAFLASH_COMMAND             = 0x00,   //!< Command code to transfer general buffer data to dataflash
    DATAFLASH_TO_GENERAL_BUFFER_COMMAND             = 0x01,   //!< Command code to transfer dataflash data to general buffer
    READ_DATAFLASH_PAGE_COMMAND                     = 0x02,   //!< Command code to read data from a page of dataflash
    ERASE_SUBSECTOR_COMMAND                         = 0x03,   //!< Command code to erase data from a subsector of dataflash
    WRITE_DATAFLASH_PAGE_COMMAND                    = 0x04,   //!< Command code to compare data of buffer with datalog data in dataflash before writing new data
    READ_DATAFLASH_FAST_COMMAND                     = 0x05,   //!< Fast read dataflash command
} DATAFLASH_COMMAND_ENUM ;

//! ENUM is used to classified dataflash into two buffer types
typedef enum
{
    DATALOG_BUFFER = 0x00,        //!< A type of buffer in dataflash
    GENERAL_BUFFER = 0x01         //!< Another type of buffer in dataflash
        
}BUFFERTYPE_ENUM ;

//Numeration for SPI Connected devices
typedef enum
{
   SPI_INVALID = 0,  //Invalid
   SPI_DATAFLASH = 1, //Main DataFlash
   SPI_NFC = 2,       //NFC Transciever
}SPI_DEVICE_ENUM; //TODO: add other SPI connected devices as well

//==============================================================================
//  GLOBAL DATA
//==============================================================================

extern unsigned short generalBufferPageNumber;     //!< This variable contains current subsector number in general buffer
extern bool isParametersClearAcknowledged;      //!< This variable is used to tell if instrument parameters erased is finished

//==============================================================================
//  EXTERNAL OR GLOBAL FUNCTIONS
//==============================================================================

//------------------------------------------------------------------------------
//   DataFlashInit(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function initializes the dataflash sector protection 
//
//------------------------------------------------------------------------------

void DataFlashInit(void);

//------------------------------------------------------------------------------
//   DataFlashGetCumulativeCRC(unsigned char  newByte, unsigned short *frameCRCValue)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function is used to to calculate table-based CRC from data to be written on dataflash
//
//------------------------------------------------------------------------------

void DataFlashGetCumulativeCRC(
                               unsigned char  newByte,       //!< This variable contains the new data byte
                               unsigned short *frameCRCValue //!< This variable contains the calculated CRC value
                               );

//------------------------------------------------------------------------------
//   DataFlashReadGeneralWord(unsigned short subsectorNumber, unsigned short wordNumber, unsigned short *data)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function reads a word(2 bytes) from a page of the dataflash
//
//------------------------------------------------------------------------------

void DataFlashReadGeneralWord(
                              unsigned short subsectorNumber,  //!< Data is to be read from the page of this subsector
                              unsigned short wordNumber,       //!< Address in the page to start reading from
                              unsigned short *data             //!< This variable contains data after successful read operation on dataflash
                              );

//------------------------------------------------------------------------------
//   DataFlashWriteGeneralWord(unsigned short subsectorNumber, unsigned short wordNumber, unsigned short data)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function writes a word(2 bytes) to the general page of the dataflash
//
//------------------------------------------------------------------------------

void DataFlashWriteGeneralWord(
                               unsigned short subsectorNumber,  //!< Data is to be write to the page of this subsector
                               unsigned short wordNumber,       //!< Address in the page to start writing to
                               unsigned short data              //!< This variable contains the data which is to be written on dataflash
                               );

//------------------------------------------------------------------------------
//   DataFlashReadSensorPage(unsigned short subsectorNumber, unsigned short byteAddress)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function reads the data from the sensor subsector of the dataflash and fill the sensor data structure
//
//------------------------------------------------------------------------------

void DataFlashReadSensorPage(
                             unsigned short subsectorNumber,  //!< Data is to be read from the sensor subsector
                             unsigned short byteAddress       //!< This variable contains the page offset
                             );

//------------------------------------------------------------------------------
//   DataFlashWriteSensorPage(unsigned short subsectorNumber, unsigned short byteAddr)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function writes the data to the sensor subsector of the dataflash which belongs to general buffer
//
//------------------------------------------------------------------------------

void DataFlashWriteSensorPage(
                              unsigned short subsectorNumber, //!< Data is to be write on the sensor subsector
                              unsigned short byteAddress      //!< This variable contains the subsector offset
                              );

//------------------------------------------------------------------------------
//   DataFlashErasePage(unsigned short subsectorNumber)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function erases the subsector from data flash 
//!  Special datalog and eventlog schemes will use erase subsector
//
//------------------------------------------------------------------------------

void DataFlashErasePage(
                        unsigned short subsectorNumber    //!< This variable contains the subsector number which is to erased from dataflash
                        );

//------------------------------------------------------------------------------
//   DataFlashReadArray(unsigned short subsectorNumber, unsigned short byteAddress, unsigned char *dataArray, unsigned short nBytes, bool isForDatalog)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function reads the data in an dataArray i.e. n number of bytes from the given subsector of dataflash
//
//------------------------------------------------------------------------------

void DataFlashReadArray(
                        unsigned short subsectorNumber,  //!< Data is to be read from the page of this subsector
                        unsigned short byteAddress,      //!< Page offset, data read starts from here
                        unsigned char *dataArray,        //!< All data read will be placed here
                        unsigned short nBytes,           //!< Number of bytes to be read
                        bool isForDatalog             //!< This variable conatins the status whether the data is read for datalog or not
                        );

//------------------------------------------------------------------------------
//   DataFlashTrasferFromBackupRAM(unsigned short subsectorNumber, unsigned short startByteNumber, 
//                                     unsigned short endByteNumber)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function transfers data from backup RAM (BURTC register 30 to 93) to dataflash.
//
//------------------------------------------------------------------------------

void DataFlashTrasferFromBackupRAM(
                                   unsigned short subsectorNumber,    //!< Subsector number to which data is to transfer
                                   unsigned short startByteNumber,    //!< Start byte number
                                   unsigned short endByteNumber       //!< Last byte number
                                  );

//------------------------------------------------------------------------------
//   DataFlashCommitGeneralBuffer(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function writes the data from the general buffer to dataflash if the contents of both are not the same
//
//------------------------------------------------------------------------------

void DataFlashCommitGeneralBuffer(void);

//------------------------------------------------------------------------------
//   DataFlashCommitBuffer(unsigned char *data, unsigned short subsectorNumber)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function writes the data from the given buffer to the specified subsector number of the dataflash
//
//------------------------------------------------------------------------------

void DataFlashCommitBuffer(
                               unsigned char *data,                             //!< start address of the data to be written
                               unsigned short subsectorNumber                   //!< The data is to be written on this subsector of dataflash
                           );

//------------------------------------------------------------------------------
//   DataFlashEraseGeneralBuffer(void) 
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function erases general buffer
//
//------------------------------------------------------------------------------

void DataFlashEraseGeneralBuffer(void);

//------------------------------------------------------------------------------
//   DataFlashDebugWrite(unsigned short sectorNumber,unsigned short wordAddress,unsigned short *dataWord);
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function writes a word in a dataflash 
//
//------------------------------------------------------------------------------

 void DataFlashDebugWrite(
                         unsigned short sectorNumber, //!< Subsector number to which data is to be transfered
                         unsigned short wordAddress,  //!< Word address to which data is to be tranfered
                         unsigned short *dataWord     //!< data to be written 
                         );

//------------------------------------------------------------------------------
//   DataFlashDebugRead(unsigned short sectorNumber,unsigned short wordAddress,unsigned short *data);
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function reads a word from dataflash
//
//------------------------------------------------------------------------------

 void DataFlashDebugRead( 
                        unsigned short sectorNumber, //!< Subsector number from which data is to be read
                        unsigned short wordAddress,  //!< Word Address which is to be read 
                        unsigned short *data         //!< data which is read 
                        );
 
 //------------------------------------------------------------------------------
//   DataFlashCommitDebugBuffer(unsigned short modbusRegisterNumber, unsigned short data)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function commit the buffer in dataflash debug mode
//
//------------------------------------------------------------------------------

void DataFlashCommitDebugBuffer(void);

void DataFlashReadWord(
                              unsigned short subsectorNumber,  //!< Data is to be read from the page of this subsector
                              unsigned short wordNumber,       //!< Address in the page to start reading from
                              unsigned short *data             //!< This variable contains data after successful read operation on dataflash
                              );

//------------------------------------------------------------------------------
//   DataFlashBackupIparams(void)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function writes the instrument parameters to one of the two specified pages in the Dataflash,
//!  depending on which page was more recently used.
//
//------------------------------------------------------------------------------

void DataFlashBackupIparams(void);


//------------------------------------------------------------------------------
//   DataFlashBackupIparamsToPage(unsigned short pageToWrite)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function writes the instrument parameters to the specified page in the Dataflash,
//!  depending on which page was more recently used.
//
//------------------------------------------------------------------------------

void DataFlashBackupIparamsToPage(
                                  unsigned short pageToWrite       //!< Page to write
                                 );

//------------------------------------------------------------------------------
//   void DataFlashWriteWord( unsigned short subsectorNumber, unsigned short wordNumber, unsigned short data );
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function writes a word directly to the Dataflash IC, without using the RAM buffers.
//
//------------------------------------------------------------------------------

void DataFlashWriteWord(
                        unsigned short subsectorNumber,
                        unsigned short wordNumber,
                        unsigned short data
                       );
//------------------------------------------------------------------------------
//   void DataFlashSPIInit(void);
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function initialize the SPI for dataflash
//
//------------------------------------------------------------------------------

void DataFlashSPIInit(void);

//------------------------------------------------------------------------------
//   void TM4CSPIInit(SPI_DEVICE_ENUM spiDevice)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/05
//
//!  This function initialize the SPI for dataflash
//
//------------------------------------------------------------------------------
void TM4CSPIInit(
                    SPI_DEVICE_ENUM spiDevice                                   //!< Connected SPI device
                );
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//   SPITransferData(SPI_DEVICE_ENUM spiDevice, unsigned char *txBuffer, unsigned short nBytes, unsigned char rxOffset, unsigned char *rxBuffer)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/05
//
//!  This function is used to read and write data over SPI bus
//
//!  \return ERRORCODE_ENUM_NO_ERROR                                   - No error detected
//!  \return ERRORCODE_ENUM_SPI_ERROR                                  - SPI error is detected
//
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

ERRORCODE_ENUM SPITransferData( 
                                SPI_DEVICE_ENUM spiDevice,     //!< SPI device id
                                unsigned char *txBuffer,       //!< Data to be written on the SPI bus
                                unsigned short nBytes,         //!< Number of bytes to be written
                                unsigned short rxOffset,       //!< Number of received bytes to be ignored
                                unsigned char *rxBuffer        //!< Pointer to the buffer containing the read value from SPI bus
                              );
//------------------------------------------------------------------------------
//   DataFlashReadSector(unsigned short subsectorNumber, unsigned char *data)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function reads a word(2 bytes) from a page of the dataflash
//
//------------------------------------------------------------------------------

void DataFlashReadSector(
                              unsigned short subsectorNumber,  //!< Data is to be read from the page of this subsector
                              unsigned char *data             //!< This variable contains data after successful read operation on dataflash
                        );

//------------------------------------------------------------------------------
//   unsigned char GetDataflashID(void)
//
//   Author:   Muhammad Shuaib
//   Date:     2016/17/12
//
//!  This function is used by other modules to get the installed dataflash type
//
//------------------------------------------------------------------------------

unsigned char GetDataflashID(void);

#endif /* __DATAFLASH_H__ */
//==============================================================================
//  End Of File
//==============================================================================