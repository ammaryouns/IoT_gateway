//==============================================================================
//
//  Dataflash.c
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
//  Source:        Dataflash.c
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
//! The External Data Flash memory 0x0000 - 0x03FF Range of DataFlash (1024 
//! subsector, 4 Kbytes each) shall be written or read using this module functions.
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
//  Revision: 1.0    2016/12/02  Fehan Arif
//      This module is taken from Vaughan
//
//==============================================================================
//  INCLUDES 
//==============================================================================
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <intrinsics.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_gpio.h>
#include <driverlib/flash.h>
#include <driverlib/gpio.h>
#include <driverlib/i2c.h>
#include <driverlib/pin_map.h>
#include <driverlib/pwm.h>
#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <driverlib/udma.h>
//#include <//assert.h>
#include "Dataflash.h"
#include "TM4CSPI.h"
#include "string.h"
#include "ErrorLog.h"
#include "Board.h"
#include "driverlib/sysctl.h"
#include "EK_TM4C1294XL.h"

//==============================================================================
//  CONSTANTS, TYPEDEFS AND MACROS 
//==============================================================================

// For new Dataflash SST/N25Q
#define STATUS_READ_OPCODE                          0xD7U     //!< Code to read status of dataflash
#define DATAFLASH_COMMAND_TABLE_SIZE                0x06U     //!< Number of dataflash commands i.e. one more than last DATAFLASH_COMMAND_ENUM
#define DATAFLASH_WRITE_EABLE                       0x06U     //!< Code to set the write enable latch bit
#define DATAFLASH_READ_IDENTIFICATION               0x9FU     //!< Code to read the device identification data
#define DATAFLASH_WRITE_STATUS_REGISTER             0x01U     //!< Code to write the status register
#define DATAFLASH_PAGE_WRITE_OPCODE                 0x02U     //!< Code to write a data to dataflash page
#define DATAFLASH_FAST_READ_OPCODE                  0x0Bu      //!< Code for fast read
#define SUBSECTOR_ERASE_OPCODE                      0x20U     //!< Code to erase the 4 Kbyte of memory  
#define DATAFLASH_PAGE_READ_OPCODE                  0x03U     //!< Code to read data of a page from dataflash
#define DATAFLASH_SENSOR_PAGE_BYTES                 16U       //!< 16 number of bytes are used to store sensor parameter values    
#define DATAFLASH_COMMANDS_RETRIES                  0X03U     //!< Maximum number of retries until a dataflash command becomes successful   
#define DATALOG_EMPTY_MEMORY_MARKER                 0xFFU     //!< Code for no data written   
#define MAX_FLASH_READY_RETRIES                     10000U     //!< Number of maximum retries to check dataflash ready status
#define COMMAND_STATUS_REGISTER_READ                0X05U     //!< Code to read the status of register 
#define DONT_CARE                                   0x00U     //!< Optional argument code
#define PAGE_SIZE                                   256U      //!< Define for number of bytes in a dataflash page
#define RX_OFFSET                                   4U        //!< Number of bytes to b ignored during the read operation
#define NEXT_PAGE_OFFSET                            0x0100U   //!< Define for the offset of next page
#define NBYTES                                      4U        //!< Define for number of bytes to b written over SPI
#define MANUFACTURER_IDENTIFICATION_N25Q            0x20U     //!< Code for the manufacturer identification of the device
#define MEMORY_TYPE_N25Q                            0xBAU     //!< Code that indicates the memory type of the device
#define MEMORY_CAPACITY_N25Q                        0x18U     //!< Code that indicates the memory capacity of the device
#define MANUFACTURER_IDENTIFICATION_SST             0xBFU     //!< Code for the manufacturer identification of the device
#define MEMORY_TYPE_SST                             0x25U     //!< Code that indicates the memory type of the device
#define MEMORY_CAPACITY_SST                         0x4AU     //!< Code that indicates the memory capacity of the device
#define MANUFACTURER_IDENTIFICATION_M25             0x20U     //!< Code for the manufacturer identification of the device
#define MEMORY_TYPE_M25                             0x71U     //!< Code that indicates the memory type of the device
#define MEMORY_CAPACITY_M25                         0x16U     //!< Code that indicates the memory capacity of the device
#define MANUFACTURER_IDENTIFICATION_MX25            0xC2U     //!< Code for the manufacturer identification of the device
#define MEMORY_TYPE_MX25                            0x20U     //!< Code that indicates the memory type of the device
#define MEMORY_CAPACITY_MX25                        0x16U     //!< Code that indicates the memory capacity of the device
#define LEFT_SHIFT_OPERATOR                         12U       //!< Define to left shift the value by 12
#define NBYTES_TO_WRITE                             2U        //!< Number of bytes to be written through SPI
#define LEFT_SHIFT_BY_EIGHT                         8U        //!< Define to left shift the value by eight
#define ARRAY_SIZE                                  3U        //!< Code for size of array
#define DEVICE_IDENTIFICATION                       0U        //!< Define used as a index of array to read the manufacturer identification of device
#define DEVICE_TYPE                                 1U        //!< Define used as a index of array to read the memory type of device
#define DEVICE_CAPACITY                             2U        //!< Define used as a index of array to read the memory capacity of device
#define RESET_VALUE                                 0x00U     //!< Code to reset the status register to disable the block protection
#define N25Q                                        1U        //!< Define for dataflash type - N25Q  
#define SST                                         2U        //!< Define for dataflash type - SST
#define M25                                         3U        //!< Define for dataflash type - M25
#define MX25                                        4U        //!< Define for dataflash type - MX25
#define DATAFLASH_BUSY                              1U        //!< Define for status of busy dataflash

//==============================================================================
//  LOCAL DATA STRUCTURE DEFINITION
//==============================================================================

//! This data structure defines the opCode and command length of the dataflash commands
typedef struct
{
    unsigned char opCode;                                     //!< Opcode of a dataflash command
    unsigned char commandLength;                              //!< Length of a dataflash command
    
} DATAFLASH_COMMAND_STRUCT;

typedef struct  
{
    unsigned char generalBuffer1[GENERAL_BUFFER_LENGTH];      //!< Buffer1 for general read/write operation from dataflash
    unsigned char generalBuffer2[GENERAL_BUFFER_LENGTH];      //!< Buffer2 for general read/write operation from dataflash
    unsigned char generalBuffer3[GENERAL_BUFFER_LENGTH];      //!< Buffer3 for general read/write operation from dataflash

}GENERAL_BUFFER_STRUCT;

//==============================================================================
//  GLOBAL DATA DECLARATIONS
//==============================================================================

unsigned short generalBufferPageNumber = NO_DATAFLASH_SUBSECTOR;   //!< This variable contains current subsector number in general buffer
bool isParametersClearAcknowledged = false;                     //!< This variable is used to tell if instrument parameters erased is finished
// Datalog
unsigned short startByteIndexForDatalog = 0u;                                     //!< This variable is used for starting address of backup RAM buffer from where data needs to be transfered to dataflash  
//==============================================================================
//  LOCAL DATA DECLARATIONS
//==============================================================================

unsigned char spiBuffer[20];                   //!< SPI buffer contains ata to write on SPI device
unsigned char firstSessionNumber = 1u;
unsigned short address ; 
   
   
//! This table provides the dataflash opcode and total length of dataflash command
// For new Dataflash
const DATAFLASH_COMMAND_STRUCT dataFlashCommandTable[DATAFLASH_COMMAND_TABLE_SIZE] =
{
    {DATAFLASH_PAGE_WRITE_OPCODE,             4U},   //!< GENERAL_BUFFER_TO_DATAFLASH_COMMAND = 0x00
    {DATAFLASH_PAGE_READ_OPCODE,              4U},   //!< DATAFLASH_TO_GENERAL_BUFFER_COMMAND = 0x01
    {DATAFLASH_PAGE_READ_OPCODE,              4U},   //!< READ_DATAFLASH_PAGE_COMMAND         = 0x02
    {SUBSECTOR_ERASE_OPCODE,                  4U},   //!< ERASE_SUBSECTOR_COMMAND             = 0x03
    {DATAFLASH_PAGE_WRITE_OPCODE,             4U},   //!< WRITE_DATAFLASH_PAGE_COMMAND        = 0x04
    {DATAFLASH_FAST_READ_OPCODE,              4U},   //!< FAST READ Dataflah

};

