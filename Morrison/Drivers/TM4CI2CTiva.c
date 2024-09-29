//==============================================================================
// Start of file
//==============================================================================
//
//  TM4CI2CTiva.c
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
//  Source:        TM4CI2CTiva.c
//
//  Project:       Morrison
//
//  Author:        Muhammad Shuaib
//
//  Date:          2016/21/11
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This is I2C Device Driver module.
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//
// Revision: 1.0 2016/21/11 Muhammad Shuaib
//           Initial Version
//
//==============================================================================

//==============================================================================
//  INCLUDES
//==============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <inc/hw_types.h>
#include <driverlib/i2c.h>
#include "TM4CI2CTiva.h"
//
//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================

//==============================================================================
// Specific I2C CMD MACROs that are not defined in I2C.h are defined here. Their
// equivalent values are taken from the existing MACROs in I2C.h
//==============================================================================
 
#ifndef I2C_MASTER_CMD_BURST_RECEIVE_START_NACK
#define I2C_MASTER_CMD_BURST_RECEIVE_START_NACK  I2C_MASTER_CMD_BURST_SEND_START
#endif

#ifndef I2C_MASTER_CMD_BURST_RECEIVE_STOP
#define I2C_MASTER_CMD_BURST_RECEIVE_STOP        I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP
#endif

#ifndef I2C_MASTER_CMD_BURST_RECEIVE_CONT_NACK
#define I2C_MASTER_CMD_BURST_RECEIVE_CONT_NACK   I2C_MASTER_CMD_BURST_SEND_CONT
#endif


static const uint32_t bitRate[] = {
    false,  //  I2C_100kHz = 0 
    true    //  I2C_400kHz = 1 
};

//==============================================================================
// LOCAL function PROTOTYPES
//==============================================================================

static void  I2CTiva_blockingCallback(I2C_Handle handle, I2C_Transaction *msg,
                                      bool transferStatus);


//==============================================================================
// LOCAL FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
// LOCAL FUNCTIONS DEFINITIONS
//==============================================================================

//==============================================================================
// GLOBAL  FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
// LOCAL FUNCTIONS IMPLEMENTATION
//==============================================================================

//==============================================================================
//
// static void I2CTiva_primeTransfer(I2CTiva_Object  *object,I2CTiva_HWAttrs const *hwAttrs,
//                                  I2C_Transaction *transaction)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function initializes the I2C
//
//==============================================================================

static void I2CTiva_primeTransfer(I2CTiva_Object  *object,
                                  I2CTiva_HWAttrs const *hwAttrs,
                                  I2C_Transaction *transaction)
{
    // Store the new internal counters and pointers 
    object->currentTransaction = transaction;

    object->writeBufIdx = transaction->writeBuf;
    object->writeCountIdx = transaction->writeCount;

    object->readBufIdx = transaction->readBuf;
    object->readCountIdx = transaction->readCount;

    Log_print2(Diags_USER1,
               "I2C:(%p) Starting transaction to slave: 0x%x",
               hwAttrs->baseAddr,
               object->currentTransaction->slaveAddress);

    // Start transfer in Transmit mode 
    if (object->writeCountIdx) {
        // Specify the I2C slave address 
        I2CMasterSlaveAddrSet(hwAttrs->baseAddr,
                              object->currentTransaction->slaveAddress, false);
        // Update the I2C mode 
        object->mode = I2CTiva_WRITE_MODE;

        Log_print3(Diags_USER2,
                   "I2C:(%p) I2CTiva_IDLE_MODE: Data to write: 0x%x; To Slave: 0x%x",
                   hwAttrs->baseAddr,
                   *(object->writeBufIdx),
                   object->currentTransaction->slaveAddress);

        // Write data contents into data register 
        I2CMasterDataPut(hwAttrs->baseAddr, *((object->writeBufIdx)++));

        // Start the I2C transfer in master transmit mode 
        I2CMasterControl(hwAttrs->baseAddr, I2C_MASTER_CMD_BURST_SEND_START);

        Log_print1(Diags_USER2,
                   "I2C:(%p) I2CTiva_IDLE_MODE: -> I2CTiva_WRITE_MODE; Writing w/ START",
                   hwAttrs->baseAddr);
    }

    // Start transfer in Receive mode 
    else {
        // Specify the I2C slave address 
        I2CMasterSlaveAddrSet(hwAttrs->baseAddr,
                              object->currentTransaction->slaveAddress,
                              true);

        // Update the I2C mode 
        object->mode = I2CTiva_READ_MODE;

        if (object->readCountIdx < 2) {
            // Start the I2C transfer in master receive mode 
            I2CMasterControl(hwAttrs->baseAddr,
                             I2C_MASTER_CMD_BURST_RECEIVE_START_NACK);

            Log_print1(Diags_USER2,
                       "I2C:(%p) I2CTiva_IDLE_MODE: -> I2CTiva_READ_MODE; Reading w/ "
                       "NACK",
                       hwAttrs->baseAddr);
        }
        else {
            // Start the I2C transfer in master receive mode 
            I2CMasterControl(hwAttrs->baseAddr,
                    I2C_MASTER_CMD_BURST_RECEIVE_START);

            Log_print1(Diags_USER2,
                       "I2C:(%p) I2CTiva_IDLE_MODE: -> I2CTiva_READ_MODE; Reading w/ ACK",
                       hwAttrs->baseAddr);
        }
    }
}