//! Table of CRC values for high-order byte
const unsigned char dataFlashCRCTableHigh[] = 
{
    0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x01U, 0xC0U,
    0x80U, 0x41U, 0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U,
    0x00U, 0xC1U, 0x81U, 0x40U, 0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U,
    0x80U, 0x41U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x00U, 0xC1U, 0x81U, 0x40U,
    0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x00U, 0xC1U,
    0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x01U, 0xC0U, 0x80U, 0x41U,
    0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x00U, 0xC1U,
    0x81U, 0x40U, 0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U,
    0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x01U, 0xC0U,
    0x80U, 0x41U, 0x00U, 0xC1U, 0x81U, 0x40U, 0x00U, 0xC1U, 0x81U, 0x40U,
    0x01U, 0xC0U, 0x80U, 0x41U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x00U, 0xC1U,
    0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x00U, 0xC1U, 0x81U, 0x40U,
    0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x01U, 0xC0U,
    0x80U, 0x41U, 0x00U, 0xC1U, 0x81U, 0x40U, 0x00U, 0xC1U, 0x81U, 0x40U,
    0x01U, 0xC0U, 0x80U, 0x41U, 0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U,
    0x80U, 0x41U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x00U, 0xC1U, 0x81U, 0x40U,
    0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x01U, 0xC0U,
    0x80U, 0x41U, 0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U,
    0x00U, 0xC1U, 0x81U, 0x40U, 0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U,
    0x80U, 0x41U, 0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U,
    0x01U, 0xC0U, 0x80U, 0x41U, 0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U,
    0x80U, 0x41U, 0x00U, 0xC1U, 0x81U, 0x40U, 0x00U, 0xC1U, 0x81U, 0x40U,
    0x01U, 0xC0U, 0x80U, 0x41U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x00U, 0xC1U,
    0x81U, 0x40U, 0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U,
    0x00U, 0xC1U, 0x81U, 0x40U, 0x01U, 0xC0U, 0x80U, 0x41U, 0x01U, 0xC0U,
    0x80U, 0x41U, 0x00U, 0xC1U, 0x81U, 0x40U
}; 

//! Table of CRC values for low-order byte
const unsigned char dataFlashCRCTableLow[] = 
{
    0x00U, 0xC0U, 0xC1U, 0x01U, 0xC3U, 0x03U, 0x02U, 0xC2U, 0xC6U, 0x06U,
    0x07U, 0xC7U, 0x05U, 0xC5U, 0xC4U, 0x04U, 0xCCU, 0x0CU, 0x0DU, 0xCDU,
    0x0FU, 0xCFU, 0xCEU, 0x0EU, 0x0AU, 0xCAU, 0xCBU, 0x0BU, 0xC9U, 0x09U,
    0x08U, 0xC8U, 0xD8U, 0x18U, 0x19U, 0xD9U, 0x1BU, 0xDBU, 0xDAU, 0x1AU,
    0x1EU, 0xDEU, 0xDFU, 0x1FU, 0xDDU, 0x1DU, 0x1CU, 0xDCU, 0x14U, 0xD4U,
    0xD5U, 0x15U, 0xD7U, 0x17U, 0x16U, 0xD6U, 0xD2U, 0x12U, 0x13U, 0xD3U,
    0x11U, 0xD1U, 0xD0U, 0x10U, 0xF0U, 0x30U, 0x31U, 0xF1U, 0x33U, 0xF3U,
    0xF2U, 0x32U, 0x36U, 0xF6U, 0xF7U, 0x37U, 0xF5U, 0x35U, 0x34U, 0xF4U,
    0x3CU, 0xFCU, 0xFDU, 0x3DU, 0xFFU, 0x3FU, 0x3EU, 0xFEU, 0xFAU, 0x3AU,
    0x3BU, 0xFBU, 0x39U, 0xF9U, 0xF8U, 0x38U, 0x28U, 0xE8U, 0xE9U, 0x29U,
    0xEBU, 0x2BU, 0x2AU, 0xEAU, 0xEEU, 0x2EU, 0x2FU, 0xEFU, 0x2DU, 0xEDU,
    0xECU, 0x2CU, 0xE4U, 0x24U, 0x25U, 0xE5U, 0x27U, 0xE7U, 0xE6U, 0x26U,
    0x22U, 0xE2U, 0xE3U, 0x23U, 0xE1U, 0x21U, 0x20U, 0xE0U, 0xA0U, 0x60U,
    0x61U, 0xA1U, 0x63U, 0xA3U, 0xA2U, 0x62U, 0x66U, 0xA6U, 0xA7U, 0x67U,
    0xA5U, 0x65U, 0x64U, 0xA4U, 0x6CU, 0xACU, 0xADU, 0x6DU, 0xAFU, 0x6FU,
    0x6EU, 0xAEU, 0xAAU, 0x6AU, 0x6BU, 0xABU, 0x69U, 0xA9U, 0xA8U, 0x68U,
    0x78U, 0xB8U, 0xB9U, 0x79U, 0xBBU, 0x7BU, 0x7AU, 0xBAU, 0xBEU, 0x7EU,
    0x7FU, 0xBFU, 0x7DU, 0xBDU, 0xBCU, 0x7CU, 0xB4U, 0x74U, 0x75U, 0xB5U,
    0x77U, 0xB7U, 0xB6U, 0x76U, 0x72U, 0xB2U, 0xB3U, 0x73U, 0xB1U, 0x71U,
    0x70U, 0xB0U, 0x50U, 0x90U, 0x91U, 0x51U, 0x93U, 0x53U, 0x52U, 0x92U,
    0x96U, 0x56U, 0x57U, 0x97U, 0x55U, 0x95U, 0x94U, 0x54U, 0x9CU, 0x5CU,
    0x5DU, 0x9DU, 0x5FU, 0x9FU, 0x9EU, 0x5EU, 0x5AU, 0x9AU, 0x9BU, 0x5BU,
    0x99U, 0x59U, 0x58U, 0x98U, 0x88U, 0x48U, 0x49U, 0x89U, 0x4BU, 0x8BU,
    0x8AU, 0x4AU, 0x4EU, 0x8EU, 0x8FU, 0x4FU, 0x8DU, 0x4DU, 0x4CU, 0x8CU,
    0x44U, 0x84U, 0x85U, 0x45U, 0x87U, 0x47U, 0x46U, 0x86U, 0x82U, 0x42U,
    0x43U, 0x83U, 0x41U, 0x81U, 0x80U, 0x40U
};
unsigned char deviceInstalled = 0u;           //!< This variable contains the type of dataflash device
GENERAL_BUFFER_STRUCT generalBuffer;                        //!< Structure used as a general buffer for reading and writing data to dataflash   
unsigned short currentSubSector = 2000u;                    //!< This variable contains the number of  sub-sector in buffer
unsigned char dataBuffer[DATAFLASH_SUBSECTOR_SIZE];         //!< This variable contains the data of a sub-sector
bool isDebugBufferUpadted=false;                         //!< This variable indicates that buffer has been updated

// This buffer contains the read data
unsigned char receiveBuffer[5] = {8U};
//==============================================================================
//  LOCAL FUNCTION PROTOTYPES
//==============================================================================

static int  IfDataFlashReady(void);
static void TransferFlashToBuffer(BUFFERTYPE_ENUM bufferType, unsigned short subsectorNumber);
static void ReadIdentification(void);
static void WriteGeneralWord(unsigned short pageOffset, unsigned short data);
static void ReadGeneralWord(unsigned short pageOffset, unsigned short *rxData);
static void SubsectorErase(unsigned short sectortNumber);
static void BuildDataFlashCommand( DATAFLASH_COMMAND_ENUM command,unsigned int dataFlashSubsector,unsigned short pageOffset,unsigned char *pBuffer, unsigned char *dataFlashCommandLength,bool isDatalog);
static void GetDataFlashStatus(unsigned char *flashStatus);
static void WriteDataFlashCommandBytes(unsigned char *commandByte,unsigned short commandLength, unsigned char rxOffset,unsigned char *rxData);
static void WriteEnableDataflash(void);

//==============================================================================
//   LOCAL FUNCTIONS IMPLEMENTATION
//==============================================================================

//------------------------------------------------------------------------------
//   BuildDataFlashCommand(DATAFLASH_COMMAND_ENUM command, unsigned short dataFlashSubsector, unsigned short pageOffset, unsigned char *pBuffer, unsigned char *dataFlashCommandLength )
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function is used to build Dataflash commands and sets the command length
//
//------------------------------------------------------------------------------

static void BuildDataFlashCommand(
                                  DATAFLASH_COMMAND_ENUM command,        //!< This variable contains command code for dataflash
                                  unsigned int dataFlashSubsector,       //!< This variable contains dataflash page number
                                  unsigned short pageOffset,             //!< This variable contains the dataflash page offset address
                                  unsigned char *pBuffer,                //!< This is a data buffer
                                  unsigned char *dataFlashCommandLength, //!< The variable contains the length of the command for SPI commands for dataflash
                                  bool isDatalog                      //!< This flag tells if command is for datalog
                                  )
{
    unsigned int dataFlashAddress = (unsigned int)0;
    // Validating subsector number and offset
    ////assert(dataFlashSubsector > 0) ;
    //assert(dataFlashSubsector < DATAFLASH_SUBSECTOR) ;
    //assert(pageOffset < DATAFLASH_SUBSECTOR_SIZE) ;
    // Get the opcode for instruction
    *pBuffer++ = dataFlashCommandTable[command].opCode ;
    // Calculate the dataflash page number
    if(isDatalog == true)
    {
        dataFlashSubsector = dataFlashSubsector << 8;
    }
    else
    {
        dataFlashSubsector = dataFlashSubsector << LEFT_SHIFT_OPERATOR;
    }
    // Add the offset of page
    dataFlashAddress = dataFlashSubsector + pageOffset;
    *pBuffer++ = LOBYTE_WORD16(HIWORD_WORD32(dataFlashAddress));
    *pBuffer++ = HIBYTE_WORD16(LOWORD_WORD32(dataFlashAddress));
    *pBuffer++= LOBYTE_WORD16(LOWORD_WORD32(dataFlashAddress));
    // Get the command length
    *dataFlashCommandLength = dataFlashCommandTable[command].commandLength ;
}