//==============================================================================
// GLOBAL  function PROTOTYPES
//==============================================================================

void         I2CTiva_close(I2C_Handle handle);
int          I2CTiva_control(I2C_Handle handle, unsigned int cmd, void *arg);
void         I2CTiva_init(I2C_Handle handle);
I2C_Handle   I2CTiva_open(I2C_Handle handle, I2C_Params *params);
bool         I2CTiva_transfer(I2C_Handle handle, I2C_Transaction *transaction);
static void  I2CTiva_primeTransfer(I2CTiva_Object        *object,
                                   I2CTiva_HWAttrs const *hwAttrs,
                                   I2C_Transaction       *transaction);
// I2C function table for I2CTiva implementation 
const I2C_FxnTable I2CTiva_fxnTable = {
    I2CTiva_close,
    I2CTiva_control,
    I2CTiva_init,
    I2CTiva_open,
    I2CTiva_transfer
};

//==============================================================================
// GLOBAL  FUNCTIONS IMPLEMENTATIONS
//==============================================================================

//==============================================================================
//
//  void I2CTiva_close(I2C_Handle handle)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function closes I2C driver
//
//============================================================================== 

void I2CTiva_close(I2C_Handle handle)
{
    I2CTiva_Object         *object = handle->object;
    I2CTiva_HWAttrs const  *hwAttrs = handle->hwAttrs;

    // Check to see if a I2C transaction is in progress 
    Assert_isTrue(object->headPtr == NULL, NULL);

    // Mask I2C interrupts 
    I2CMasterIntDisable(hwAttrs->baseAddr);

    // Disable the I2C Master 
    I2CMasterDisable(hwAttrs->baseAddr);

    Hwi_destruct(&(object->hwi));
    Semaphore_destruct(&(object->mutex));
    if (object->transferMode == I2C_MODE_BLOCKING) {
        Semaphore_destruct(&(object->transferComplete));
    }

    object->isOpen = false;

    Log_print1(Diags_USER1, "I2C: Object closed 0x%x", hwAttrs->baseAddr);

    return;
}

//==============================================================================
//
//  static void I2CTiva_completeTransfer(I2C_Handle handle)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function performs the I2C tranfser
//
//============================================================================== 

static void I2CTiva_completeTransfer(I2C_Handle handle)
{
    // Get the pointer to the object 
    I2CTiva_Object         *object = handle->object;

    Log_print1(Diags_USER2, "I2C:(%p) ISR Transfer Complete",
               ((I2CTiva_HWAttrs const *)(handle->hwAttrs))->baseAddr);

    
    // Perform callback in a HWI context, thus any tasks or SWIs
    //  made ready to run won't start until the interrupt has
    // finished
     
    object->transferCallbackFxn(handle, object->currentTransaction,
                                !((bool)object->mode));

    // See if we need to process any other transactions 
    if (object->headPtr == object->tailPtr) {
        // No other transactions need to occur 
        object->currentTransaction = NULL;
        object->headPtr = NULL;

        Log_print1(Diags_USER2,
                   "I2C:(%p) ISR No other I2C transaction in queue",
                   ((I2CTiva_HWAttrs const *)(handle->hwAttrs))->baseAddr);
    }
    else {
        // Another transfer needs to take place 
        object->headPtr = object->headPtr->nextPtr;

        Log_print2(Diags_USER2,
                   "I2C:(%p) ISR Priming next I2C transaction (%p) from queue",
                   ((I2CTiva_HWAttrs const *)(handle->hwAttrs))->baseAddr,
                   (UArg)object->headPtr);

        I2CTiva_primeTransfer(object, (I2CTiva_HWAttrs const *)handle->hwAttrs,
                              object->headPtr);
    }
}

//==============================================================================
//
//  int I2CTiva_control(I2C_Handle handle, unsigned int cmd, void *arg)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function is used for sending control data to I2C module
//
//============================================================================== 

int I2CTiva_control(I2C_Handle handle, unsigned int cmd, void *arg)
{
	// No implementation yet 
	return (I2C_STATUS_UNDEFINEDCMD);
}

//==============================================================================
//
//  static void I2CTiva_hwiFxn(UArg arg)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! Hwi interrupt handler to service the I2C peripheral.The handler is a generic handler for a I2C object.
//
//============================================================================== 

static void I2CTiva_hwiFxn(UArg arg)
{
    // Get the pointer to the object and hwAttrs 
    I2CTiva_Object         *object = ((I2C_Handle)arg)->object;
    I2CTiva_HWAttrs const  *hwAttrs = ((I2C_Handle)arg)->hwAttrs;
    uint32_t                errStatus;

    // Get the interrupt status of the I2C controller 
    errStatus = I2CMasterErr(hwAttrs->baseAddr);

    // Clear interrupt source to avoid additional interrupts 
    I2CMasterIntClear(hwAttrs->baseAddr);

    // Check for I2C Errors 
    if ((errStatus == I2C_MASTER_ERR_NONE) || (object->mode == I2CTiva_ERROR)) {
        // No errors, now check what we need to do next 
        switch (object->mode) {
            //
            //  ERROR case is OK because if an Error is detected, a STOP bit is
            //  sent; which in turn will call another interrupt. This interrupt
            //  call will then post the transferComplete semaphore to unblock the
            //  I2C_transfer function
             
            case I2CTiva_ERROR:
            case I2CTiva_IDLE_MODE:
                I2CTiva_completeTransfer((I2C_Handle) arg);
                break;

            case I2CTiva_WRITE_MODE:
                // Decrement write Counter 
                object->writeCountIdx--;

                // Check if more data needs to be sent 
                if (object->writeCountIdx) {
                    Log_print3(Diags_USER2,
                               "I2C:(%p) ISR I2CTiva_WRITE_MODE: Data to write: "
                               "0x%x; To slave: 0x%x",
                               hwAttrs->baseAddr,
                               *(object->writeBufIdx),
                               object->currentTransaction->slaveAddress);

                    // Write data contents into data register 
                    I2CMasterDataPut(hwAttrs->baseAddr, *(object->writeBufIdx));
                    object->writeBufIdx++;

                    if ((object->writeCountIdx < 2) && !(object->readCountIdx)) {
                        // Everything has been sent, nothing to receive 
                        // Next state: Idle mode 
                        object->mode = I2CTiva_IDLE_MODE;

                        // Send last byte with STOP bit 
                        I2CMasterControl(hwAttrs->baseAddr,
                                         I2C_MASTER_CMD_BURST_SEND_FINISH);

                        Log_print1(Diags_USER2,
                                   "I2C:(%p) ISR I2CTiva_WRITE_MODE: ACK received; "
                                   "Writing w/ STOP bit",
                                   hwAttrs->baseAddr);
                    }
                    else {
                        //
                        //   Either there is more date to be transmitted or some
                        //  data needs to be received next
                         
                        I2CMasterControl(hwAttrs->baseAddr,
                                         I2C_MASTER_CMD_BURST_SEND_CONT);

                        Log_print1(Diags_USER2,
                                   "I2C:(%p) ISR I2CTiva_WRITE_MODE: ACK received; "
                                   "Writing",
                                   hwAttrs->baseAddr);
                    }
                }

                // At this point, we know that we need to receive data 
                else {
                    //
                    //   We need to check after we are done transmitting data, if
                    //   we need to receive any data.
                    //   In a corner case when we have only one byte transmitted
                    //  and no data to receive, the I2C will automatically send
                    //  the STOP bit. In other words, here we only need to check
                    //  if data needs to be received. If so, how much.
                     
                    if (object->readCountIdx) {
                        // Next state: Receive mode 
                        object->mode = I2CTiva_READ_MODE;

                        // Switch into Receive mode 
                        I2CMasterSlaveAddrSet(hwAttrs->baseAddr,
                                              object->currentTransaction->slaveAddress,
                                              true);

                        if (object->readCountIdx > 1) {
                            // Send a repeated START 
                            I2CMasterControl(hwAttrs->baseAddr,
                                             I2C_MASTER_CMD_BURST_RECEIVE_START);

                            Log_print1(Diags_USER2,
                                       "I2C:(%p) ISR I2CTiva_WRITE_MODE: -> "
                                       "I2CTiva_READ_MODE; Reading w/ RESTART and ACK",
                                       hwAttrs->baseAddr);
                        }
                        else {
                            //
                             //   Send a repeated START with a NACK since it's the
                             //   last byte to be received.
                             //   I2C_MASTER_CMD_BURST_RECEIVE_START_NACK is
                             //   is locally defined because there is no macro to
                             //   receive data and send a NACK after sending a
                             //   start bit (0x00000003)
                             
                            I2CMasterControl(hwAttrs->baseAddr,
                                             I2C_MASTER_CMD_BURST_RECEIVE_START_NACK);

                            Log_print1(Diags_USER2,
                                       "I2C:(%p) ISR I2CTiva_WRITE_MODE: -> "
                                       "I2CTiva_READ_MODE; Reading w/ RESTART and NACK",
                                       hwAttrs->baseAddr);
                        }
                    }
                    else {
                        // Done with all transmissions 
                        object->mode = I2CTiva_IDLE_MODE;
                        //
                         //   No more data needs to be received, so follow up with
                         //   a STOP bit
                         //   Again, there is no equivalent macro (0x00000004) so
                         //   I2C_MASTER_CMD_BURST_RECEIVE_STOP is used.
                         
                        I2CMasterControl(hwAttrs->baseAddr,
                                         I2C_MASTER_CMD_BURST_RECEIVE_STOP);

                        Log_print1(Diags_USER2,
                                   "I2C:(%p) ISR I2CTiva_WRITE_MODE: -> "
                                   "I2CTiva_IDLE_MODE; Sending STOP bit",
                                   hwAttrs->baseAddr);
                    }
                }
                break;

            case I2CTiva_READ_MODE:
                // Save the received data 
                *(object->readBufIdx) = I2CMasterDataGet(hwAttrs->baseAddr);

                Log_print2(Diags_USER2,
                           "I2C:(%p) ISR I2CTiva_READ_MODE: Read data byte: 0x%x",
                           hwAttrs->baseAddr,
                           *(object->readBufIdx));

                object->readBufIdx++;

                // Check if any data needs to be received 
                object->readCountIdx--;
                if (object->readCountIdx) {
                    if (object->readCountIdx > 1) {
                        // More data to be received 
                        I2CMasterControl(hwAttrs->baseAddr,
                                         I2C_MASTER_CMD_BURST_RECEIVE_CONT);

                        Log_print1(Diags_USER2,
                                   "I2C:(%p) ISR I2CTiva_READ_MODE: Reading w/ ACK",
                                   hwAttrs->baseAddr);
                    }
                    else {
                        //
                         //   Send NACK because it's the last byte to be received
                         //   There is no NACK macro equivalent (0x00000001) so
                         //   I2C_MASTER_CMD_BURST_RECEIVE_CONT_NACK is used
                         
                        I2CMasterControl(hwAttrs->baseAddr,
                                         I2C_MASTER_CMD_BURST_RECEIVE_CONT_NACK);

                        Log_print1(Diags_USER2,
                                   "I2C:(%p) ISR I2CTiva_READ_MODE: Reading w/ NACK",
                                   hwAttrs->baseAddr);
                    }
                }
                else {
                    // Next state: Idle mode 
                    object->mode = I2CTiva_IDLE_MODE;

                    //
                     //   No more data needs to be received, so follow up with a
                     //   STOP bit
                     //   Again, there is no equivalent macro (0x00000004) so
                     //   I2C_MASTER_CMD_BURST_RECEIVE_STOP is used
                     
                    I2CMasterControl(hwAttrs->baseAddr,
                                     I2C_MASTER_CMD_BURST_RECEIVE_STOP);

                    Log_print1(Diags_USER2,
                               "I2C:(%p) ISR I2CTiva_READ_MODE: -> I2CTiva_IDLE_MODE; "
                               "Sending STOP bit",
                               hwAttrs->baseAddr);
                }

                break;

            default:
                object->mode = I2CTiva_ERROR;
                break;
        }

    }
    else {
        // Some sort of error happened! 
        object->mode = I2CTiva_ERROR;

        if (errStatus & (I2C_MASTER_ERR_ARB_LOST | I2C_MASTER_ERR_ADDR_ACK)) {
            I2CTiva_completeTransfer((I2C_Handle) arg);
        }
        else {
        // Try to send a STOP bit to end all I2C communications immediately 
        //
         //   I2C_MASTER_CMD_BURST_SEND_ERROR_STOP -and-
         //   I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP
         //   have the same values
         
            I2CMasterControl(hwAttrs->baseAddr,
                             I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
        }

        Log_print2(Diags_USER1,
                   "I2C:(%p) ISR I2C Bus fault (Status Reg: 0x%x)",
                   hwAttrs->baseAddr,
                   errStatus);
    }

    return;
}


//==============================================================================
//
//  void I2CTiva_init(I2C_Handle handle)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function initializes the I2C driver
//
//==============================================================================

void I2CTiva_init(I2C_Handle handle)
{
    // Mark the object as available 
    ((I2CTiva_Object *)(handle->object))->isOpen = false;
}

//==============================================================================
//
//  void I2CTiva_init(I2C_Handle handle)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function opens the I2C driver
//
//==============================================================================

I2C_Handle I2CTiva_open(I2C_Handle handle, I2C_Params *params)
{
    unsigned int                key;
    Types_FreqHz                freq;
    I2CTiva_Object             *object = handle->object;
    I2CTiva_HWAttrs const      *hwAttrs = handle->hwAttrs;
    union {
        Semaphore_Params        semParams;
        Hwi_Params              hwiParams;
    } paramsUnion;

    // Determine if the device index was already opened 
    key = Hwi_disable();
    if(object->isOpen == true){
        Hwi_restore(key);
        return (NULL);
    }

    // Mark the handle as being used 
    object->isOpen = true;
    Hwi_restore(key);

    // Save parameters 
    object->transferMode = params->transferMode;
    object->transferCallbackFxn = params->transferCallbackFxn;

    // Create Hwi object for this I2C peripheral 
    Hwi_Params_init(&(paramsUnion.hwiParams));
    paramsUnion.hwiParams.arg = (UArg)handle;
    paramsUnion.hwiParams.priority = hwAttrs->intPriority;
    Hwi_construct(&(object->hwi), hwAttrs->intNum, I2CTiva_hwiFxn,
                  &(paramsUnion.hwiParams), NULL);

    //
     //   Create threadsafe handles for this I2C peripheral
     //   Semaphore to provide exclusive access to the I2C peripheral
     
    Semaphore_Params_init(&(paramsUnion.semParams));
    paramsUnion.semParams.mode = Semaphore_Mode_BINARY;
    Semaphore_construct(&(object->mutex), 1, &(paramsUnion.semParams));

    //
     //   Store a callback function that posts the transfer complete
     //   semaphore for synchronous mode
     
    if (object->transferMode == I2C_MODE_BLOCKING) {
        //
         //   Semaphore to cause the waiting task to block for the I2C
         //   to finish
         
        Semaphore_construct(&(object->transferComplete), 0,
                            &(paramsUnion.semParams));

        // Store internal callback function 
        object->transferCallbackFxn = I2CTiva_blockingCallback;
    }
    else {
        // Check to see if a callback function was defined for async mode 
        Assert_isTrue(object->transferCallbackFxn != NULL, NULL);
    }

    // Specify the idle state for this I2C peripheral 
    object->mode = I2CTiva_IDLE_MODE;

    // Clear the head pointer 
    object->headPtr = NULL;
    object->tailPtr = NULL;

    Log_print1(Diags_USER1, "I2C: Object created 0x%x", hwAttrs->baseAddr);

    // Set the I2C configuration 
    BIOS_getCpuFreq(&freq);
    I2CMasterInitExpClk(hwAttrs->baseAddr, freq.lo, bitRate[params->bitRate]);

    // Clear any pending interrupts 
    I2CMasterIntClear(hwAttrs->baseAddr);

    // Enable the I2C Master for operation 
    I2CMasterEnable(hwAttrs->baseAddr);

    // Unmask I2C interrupts 
    I2CMasterIntEnable(hwAttrs->baseAddr);

    // Return the address of the handle 
    return (handle);
}


//==============================================================================
//
//  void I2CTiva_init(I2C_Handle handle)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function sets the control variable of the UART driver
//
//==============================================================================

bool I2CTiva_transfer(I2C_Handle handle, I2C_Transaction *transaction)
{
    unsigned int            key;
    bool                    ret = false;
    I2CTiva_Object         *object = handle->object;
    I2CTiva_HWAttrs const  *hwAttrs = handle->hwAttrs;

    // Check if anything needs to be written or read 
    if ((!transaction->writeCount) && (!transaction->readCount)) {
        // Nothing to write or read 
        return (ret);
    }

    if (object->transferMode == I2C_MODE_CALLBACK) {
        // Check if a transfer is in progress 
        key = Hwi_disable();
        if (object->headPtr) {
            // Transfer in progress 

            //
             //   Update the message pointed by the tailPtr to point to the next
             //   message in the queue
             
            object->tailPtr->nextPtr = transaction;

            // Update the tailPtr to point to the last message 
            object->tailPtr = transaction;

            // I2C is still being used 
            Hwi_restore(key);
            return (true);
        }
        else {
            // Store the headPtr indicating I2C is in use 
            object->headPtr = transaction;
            object->tailPtr = transaction;
        }
        Hwi_restore(key);
    }

    // Acquire the lock for this particular I2C handle 
    Semaphore_pend(Semaphore_handle(&(object->mutex)), BIOS_WAIT_FOREVER);

    //
     //   I2CTiva_primeTransfer is a longer process and
     //   protection is needed from the I2C interrupt
     
    Hwi_disableInterrupt(hwAttrs->intNum);
    I2CTiva_primeTransfer(object, hwAttrs, transaction);
    Hwi_enableInterrupt(hwAttrs->intNum);

    if (object->transferMode == I2C_MODE_BLOCKING) {
        Log_print1(Diags_USER1,
                   "I2C:(%p) Pending on transferComplete semaphore",
                   hwAttrs->baseAddr);
        //
         //   Wait for the transfer to complete here.
         //   It's OK to block from here because the I2C's Hwi will unblock
         //   upon errors
         
        Semaphore_pend(Semaphore_handle(&(object->transferComplete)),
                                        BIOS_WAIT_FOREVER);

        Log_print1(Diags_USER1,
                   "I2C:(%p) Transaction completed",
                   hwAttrs->baseAddr);

        // Hwi handle has posted a 'transferComplete' check for Errors 
        if (object->mode == I2CTiva_IDLE_MODE) {
            Log_print1(Diags_USER1, "I2C:(%p) Transfer OK", hwAttrs->baseAddr);
            ret = true;
        }
    }
    else {
        // Always return true if in Asynchronous mode 
        ret = true;
    }

    // Release the lock for this particular I2C handle 
    Semaphore_post(Semaphore_handle(&(object->mutex)));

    // Return the number of bytes transfered by the I2C 
    return (ret);
}

//==============================================================================
//
//  void I2CTiva_init(I2C_Handle handle)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function sets the control variable of the UART driver
//
//==============================================================================

static void I2CTiva_blockingCallback(I2C_Handle handle,
                                     I2C_Transaction *msg,
                                     bool transferStatus)
{
    I2CTiva_Object  *object = handle->object;

    Log_print1(Diags_USER1,
               "I2C:(%p) posting transferComplete semaphore",
               ((I2CTiva_HWAttrs const  *)(handle->hwAttrs))->baseAddr);

    // Indicate transfer complete 
    Semaphore_post(Semaphore_handle(&(object->transferComplete)));
}