//------------------------------------------------------------------------------
//   GetDataFlashStatus(unsigned char *flashStatus)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function reads the write in progress(WIP) bit which indicates that the memory is busy
//!  with a write status register, program or erase cycle
//
//------------------------------------------------------------------------------

static void GetDataFlashStatus(
                               unsigned char *flashStatus    //!< This variable contains the flash status
                               )
{
    // Length of dataFlash commands used in this function
    unsigned char commandLength = 0u;
    // This variable is used to contain the error code
    ERRORCODE_ENUM error = ERRORCODE_ENUM_NO_ERROR;
    // Writing status register, read command byte and then read status over SPI bus
    spiBuffer[commandLength++] = COMMAND_STATUS_REGISTER_READ;
    // Transfer the data through SPI
    error = SPITransferData(SPI_DATAFLASH, spiBuffer, NBYTES_TO_WRITE, (unsigned short)commandLength, flashStatus);
    //! \todo - What we should do if there is an error in SPI communication?
    if ( error != ERRORCODE_ENUM_NO_ERROR )
    {
       //Do something here
    }
    //TODO: Add resource release equvlent here
}

//------------------------------------------------------------------------------
//   IfDataFlashReady(void)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function is used to check if the dataflash is ready for read write operation or return error if timed out
//
//!  \return 0                                             - No Error
//!  \return ERRORCODE_ENUM_DATAFLASH_TIMEOUT_OCCURRED     - Timeout, dataflash status is not read
//
//------------------------------------------------------------------------------

static int IfDataFlashReady(void)
{ 
    // This variable contains the status of the dataflash 
    unsigned char flashStatus = 0u;
    // Number of retries to check dataflash status
    unsigned int errorCount = (unsigned int)0;
    unsigned char status = 0u;
    // Function return value for error code
    int returnValue = 0;

    // Reading dataflash status i.e. 1st bit write in progress bit of the status byte for MAX_FLASH_READY_RETRIES
    // time or 1st bit is 1
    do
    {
        // Get the RDY/xBSY bit
        GetDataFlashStatus(&status);  
        flashStatus = status & WRITE_IN_PROGRESS_BIT;
        errorCount++;
       /* for(int i=0;i<500;i++)
        {
          ;        
        }*/
    }
    while( (flashStatus == DATAFLASH_BUSY) && (errorCount < MAX_FLASH_READY_RETRIES)); 

    // Write Protection Pin of SPI is enabled 
    // Flash was still busy, but it timed out
    if (flashStatus == DATAFLASH_BUSY)  
    {
        //assert(false);
        returnValue =  (int) ERRORCODE_ENUM_DFLASH_TIMEOUT_OCCURRED;
    }
    else 
    {
        returnValue = (int) ERRORCODE_ENUM_NO_ERROR;
    }
    return (returnValue);
}  

//------------------------------------------------------------------------------
//   WriteDataFlashCommandBytes(unsigned char *commandByte, unsigned char commandLength, unsigned char rxOffset, unsigned char *rxData )
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function is used to write dataflash command bytes through SPI
//
//------------------------------------------------------------------------------

static void WriteDataFlashCommandBytes(
                                       unsigned char *commandByte,      //!< This variable contains the command byte
                                       unsigned short commandLength,    //!< This variable contains the command length
                                       unsigned char rxOffset,          //!< Number of received bytes to be ignored
                                       unsigned char *rxData            //!< This variable contains the read value from SPI bus
                                       )
{
    // This variable is used to contain the error code
    ERRORCODE_ENUM error = ERRORCODE_ENUM_NO_ERROR;
    // Writes the data through SPI
    error = SPITransferData(SPI_DATAFLASH, commandByte, commandLength, (unsigned short)rxOffset, rxData);
    //! \todo - What we should do if there is an error in SPI communication?
    if ( error != ERRORCODE_ENUM_NO_ERROR )
    {
       //Do something here
    } 
}

//------------------------------------------------------------------------------
//   TransferFlashToBuffer(BUFFERTYPE_ENUM bufferType, unsigned short subsectorNumber)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function is used to transfer or copy the data from dataflash to buffer
//!  This function is mostly called with requestResource == true.  Exception was 
//!  made to allow the dataflash resource to also protect the general buffer cache
//!  page.
//
//------------------------------------------------------------------------------

static void TransferFlashToBuffer(
                                  BUFFERTYPE_ENUM bufferType,     //!< Type of the buffer to which data is to be copied
                                  unsigned short subsectorNumber  //!< This variable contains the subsector address of dataflash
                                  )
{   
    // This variable contains the length of dataFlash commands used in this function
    unsigned char commandLength = 0u;
    // Data copy retries counter    
    unsigned char numberOfTries = 0u;
    // This variable is used to store the dataflash data copy operation status is successful or not
    bool isWriteSuccessful = false;
    // Copy data from dataflash operation repeats maximum DATAFLASH_COMMANDS_RETRIES times if it does not go successful
    do
    {
        if (bufferType == GENERAL_BUFFER)
        {
            // GB1
            // Build a dataflash command for general buffer type
            BuildDataFlashCommand(DATAFLASH_TO_GENERAL_BUFFER_COMMAND, (unsigned int)subsectorNumber, DONT_CARE, spiBuffer, &commandLength, false);
            WriteDataFlashCommandBytes(spiBuffer,((unsigned short)commandLength+PAGE_SIZE), RX_OFFSET, generalBuffer.generalBuffer1);
            
            if (IfDataFlashReady() == ERRORCODE_ENUM_NO_ERROR)
            {
                isWriteSuccessful = true;   
            }
            else
            {
                isWriteSuccessful = false;   
            }
            
            if (isWriteSuccessful)
            {                 
                // GB2
                BuildDataFlashCommand(DATAFLASH_TO_GENERAL_BUFFER_COMMAND, (unsigned int)subsectorNumber, NEXT_PAGE_OFFSET, spiBuffer, &commandLength, false);
                WriteDataFlashCommandBytes(spiBuffer,((unsigned short)commandLength+PAGE_SIZE), RX_OFFSET, generalBuffer.generalBuffer2);
                
                if (IfDataFlashReady() == ERRORCODE_ENUM_NO_ERROR)
                {
                    isWriteSuccessful = true;   
                }
                else
                {
                    isWriteSuccessful = false;   
                }  
                
            }
            
            if (isWriteSuccessful)
            {          
                // GB3
                BuildDataFlashCommand(DATAFLASH_TO_GENERAL_BUFFER_COMMAND, (unsigned int)subsectorNumber, \
                  (2u*NEXT_PAGE_OFFSET), spiBuffer, &commandLength, false);
                WriteDataFlashCommandBytes(spiBuffer,((unsigned short)commandLength+PAGE_SIZE), RX_OFFSET, generalBuffer.generalBuffer3);
                
                if (IfDataFlashReady() == ERRORCODE_ENUM_NO_ERROR)
                {
                    isWriteSuccessful = true;   
                }
                else
                {
                    isWriteSuccessful = false;   
                }              
            }
        } 
        numberOfTries++;
        //Reset the watchdog once
        //TODO:
    } // Repeat the copy process DATAFLASH_COMMANDS_RETRIES times or until it is not successful
    while ( (isWriteSuccessful == false) && (numberOfTries < DATAFLASH_COMMANDS_RETRIES) );
    // If number of erase retries exceeds DATAFLASH_COMMANDS_RETRIES then this is an error
    //
   /* if ( numberOfTries >= DATAFLASH_COMMANDS_RETRIES )
    {
        ErrorLogWrite(ERRORCODE_ENUM_DFLASH_BUF_COPY_FAIL, ERRORTYPE_ENUM_CRITICAL);
    }*/
}



//------------------------------------------------------------------------------
//   WriteGeneralWord(unsigned short pageOffset, unsigned short data)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function is used to write the word data to general buffer
//
//------------------------------------------------------------------------------

static void WriteGeneralWord(
                             unsigned short pageOffset,    //!< This variable contains the address in the page to start writing to
                             unsigned short data           //!< Data to be written
                             )
{
    // This variable is used as a index
    unsigned short index = 0u;
    
    if (pageOffset < PAGE_SIZE)
    {
        // GB1
        index = pageOffset;
        generalBuffer.generalBuffer1[index] = HIBYTE_WORD16(data);
        generalBuffer.generalBuffer1[index + 1u] = LOBYTE_WORD16(data);
    }
    else if(pageOffset < (2u*PAGE_SIZE))
    {
        // GB2
        index = pageOffset - PAGE_SIZE;
        generalBuffer.generalBuffer2[index] = HIBYTE_WORD16(data);
        generalBuffer.generalBuffer2[index + 1u] = LOBYTE_WORD16(data);
    }
    else 
    {
        // GB3
        index = pageOffset - (2u * PAGE_SIZE);
        generalBuffer.generalBuffer3[index] = HIBYTE_WORD16(data);
        generalBuffer.generalBuffer3[index + 1u] = LOBYTE_WORD16(data);
    } 
}

//------------------------------------------------------------------------------
//   ReadGeneralWord(unsigned short pageOffset, unsigned char *rxBuffer)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function is used to read the word data from general buffer
//
//------------------------------------------------------------------------------

static void ReadGeneralWord(
                            unsigned short pageOffset,   //!< This variable contains the address in the page
                            unsigned short *rxData       //!< This variable contains data after successful read operation
                            )
{
    // This variable is used as a index
    unsigned short index = (unsigned short)0;
    
    if(pageOffset < PAGE_SIZE)
    {
        // GB1
        index = pageOffset;
        *rxData = (unsigned short)((unsigned short)generalBuffer.generalBuffer1[index] << LEFT_SHIFT_BY_EIGHT ) |\
            (unsigned short)generalBuffer.generalBuffer1[index+1u];

    }
    else if(pageOffset < (2u * PAGE_SIZE))
    {
        // GB2
        index = pageOffset - PAGE_SIZE;
        *rxData = (unsigned short)((unsigned short)generalBuffer.generalBuffer2[index] << LEFT_SHIFT_BY_EIGHT ) |\
            (unsigned short)generalBuffer.generalBuffer2[index+1u];
    }
    else
    {
        // GB3
        index = pageOffset - (2u * PAGE_SIZE);
        *rxData = (unsigned short)((unsigned short)generalBuffer.generalBuffer3[index] << LEFT_SHIFT_BY_EIGHT ) |\
            (unsigned short)generalBuffer.generalBuffer3[index+1u];
    }
}

//------------------------------------------------------------------------------
//   SubsectorErase(unsigned short sectortNumber)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function is used to erase the sub sector
//
//------------------------------------------------------------------------------

static void SubsectorErase(
                           unsigned short sectortNumber    //!< This variable contains the sector number which is to be erased
                           )
{
    // This variable contains the length of dataFlash commands used in this function
    unsigned char commandLength = (unsigned char)0;
    // Temporary variable contains the read value
    unsigned char temp = (unsigned char)0;    
    unsigned short attempts = 0u;    
    bool isEraseComplete = false;
    unsigned short readbackValue1 = 0u;
    unsigned short readbackValue2 = 0u;
    
    do
    {        
        IfDataFlashReady();       
        // Reset watchdog timer
        //TODO
        // Latch the write enable bit
        WriteEnableDataflash();
        BuildDataFlashCommand(ERASE_SUBSECTOR_COMMAND, (unsigned int)sectortNumber, DONT_CARE, spiBuffer, \
          &commandLength, false);
        // Start SPI command write process
        WriteDataFlashCommandBytes(spiBuffer, (unsigned short)commandLength, IGNORED_NBYTES, &temp);
        // Check the busy bit of dataflash        
        IfDataFlashReady();        
        // Confirm that the erase worked correctly.
        
        DataFlashReadWord(sectortNumber, 0u, &readbackValue1);
        DataFlashReadWord(sectortNumber, 255u, &readbackValue2);
        if ((readbackValue1 == 0xFFFFu) && (readbackValue2 == 0xFFFFu))
        {
            isEraseComplete = true;   
        }        
        // Timeout after several attempts.
        attempts++;
        
    } while ((isEraseComplete == false) && (attempts < 4u));
    
    // Throw an error if the erase command failed.
    //TODO: 
   /* if (!isEraseComplete)
    {
        // Check if low battery warning and alarm are  active
        if((isLowBatteryWarningRunning == false) && (isCriticalLowBatteryAlarmActive == false))
        {
            ErrorLogWrite(ERRORCODE_ENUM_DFLASH_ERASE_FAILURE, ERRORTYPE_ENUM_CRITICAL);                 
        }
        else
        {
            // 503 produced due to low battery, force the critical alarm if its not yet active VPRO-62
            shouldForceCriticalBatteryAlarm = true;
        }

    }*/
}

//------------------------------------------------------------------------------
//   WriteEnableDataflash(void)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function is used to set the write enable latch(WEL) bit
//
//------------------------------------------------------------------------------

static void WriteEnableDataflash(void)
{
    // Length of dataFlash commands used in this function
    unsigned char commandLength = 0u;
    // Temporary variable
    unsigned char temp= 0u;
    // This variable is used to contain the error code
    ERRORCODE_ENUM error = ERRORCODE_ENUM_NO_ERROR;
    // Instruction code to set the write enable latch bit 
    spiBuffer[commandLength++] = DATAFLASH_WRITE_EABLE;
    //TODO: Equvilent of K_RESOURCE_RELEASE in TI RTOS
    // Write the data through SPI
    error = SPITransferData(SPI_DATAFLASH, spiBuffer,(unsigned short)commandLength, IGNORED_NBYTES, &temp);
    if ( error !=  ERRORCODE_ENUM_NO_ERROR )
    {
       
    }
    //TODO: Equvilent of K_RESOURCE_RELEASE in TI RTOS
}
//------------------------------------------------------------------------------
//   ReadIdentification(void)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function is used to read the device identification data
//
//------------------------------------------------------------------------------

static void ReadIdentification(void)
{
    // Length of dataFlash commands used in this function
    unsigned char commandLength = 0u;
//    // This buffer contains the read data
//    unsigned char receiveBuffer[ARRAY_SIZE] = {0U, 0U, 0U};
    // This variable is used to contain the error code
    ERRORCODE_ENUM error = ERRORCODE_ENUM_NO_ERROR;   
    spiBuffer[commandLength++] = DATAFLASH_READ_IDENTIFICATION;
    // Write the data through SPI
    error = SPITransferData(SPI_DATAFLASH, spiBuffer, NBYTES,(unsigned short)commandLength, receiveBuffer);
     //! \todo - What we should do if there is an error in SPI communication?
    if ( error != ERRORCODE_ENUM_NO_ERROR )
    {
       //Do something here 
    }
    //  Check the type of dataflash
    else if ((receiveBuffer[DEVICE_IDENTIFICATION] == MANUFACTURER_IDENTIFICATION_N25Q) && 
            ( receiveBuffer[DEVICE_TYPE] == MEMORY_TYPE_N25Q ) &&
            ( receiveBuffer[DEVICE_CAPACITY] == MEMORY_CAPACITY_N25Q ) )
    {
        deviceInstalled = N25Q;
    }
    else if( (receiveBuffer[DEVICE_IDENTIFICATION] == MANUFACTURER_IDENTIFICATION_SST) &&
        ( receiveBuffer[DEVICE_TYPE] == MEMORY_TYPE_SST ) &&\
        ( receiveBuffer[DEVICE_CAPACITY] == MEMORY_CAPACITY_SST ) )
    {
        deviceInstalled = SST;
    }
    else if((receiveBuffer[DEVICE_IDENTIFICATION] == MANUFACTURER_IDENTIFICATION_M25) && 
        ( receiveBuffer[DEVICE_TYPE] == MEMORY_TYPE_M25 ) && 
        ( receiveBuffer[DEVICE_CAPACITY] == MEMORY_CAPACITY_M25 ) )
    {
        deviceInstalled = M25;
    }
    else if((receiveBuffer[DEVICE_IDENTIFICATION] == MANUFACTURER_IDENTIFICATION_MX25) && 
        ( receiveBuffer[DEVICE_TYPE] == MEMORY_TYPE_MX25 ) && 
        ( receiveBuffer[DEVICE_CAPACITY] == MEMORY_CAPACITY_MX25 ) )
    {
        deviceInstalled = MX25;
    }
    else
    {
    }
}

//==============================================================================
//  GLOBAL FUNCTIONS IMPLEMENTATION
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

void DataFlashInit(void)
{

    // This variable contains the length of  dataFlash commands
    unsigned char commandLength = 0u;
    // Temporary variable 
    unsigned char temp = 0u;
    //To track the error code
    ERRORCODE_ENUM error = ERRORCODE_ENUM_NO_ERROR;
    // This variable is used to contain the error code
   // Read the identification of dataflash
    ReadIdentification();
    // Check if FLASH is of type SST
    if ( deviceInstalled == SST )
    {
       // Latch the write enable bit for write process
        WriteEnableDataflash();     
        // Write the status register to unlock the protected memory
        spiBuffer[commandLength++] = DATAFLASH_WRITE_STATUS_REGISTER;
        // write the status register to disable the block protection
        spiBuffer[commandLength++] = RESET_VALUE; 
        // Write the data over SPI
        error = SPITransferData(SPI_DATAFLASH, spiBuffer, (unsigned short)commandLength, IGNORED_NBYTES, &temp);  
        //! \todo - What we should do if there is an error in SPI communication?
        if ( error!= ERRORCODE_ENUM_NO_ERROR )
        {
           //Do something here
        }
        // Check the busy bit of dataflash
       /* do
        {
            //GetDataFlashStatus(&status);
            //__no_operation();
        }
        while ( (status & WRITE_IN_PROGRESS_BIT) == true);*/
    }
}
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
                              )
{
    //For recording the error code
    ERRORCODE_ENUM errorCode = ERRORCODE_ENUM_NO_ERROR;
    //For checking the transfer success
    bool transferOK = false;
    //SPI Configuration Handle
    SPI_Handle      handle;
    //SPI Control Parameters
    SPI_Params      params;
    //SPI Transaction control
    SPI_Transaction spiTransaction;
    //Status of SPI
    SPI_Status status;
    //For indexing the loop
    unsigned short loopIndex = 0u;
    //For limiting the loop
     short loopLimit = 0;
    if ( nBytes > rxOffset )
    {
      loopLimit  = nBytes - rxOffset;
    }
    else
    {
       //Do nothing
    }
    if( spiDevice == SPI_DATAFLASH )
    {
            //Initialize SPI Parameters (TODO: Check the default parameters settings)
       SPI_Params_init(&params);
       //Set the transfer mode
       params.transferMode = SPI_MODE_BLOCKING;
       params.frameFormat = SPI_POL1_PHA1;
      //Set the baud rate
      params.bitRate = 8000000;
      //Open SPI for the communication
      handle = SPI_open(Board_SPI3, &params);
      if (!handle) 
      {
         errorCode = ERRORCODE_ENUM_SPI_ERROR;
      }
      else
      {
         // Setup SPI transaction
         spiTransaction.count = nBytes;
         spiTransaction.txBuf = txBuffer;
         spiTransaction.rxBuf = rxBuffer;        
         transferOK = SPI_transfer(handle, &spiTransaction);
         if (!transferOK) 
         {
            errorCode = ERRORCODE_ENUM_SPI_ERROR;
         }
         else
         {
            errorCode = ERRORCODE_ENUM_NO_ERROR;
         }
      }
      //Open SPI for the communication
      SPI_close(handle);
    }
    else
    {
      //Do nothing TODO: Add for other files
    }
    if ( errorCode == ERRORCODE_ENUM_NO_ERROR )
    {
       while( loopIndex < loopLimit )
       {
         rxBuffer[loopIndex] = rxBuffer[rxOffset];
         loopIndex++;
         rxOffset++;
       }
    }
    else
    {
      //Do nothing
    }
    return errorCode;
}

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
                               )
{
    // This variable contains higher byte of calculated CRC value    
    unsigned char CRCHighByte = HIBYTE_WORD16(*frameCRCValue) ;
    // This variable contains lower byte of calculated CRC value    
    unsigned char CRCLowByte = LOBYTE_WORD16(*frameCRCValue) ;
    // This variable is used as index into CRC lookup
    unsigned char CRCIndex ;   

    // Calculate the CRC from CRC tables
    CRCIndex = CRCHighByte ^ newByte ;  
    CRCHighByte = CRCLowByte ^ dataFlashCRCTableHigh[CRCIndex] ;
    CRCLowByte = dataFlashCRCTableLow[CRCIndex] ;
    (*frameCRCValue) = CRCHighByte  ;
    (*frameCRCValue) = (*frameCRCValue) << LEFT_SHIFT_BY_EIGHT ;
    (*frameCRCValue) |= CRCLowByte ;
}

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
                              )
{ 
    // This variable contains the length of  dataFlash commands
    unsigned char commandLength = 0u;
    // This variable contain the read data 
    unsigned short readWord = (unsigned short)0;
    // Buffer contains the data read from dataflash
    unsigned char tempBuffer[2] = {0U,0U};
    // This variable is used as a loop index
    unsigned int byteCounter = (unsigned int)0;
    //assert(subsectorNumber >= FIRST_HEADER_PAGE); 
    //assert(generalBufferPageNumber >= FIRST_GENERAL_SUBSECTOR);
    // Checking page to build read command from general buffer or from dataFlash page
    if (generalBufferPageNumber == subsectorNumber)
    {
        // Read from the buffer instead of the flash page, its data may be newer
        ReadGeneralWord((wordNumber << 1), &readWord);
        *data = readWord;
    }
    else
    {
        // Read from dataflash page
        BuildDataFlashCommand(READ_DATAFLASH_PAGE_COMMAND, (unsigned int)subsectorNumber, (wordNumber << 1),\
            spiBuffer, &commandLength, false);
        // SPI operation to write command bytes and then read the data
        WriteDataFlashCommandBytes(spiBuffer,((unsigned short)commandLength + NBYTES_TO_WRITE), RX_OFFSET, tempBuffer);
        byteCounter = (unsigned int)0;
        *data = (unsigned short)((unsigned short)tempBuffer[byteCounter]<< LEFT_SHIFT_BY_EIGHT) | \
            (unsigned short)tempBuffer[byteCounter+(unsigned char)1];
    }
    (void) IfDataFlashReady();
}
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
                        )
{ 
    // This variable contains the length of  dataFlash commands
    unsigned char commandLength = 0u;
    // This variable contain the read data 
    unsigned short readWord = (unsigned short)0;
    // Buffer contains the data read from dataflash
    unsigned char tempBuffer[2] = {0U,0U};
    // This variable is used as a loop index
    unsigned int byteCounter = (unsigned int)0;
    //For the page number
    unsigned char pageNumber = 0u;
    for( pageNumber = 0; pageNumber < 16; pageNumber++ )
    {
       // Read from dataflash page
       BuildDataFlashCommand(READ_DATAFLASH_FAST_COMMAND, (unsigned int)subsectorNumber, (unsigned short)(NEXT_PAGE_OFFSET*pageNumber),\
               spiBuffer, &commandLength, false);
       // SPI operation to write command bytes and then read the data
       WriteDataFlashCommandBytes(spiBuffer,((unsigned short)commandLength + 256), (RX_OFFSET+1), &data[(pageNumber*NEXT_PAGE_OFFSET)]);
       (void) IfDataFlashReady();
    }
}
//------------------------------------------------------------------------------
//   DataFlashReadWord(unsigned short subsectorNumber, unsigned short wordNumber, unsigned short *data)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function reads a word(2 bytes) directly from any page of the dataflash,
//!  without using the RAM buffer.
//
//------------------------------------------------------------------------------

void DataFlashReadWord(
                              unsigned short subsectorNumber,  //!< Data is to be read from the page of this subsector
                              unsigned short wordNumber,       //!< Address in the page to start reading from
                              unsigned short *data             //!< This variable contains data after successful read operation on dataflash
                      )
{ 
    // This variable contains the length of  dataFlash commands
    unsigned char commandLength = (unsigned char)0;

    // Buffer contains the data read from dataflash
    unsigned char tempBuffer[2] = {0U,0U};
    // This variable is used as a loop index
    unsigned int byteCounter = (unsigned int)0;
    
   //. //assert(subsectorNumber >= FIRST_HEADER_PAGE); 
    

    // Read from dataflash page
    BuildDataFlashCommand(READ_DATAFLASH_PAGE_COMMAND, (unsigned int)subsectorNumber, (wordNumber << 1),\
        spiBuffer, &commandLength, false);
    // SPI operation to write command bytes and then read the data
    WriteDataFlashCommandBytes(spiBuffer,((unsigned short)commandLength + NBYTES_TO_WRITE), RX_OFFSET, tempBuffer);
    byteCounter = (unsigned int)0;
    *data = (unsigned short)((unsigned short)tempBuffer[byteCounter]<< LEFT_SHIFT_BY_EIGHT) | \
        (unsigned short)tempBuffer[byteCounter+(unsigned char)1];

    (void) IfDataFlashReady();    
}

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
                               )
{
    // This variable contains the length of  dataFlash commands
    unsigned char commandLength = 0u;
    // Temporary variable contains the read value
    unsigned char temp= 0u;
    unsigned short byteAddres = (unsigned short)0;
    // If the subsector number is for event logging write the data to that subsector otherwise write the data to general buffer
    if ( (subsectorNumber >= FIRST_DATAFLASH_SUBSECTOR )&& (subsectorNumber <= (LEGACY_ERRORS_SUBSECTOR + TOTAL_NUMBER_OF_ERRORS)))
    {
        if ( deviceInstalled == SST )
        {
            byteAddres = wordNumber << 1;
            // Latch the write enable bit
            WriteEnableDataflash();
            // Build the command to write the data to the dataflash page
            BuildDataFlashCommand(WRITE_DATAFLASH_PAGE_COMMAND, (unsigned int)subsectorNumber, byteAddres, spiBuffer, \
                &commandLength, false);
            spiBuffer[commandLength] = HIBYTE_WORD16(data);
            // SPI operation to write command bytes
            WriteDataFlashCommandBytes(spiBuffer, ((unsigned short)commandLength + (unsigned char)1),\
                IGNORED_NBYTES, &temp);
            
            IfDataFlashReady();

            // Latch the write enable bit
            WriteEnableDataflash();
            // Build the command to write the data to the dataflash page
            BuildDataFlashCommand(WRITE_DATAFLASH_PAGE_COMMAND, (unsigned int)subsectorNumber,\
                (unsigned short)(byteAddres+(unsigned char)1), spiBuffer, &commandLength, false);
            spiBuffer[commandLength] = LOBYTE_WORD16(data);
            // SPI operation to write command bytes
            WriteDataFlashCommandBytes(spiBuffer, ((unsigned short)commandLength + (unsigned char)1), \
                IGNORED_NBYTES, &temp);
        }
        else
        {
            // Latch the write enable bit
            WriteEnableDataflash();
            // Build the command to write the data to the dataflash page
            BuildDataFlashCommand(WRITE_DATAFLASH_PAGE_COMMAND, (unsigned int)subsectorNumber, \
                (unsigned short)(wordNumber << 1), spiBuffer, &commandLength, false);
            spiBuffer[commandLength] = HIBYTE_WORD16(data);
            spiBuffer[commandLength+(unsigned char)1] = LOBYTE_WORD16(data);
            // SPI operation to write command bytes
            WriteDataFlashCommandBytes(spiBuffer, ((unsigned short)commandLength + NBYTES_TO_WRITE),\
                IGNORED_NBYTES, &temp);
        }

        IfDataFlashReady();
    }
    else
    {
       /* // Checking if page is not same as the current page in general buffer
        if (generalBufferPageNumber != subsectorNumber)
        {
            // Write the data of general buffer to the particular sub sector of dataflash
            DataFlashCommitGeneralBuffer();
            // Transfer the data from the pages of data flash to general buffer
            TransferFlashToBuffer(GENERAL_BUFFER, subsectorNumber) ;
            generalBufferPageNumber = subsectorNumber;
        }
        // Write the two bytes of data to general buffer
        WriteGeneralWord((wordNumber << 1), data);*/
    }
}
//------------------------------------------------------------------------------
//   DataFlashErasePage(unsigned short subsectorNumber)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function erases the subsector of data flash 
//!  Special datalog and eventlog schemes will use erase subsector
//
//------------------------------------------------------------------------------

void DataFlashErasePage(
                        unsigned short subsectorNumber    //!< This variable contains the subsector number which is to erased from dataflash
                        )
{
    // Erase the particular sub sector
    SubsectorErase(subsectorNumber);
}

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
                        bool isForDatalog             //!< This variable contains the status whether the data is read for datalog or not 
                        )
{
    // This variable contains the length of  dataFlash commands
    unsigned char commandLength = 0u;
    // A loop index variable to read the n bytes of data
    unsigned int index = (unsigned int)0;
    // Check that the data doesn't go over a page boundary
    //assert( (byteAddress + nBytes) <= DATAFLASH_SUBSECTOR_SIZE );
    // Checking subsector to build read command from dataFlash page  
    if( isForDatalog == true )
    {
        if(( (byteAddress + nBytes) > DATAFLASH_PAGE_SIZE ))
        {
            nBytes = DATAFLASH_PAGE_SIZE - byteAddress;
            for(index = nBytes; index < (unsigned char)DATALOG_PACKET_SIZE; index++ )
            {
                dataArray[index] = DATALOG_EMPTY_MEMORY_MARKER;
            }
        }
        BuildDataFlashCommand(READ_DATAFLASH_PAGE_COMMAND, (unsigned int)subsectorNumber, byteAddress, \
            spiBuffer, &commandLength, true);
    }
    else
    {
        BuildDataFlashCommand(READ_DATAFLASH_PAGE_COMMAND, (unsigned int)subsectorNumber, byteAddress,\
            spiBuffer, &commandLength, false);
    }

    WriteDataFlashCommandBytes(spiBuffer, (commandLength+nBytes), RX_OFFSET, dataArray);
}

/*
//------------------------------------------------------------------------------
//   DataFlashTrasferFromBackupRAM(unsigned short subsectorNumber, unsigned short startByteNumber, 
//                                     unsigned short endByteNumber)
//
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function transfers data from backup RAM (BURTC register 30 to 93) to dataflash
//
//------------------------------------------------------------------------------

void DataFlashTrasferFromBackupRAM(
                                  unsigned short subsectorNumber,    //!< Subsector number to which data is to transfer
                                  unsigned short startByteNumber,    //!< Start byte number
                                  unsigned short endByteNumber       //!< Last byte number
                                 ) 
{
    // This variable is used to update index
    unsigned short index = 0u;
    // This variable is used to get retention register index
    unsigned int retentionRegisterIndex = 0u;
    // This variable is used to get retention register value
    unsigned int retentionWordData = 0u;
    // This variable is sued to get byte from retention register which needs to be sent to dataflash
    unsigned char byteToTransfer = 0u;
    // This variable is used to get shift number for the byte
    unsigned char byteShiftNumber = 0u;
    // This variable contains the length of dataflash command
    unsigned char comandLength = 0u;
    // Temporary variable contains the read data
    unsigned char temp = 0u;
    // This variable is used to prepare register mask
    unsigned int registerMask = 0u;
    // This variable is used to get masked retention register
    unsigned int maskedRegister = 0u;
    // This variable is used to get shifted retention register
    unsigned int shiftedRegisterValue = 0u;

    // Upadted the loop to run N time for N data values. It was running N+1 time for N data elements before
    // That could result in datalog corruptoion
    for (index = startByteNumber; index < endByteNumber; index++)
    {
        // Set the write enable latch bit
        WriteEnableDataflash();
        // Build the page program command of dataflash
        BuildDataFlashCommand(WRITE_DATAFLASH_PAGE_COMMAND, (unsigned int)subsectorNumber, index,\
          (unsigned char*)spiBuffer, &comandLength, true);
        // Retention register index
        retentionRegisterIndex = BURAM_DATALOG_OFFSET + ((unsigned int)index/4u);
        // Get retention register data from position = (BURAM_DATALOG_OFFSET + index/4) 
        retentionWordData = BURAM_GET_REGISTER(retentionRegisterIndex);
        // Get shift number for the byte of the retention register
        byteShiftNumber = 8u * ( (unsigned char)index % 4u );
        // Prepare register mask
        registerMask = MASK;
        registerMask = registerMask << byteShiftNumber;
        // Mask retention register to get byte which is to be transfered
        maskedRegister = retentionWordData & registerMask;
        // Right shift retention register to get required byte on LSB position 
        shiftedRegisterValue  =  maskedRegister >> byteShiftNumber;
        // Get byte which needs to be sent to dataflash
        byteToTransfer = (unsigned char)shiftedRegisterValue;
        spiBuffer[comandLength] = byteToTransfer;
        // Write the data through SPI
        WriteDataFlashCommandBytes((unsigned char*)spiBuffer, ((unsigned short)comandLength+(unsigned char)1), \
          IGNORED_NBYTES, &temp);
        // Check the busy bit of dataflash
        IfDataFlashReady();         

    }
}
*/
//------------------------------------------------------------------------------
//   DataFlashEraseGeneralBuffer(void) 
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function erases general buffer
//
//------------------------------------------------------------------------------

void DataFlashEraseGeneralBuffer(void)
{
    unsigned int index = 0u;

    for (index = 0u; index < GENERAL_BUFFER_LENGTH; index++)
    {
        generalBuffer.generalBuffer1[index] = 0xFFu;
        generalBuffer.generalBuffer2[index] = 0xFFu;
        generalBuffer.generalBuffer3[index] = 0xFFu;
    }
}

//------------------------------------------------------------------------------
//   DataFlashCommitGeneralBuffer(void)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function writes the data from the general buffer to dataflash if the contents of both are not the same
//
//------------------------------------------------------------------------------

void DataFlashCommitGeneralBuffer(void)
{
    // Validate current generalBufferPageNumber it should be withing general pages of dataFlash
    //assert(generalBufferPageNumber >= FIRST_GENERAL_SUBSECTOR);
    // If no data available exit from function
    if ( generalBufferPageNumber != NO_DATAFLASH_SUBSECTOR )
    {
        // Commit buffer
        //.DataFlashCommitBuffer(GENERAL_BUFFER, generalBufferPageNumber);
        // No data available in general buffer
        generalBufferPageNumber = NO_DATAFLASH_SUBSECTOR;
    }
}

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
                           )
{
    // This variable contains the length of dataFlash commands used in this function
    unsigned char commandLength = 0u;
    // Write retries counter    
    unsigned char numberOfTries = 0u;
    // This variable is used to store the dataflash write operation status is successful or not
    unsigned char isWriteSuccessful = false;
    // Temporary variable contains the read value
    unsigned char temp= 0u;
    // This variable is used as a loop index
    unsigned int tempIndex = (unsigned int)0;
    // For indexing the loop
    unsigned short loopIndex = 0u;
    // For indexing the loop
    unsigned short loopIndex2 = 0u;
    //For indexing the page number
    unsigned char pageNumber = 0u;
    //For the data write
    unsigned char dataWrite[260] = {0u};
    // Check if page is valid
    if ( subsectorNumber != NO_DATAFLASH_SUBSECTOR )
    {
        // Write dataflash operation repeats maximum DATAFLASH_COMMANDS_RETRIES times until it goes successful
        do
        {
            //Reset the watchdog once
            //.HardwareWDTReset();
            // Wait a while for dataflash to be ready
            (void) IfDataFlashReady();
            // Check which buffer is to be copied to dataflash
            // Erase the particular sub sector before writing the data 
            SubsectorErase(subsectorNumber);
            // Check the busy bit of dataflash
            IfDataFlashReady();
            for( pageNumber = 0; pageNumber < 16; pageNumber++ )
            {
                // Latch the write enable bit 
                WriteEnableDataflash();
                // Build the command to write the data to dataflash
                BuildDataFlashCommand(GENERAL_BUFFER_TO_DATAFLASH_COMMAND, (unsigned int)subsectorNumber, (unsigned short)(NEXT_PAGE_OFFSET*pageNumber), dataWrite, &commandLength, false);
                //Get the command length
                loopIndex = commandLength;
                //Get the loopIndex2
                loopIndex2 = 256 * pageNumber;
                while ( loopIndex < (commandLength+256) )
                {
                    dataWrite[loopIndex] = data[loopIndex2];
                    loopIndex++;
                    loopIndex2++;
                }
               // SPI process to write the one byte of data to the dataflash
               WriteDataFlashCommandBytes(dataWrite, ((unsigned short)commandLength+256u), IGNORED_NBYTES, &temp);
               // Check the busy bit of dataflash
               IfDataFlashReady();
            }
            // Check if write operation was successful
            if(IfDataFlashReady() == (int) ERRORCODE_ENUM_NO_ERROR)
            {
                isWriteSuccessful = true;
            }
            else
            {
                isWriteSuccessful = false;
            }
            numberOfTries++;
        } // Repeat the copy process DATAFLASH_COMMANDS_RETRIES times or until it is not successful
        while ( (isWriteSuccessful == false) && (numberOfTries < DATAFLASH_COMMANDS_RETRIES) );
        // If number of erase retries exceeds DATAFLASH_COMMANDS_RETRIES then this is an error
        if ( numberOfTries >= DATAFLASH_COMMANDS_RETRIES )
        {
            //.ErrorLogWrite(ERRORCODE_ENUM_DFLASH_BUF_COPY_FAIL, ERRORTYPE_ENUM_CRITICAL);
        }
    }
}

//------------------------------------------------------------------------------
//  DataFlashDebugWrite(unsigned short sectorNumber,unsigned short wordAddress,unsigned short *dataWord)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function writes the data in DataFlahs for debugging purpose.
//------------------------------------------------------------------------------

void DataFlashDebugWrite(
                         unsigned short sectorNumber, //!< Subsector number to which data is to be transfered
                         unsigned short wordAddress,  //!< Word address to which data is to be tranfered
                         unsigned short *dataWord     //!< data to be written 
                         )
{
    unsigned short data=*dataWord;  
    if(currentSubSector!=sectorNumber)
    {
        //if buffer not update then  just read teh buffer do not write it 
        if(isDebugBufferUpadted==true)
        {
            DataFlashCommitDebugBuffer();
        }
        currentSubSector=sectorNumber;
        DataFlashReadArray(currentSubSector,0u,dataBuffer,DATAFLASH_SUBSECTOR_SIZE,false);
        isDebugBufferUpadted=true;
    }
    else
    {
      
    } 
    dataBuffer[wordAddress]=HIBYTE_WORD16(data);
    dataBuffer[wordAddress+1u]=LOBYTE_WORD16(data);

}

//------------------------------------------------------------------------------
//  DataFlashDebugRead(unsigned short sectorNumber,unsigned short wordAddress,unsigned short *data)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function Read the data in DataFlahs for debugging  purpose.
//------------------------------------------------------------------------------

void DataFlashDebugRead( 
                        unsigned short sectorNumber, //!< Subsector number from which data is to be read
                        unsigned short wordAddress,  //!< Word Address which is to be read 
                        unsigned short *data         //!< data which is read 
                        )   
{
    if(currentSubSector!=sectorNumber)
    {
        if(isDebugBufferUpadted==true)
        {
            DataFlashCommitDebugBuffer();        
        }
        currentSubSector=sectorNumber;
        DataFlashReadArray(currentSubSector,0u,dataBuffer,DATAFLASH_SUBSECTOR_SIZE,false);
        isDebugBufferUpadted=true;
        *data=(unsigned short)((unsigned short)dataBuffer[wordAddress]<< LEFT_SHIFT_BY_EIGHT) | \
            (unsigned short)dataBuffer[wordAddress+(unsigned char)1];     
    }
    else
    {
        *data=(unsigned short)((unsigned short)dataBuffer[wordAddress]<< LEFT_SHIFT_BY_EIGHT) | \
            (unsigned short)dataBuffer[wordAddress+(unsigned char)1];
    }     
}

//------------------------------------------------------------------------------
//   DataFlashCommitDebugBuffer(unsigned short modbusRegisterNumber, unsigned short data)
//   
//   Author:   Ali Zulqarnain Anjum
//   Date:     2016/12/02
//
//!  This function commit the buffer in dataflash debug mode
//
//------------------------------------------------------------------------------

void DataFlashCommitDebugBuffer(void)
{
    // This variable contains the length of  dataFlash commands
    unsigned char commandLength = 0u;
    unsigned int index;
    unsigned short byteAddres;
    // Temporary variable contains the read value
    unsigned char temp[6] = {0u};
//    unsigned short address ;  
//    int FlashStatus = 1;
//    unsigned short waitCount = 0u;
    DataFlashErasePage(currentSubSector);        

    // Check if buffer not update then  just read teh buffer do not write it 
    if(isDebugBufferUpadted==true)
    {
        address=0u;
        for(index=0u; index<DATAFLASH_SUBSECTOR_SIZE; index+=2u)
        {
            if ( deviceInstalled == SST )
            {
                byteAddres = address << 1;
                // Latch the write enable bit
                WriteEnableDataflash();
                // Build the command to write the data to the dataflash page
                BuildDataFlashCommand(WRITE_DATAFLASH_PAGE_COMMAND, (unsigned int)currentSubSector, \
                    byteAddres, spiBuffer, &commandLength, false);
                // build spi Buffer
                spiBuffer[commandLength] = dataBuffer[index];
                // SPI operation to write command bytes
                WriteDataFlashCommandBytes(spiBuffer, ((unsigned short)commandLength + (unsigned char)1), \
                    IGNORED_NBYTES, temp);
                // Check the busy bit of dataflash
                IfDataFlashReady();
                // Latch the write enable bit
                WriteEnableDataflash();
                // Build the command to write the data to the dataflash page
                BuildDataFlashCommand(WRITE_DATAFLASH_PAGE_COMMAND, (unsigned int)currentSubSector, \
                    (unsigned short)(byteAddres+(unsigned char)1), spiBuffer, &commandLength, false);
                // build spi Buffer
                spiBuffer[commandLength] = dataBuffer[index+1u];
                // SPI operation to write command bytes
                WriteDataFlashCommandBytes(spiBuffer, ((unsigned short)commandLength + (unsigned char)1), \
                    IGNORED_NBYTES, temp);
            }
            else
            {
                // Latch the write enable bit
                WriteEnableDataflash();
                // Build the command to write the data to the dataflash page
                BuildDataFlashCommand(WRITE_DATAFLASH_PAGE_COMMAND, (unsigned int)currentSubSector, \
                    (unsigned short)(address << 1), spiBuffer, &commandLength, false);
                spiBuffer[commandLength] = dataBuffer[index];
                spiBuffer[commandLength+(unsigned char)1] = dataBuffer[index+1u];
                // SPI operation to write command bytes
                WriteDataFlashCommandBytes(spiBuffer, ((unsigned short)commandLength + NBYTES_TO_WRITE), \
                    IGNORED_NBYTES, temp);
            }
            IfDataFlashReady();
            address++;
        }
    }
}
/*
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
                                 )
{
#ifndef ENABLE_INSTRUMENT_CORRUPTION   //#VAUG-1146     
    unsigned char commandLength = (unsigned char)0;
    // Write retries counter    
    unsigned char numberOfTries = (unsigned char)0;
    // This variable is used to store the dataflash write operation status is successful or not
    unsigned char isWriteSuccessful = false;
    // Temporary variable contains the read value
    unsigned char temp= (unsigned char)0;
    // This variable is used as a loop index
    unsigned int tempIndex = (unsigned int)0;    
    unsigned short readbackValue = 0u;
    bool isWriteOk = true;    
    unsigned short currentMagicNumPage = 0u;
    // This variable is used to calculate the checksum value of data which is to be read
    unsigned short calculatedCheckSum = 0u;   
    // This variable contains the byte address of instrument parameter page to start reading from
    unsigned short registerCounter = 0u;     
    // This variable contains the data after successful read operation
    unsigned short registerValue = 0u;    
    
    ( pageToWrite == INST_PARAM_DATAFLASH_BLOCK_1 ) ? ( currentMagicNumPage = INST_PARAM_DATAFLASH_MAGIC_1_BLOCK ) : ( currentMagicNumPage = INST_PARAM_DATAFLASH_MAGIC_2_BLOCK );
    
    // Write dataflash operation repeats maximum DATAFLASH_COMMANDS_RETRIES times until it goes successful
    do
    {  
        // Erase the particular sub sector before writing the data 
        SubsectorErase(pageToWrite);
        // Check the busy bit of dataflash        
        IfDataFlashReady();
        
        // Write the data from the instrument parameter backup page in RAM to the correct subsector of dataflash.
        // Don't write the entire page - only the addressses that are being used.
        for ( tempIndex = 2u; tempIndex < ( (NUMBER_OF_INSTURMENT_PARAMETERS * 2u) ); tempIndex++ )
        {
            // Latch the write enable bit 
            WriteEnableDataflash();            
            // Build the command to write the data to dataflash
            BuildDataFlashCommand(GENERAL_BUFFER_TO_DATAFLASH_COMMAND, (unsigned int)pageToWrite, \
                (unsigned short)( tempIndex ), spiBuffer, &commandLength, false);            
            spiBuffer[commandLength] = instrumentParamArray[tempIndex];            
            // SPI process to write the one byte of data to the dataflash
            WriteDataFlashCommandBytes(spiBuffer, ((unsigned short)commandLength + (unsigned char)1), \
                IGNORED_NBYTES, &temp);            
            // Check the busy bit of dataflash
            IfDataFlashReady();
        }        
        
        // Check if write operation was successful
        if(IfDataFlashReady() == (int) ERRORCODE_ENUM_NO_ERROR)
        {
            isWriteSuccessful = true;
        }
        else
        {
            isWriteSuccessful = false;
        }
        numberOfTries++;
    } while ( (isWriteSuccessful == false) && (numberOfTries < DATAFLASH_COMMANDS_RETRIES) ); // Repeat the copy process DATAFLASH_COMMANDS_RETRIES times or until it is not successful    
    
    
    // Reset calculated checksum 
    calculatedCheckSum = 0u;
    for ( registerCounter = 1u; registerCounter <= (unsigned short) LAST_INSTRUMENT_PARAMETER; registerCounter++ )
    {
        // Read a word(2 bytes) from instrument parameter page of dataflash.        
        // Revised this function to handle the two instrument parameter pages (previously, there was only one).
        DataFlashReadWord( pageToWrite, registerCounter, &registerValue );
        
        // Convert from register (word) address to byte address
        // Calculate the checksum of read value
        calculatedCheckSum += registerValue;
    }
    
    // Write the checksum high and low bytes.
    // Latch the write enable bit 
    WriteEnableDataflash();
    // Build the command to write the data to dataflash
    BuildDataFlashCommand(GENERAL_BUFFER_TO_DATAFLASH_COMMAND, (unsigned int)pageToWrite, \
        (unsigned short)(INSTRUMENT_PARAMETER_CHECKSUM_WORD_ADDRESS * 2u), spiBuffer, &commandLength, false);    
    spiBuffer[commandLength] = HIBYTE_WORD16(calculatedCheckSum);
    spiBuffer[commandLength + ( unsigned char ) 1 ] = LOBYTE_WORD16(calculatedCheckSum);    
    // SPI process to write the one byte of data to the dataflash
    WriteDataFlashCommandBytes(spiBuffer, ((unsigned short)commandLength+NBYTES_TO_WRITE), IGNORED_NBYTES, &temp);
    // Check the busy bit of dataflash 
    IfDataFlashReady();
    
    // If number of erase retries exceeds DATAFLASH_COMMANDS_RETRIES then this is an error
    if ( numberOfTries >= DATAFLASH_COMMANDS_RETRIES )
    {
        isWriteOk = false; 
    }    
    
    // Check the first word and the checksum as a quick method to confirm that the write worked.
    DataFlashReadWord(currentMagicNumPage, (unsigned short)IP_ENUM_MAGIC_NUMBER, &readbackValue);
    
    if ( ( HIBYTE_WORD16(readbackValue) != instrumentParamArray[0] ) ||\
       ( LOBYTE_WORD16(readbackValue) != instrumentParamArray[1] ) )
    {
        isWriteOk = false;   
    }
    // Verify Checksum
    DataFlashReadWord( pageToWrite, INSTRUMENT_PARAMETER_CHECKSUM_WORD_ADDRESS, &readbackValue );
    
    if (readbackValue != calculatedCheckSum)
    {
        isWriteOk = false;   
    }     
    
    // If any part of the write to Flash failed, throw a System Alarm.
    if ( isWriteOk == false )
    {
        ErrorLogWrite(ERRORCODE_ENUM_DFLASH_BUF_COPY_FAIL, ERRORTYPE_ENUM_CRITICAL);   
    }
#endif     
}

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

void DataFlashBackupIparams( void )
{
    unsigned short currentIparamPage = 0u;
    unsigned short pageToWrite = 0u;
    
    // Write and clear out the general buffer before writing the instrument parameters to Dataflash.
    DataFlashCommitGeneralBuffer();
    
    currentIparamPage = InstrumentParametersGetCurrentPage();
    
    // If the instrument is not currently configured, write to the first instrument parameter page.
    if ( isInstrumentConfigured == false )
    {
        pageToWrite = INST_PARAM_DATAFLASH_BLOCK_1;
    }
    // Write the instrument parameters to the page that is NOT the current page.
    else if ( currentIparamPage == INST_PARAM_DATAFLASH_BLOCK_1 )
    {
        pageToWrite = INST_PARAM_DATAFLASH_BLOCK_2;
    }
    else
    {
        pageToWrite = INST_PARAM_DATAFLASH_BLOCK_1;        
    }
    
    // Only backup if parameters are loaded
    if ( (areInstrumentParametersLoaded == true ) && ( isInstChecksumError == false ) && \
       ( isInstrumentConfigured == true ) )
    {
        DataFlashBackupIparamsToPage(pageToWrite);
        
        // Also backup the old page from ram buffer just in case if any parameter changed in ram
        if ( isInstrumentConfigured == true )
        {
            DataFlashBackupIparamsToPage(currentIparamPage);
        }
    }
    
    InstrumentParametersFindValidPage();
}
*/
//------------------------------------------------------------------------------
//   DataFlashWriteWord( unsigned short subsectorNumber, unsigned short wordNumber,  unsigned short data )
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
                            )
{   
    // This variable contains the length of  dataFlash commands
    unsigned char commandLength = (unsigned char)0;
    // Temporary variable contains the read value
    unsigned char temp= (unsigned char)0;
    
    // Latch the write enable bit
    WriteEnableDataflash();
    // Build the command to write the data to the dataflash page
    BuildDataFlashCommand(WRITE_DATAFLASH_PAGE_COMMAND, (unsigned int)subsectorNumber, \
        (unsigned short)(wordNumber << 1), spiBuffer, &commandLength, false);
    spiBuffer[commandLength] = HIBYTE_WORD16(data);
    spiBuffer[commandLength+(unsigned char)1] = LOBYTE_WORD16(data);
    // SPI operation to write command bytes
    WriteDataFlashCommandBytes(spiBuffer, ((unsigned short)commandLength + NBYTES_TO_WRITE),\
        IGNORED_NBYTES, &temp);
    
    // Check the busy bit of dataflash
    IfDataFlashReady();
}
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
                )
{
    if ( spiDevice == SPI_DATAFLASH )
    {
       // Enable SSI3 module
       SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
      // Configure respective GPIO pins for FSS(PB4),CLK(PB5), MISO(PE4) and MOSI(PE5)
      GPIOPinConfigure(GPIO_PQ1_SSI3FSS);
      GPIOPinConfigure(GPIO_PQ0_SSI3CLK);
      GPIOPinConfigure(GPIO_PQ2_SSI3XDAT0);
      GPIOPinConfigure(GPIO_PQ3_SSI3XDAT1);
      GPIOPinTypeSSI(GPIO_PORTQ_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    }
    else
    {
      //TODO: add for other device as well
    }
      EK_TM4C1294XL_initDMA();
      SPI_init();
}

//------------------------------------------------------------------------------
//   unsigned char GetDataflashID(void)
//
//   Author:   Muhammad Shuaib
//   Date:     2016/17/12
//
//!  This function is used by other modules to get the installed dataflash type
//
//------------------------------------------------------------------------------

unsigned char GetDataflashID(void)
{
    return deviceInstalled;
}

//==============================================================================
//  End Of File
//==============================================================================
