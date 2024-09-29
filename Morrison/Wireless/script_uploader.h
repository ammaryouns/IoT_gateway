/*******************************************************************************
 * Copyright (C) 2014 Synapse Wireless, Inc.
 * 
 * This software was developed by Synapse Wireless, Inc. for use by Industrial 
 * Scientific Corporation.  It was developed and is being delivered under the 
 * Design Service License provided in the SNAP(TM) Script Uploader Development 
 * Proposal.  Use of this software must be in accordance with the terms and 
 * conditions of the aforementioned license (included below for reference).
 * 
 * --------------------------------------------------------------------------------
 * The Deliverables as defined herein, newly designed and developed by Synapse 
 * pursuant to this agreement (collectively referred to as “Licensed Technology”) 
 * shall be owned by Synapse and licensed to Customer according to the terms 
 * outlined in this proposal and with the following limitations.
 * 
 * No Intellectual Property (IP) Rights To Any Core Synapse Software And Other 
 * Existing Technology Are Granted
 * 
 * The Licensed Technology may be designed to interface or otherwise operate with 
 * products owned or marketed by Synapse (“Synapse Products”), including but not 
 * limited to software and modules for implementing, controlling, and monitoring 
 * wireless networks and for executing and interpreting instructions of the 
 * Software. Synapse Products shall include, but are not limited to, modules 
 * currently or previously marketed by Synapse under the mark RF Engine® and the 
 * Synapse Network Application Protocol software currently or previously marketed 
 * by Synapse under the marks SNAPTM , PortalTM and SNAPTM Connect, 
 * SNAP Lighting.com, SNAP Lighting Cloud Service as well as all improvements, 
 * modifications, and derivatives thereof. Synapse Products shall also include all 
 * software and hardware owned by Synapse as of the date of execution of this 
 * Agreement, as well as all software and hardware conceived of, designed, or 
 * developed by Synapse outside of the obligations of this Agreement. Customer 
 * agrees that Synapse owns all rights, title, and interest in the Synapse Products 
 * and except as otherwise expressly set forth herein, no licenses or intellectual 
 * property rights in the Synapse Products are conveyed to Customer.
 * 
 * Licenses To Core Synapse Software Are Not Waived
 * 
 * To the extent that Customer desires to interface or operate the Licensed 
 * Technology with any Synapse Product, then Customer shall separately purchase or
 * license such Synapse Product.
 * 
 * License That Is Granted Is Limited to Modifications
 * 
 * If Synapse, pursuant to its obligations under this Agreement, creates or 
 * develops the Licensed Technology by modifying original software or hardware that
 * is owned by Synapse prior to such modifications, then Customer shall have a 
 * world-wide, non-exclusive, royalty-free, and perpetual license to use the 
 * modifications to the original software or hardware created or developed pursuant
 * to this Agreement for the sole purpose of designing, developing, manufacturing,
 * and distributing the products and nodes contemplated by this Agreement, but 
 * Synapse shall retain all right, title, and interest in the software or hardware, 
 * as well as all improvements, modifications, and derivatives of the original 
 * software or hardware that are developed or otherwise created by Synapse. 
 * Customer shall not have the right to sub-license the software that is licensed 
 * to Customer pursuant to this Agreement, and Customer shall not make copies of 
 * the licensed software or distribute the licensed software or hardware except to 
 * the extent necessary to design, develop, manufacture, and distribute the 
 * products and nodes contemplated by this Agreement.
 * --------------------------------------------------------------------------------
 ******************************************************************************/

/* (c) Copyright 2014, Synapse Wireless */

/**
 * @file
 * This header file defines the Script Uploader API.  This API allows the
 * client to upload an unpacked spy file to a serially-attached SNAP node.
 *
 * This library is hardware/platform independent.  It relies on the client to
 * provide a set of hardware-specific functions that perform all of the
 * necessary IO and timer related functionality.  These functions are
 * provided by the client using the snap_script_uploader_config API call.
 * The requirements for these functions are specified in the documentation
 * of that API call.
 *
 * Using this API typically a three step process:
 * # Configure the API with the appropriate hardware-specific functions.
 * # Request the CRC of the currently loaded script.
 * # If it needs updating, start the script update process.
 * # Wait for the provided done callback to be triggered.
 */

#ifndef _script_uploader_h_
#define _script_uploader_h_

#include "syn_types.h"

/*******************************************************************************
 * Status
 ******************************************************************************/

/* Script Uploader Status Codes */
typedef enum {
  SCRIPT_UPLOADER_STATUS_OK = 0,
  SCRIPT_UPLOADER_STATUS_NO_RESPONSE_TO_ERASE_COMMAND,
  SCRIPT_UPLOADER_STATUS_NO_RESPONSE_TO_WRITE_DATA_BLOCK,
  SCRIPT_UPLOADER_STATUS_DATA_BLOCK_TOO_LONG,
  SCRIPT_UPLOADER_STATUS_UNREQUESTED_DATA_BLOCK_RECEIVED,
  SCRIPT_UPLOADER_STATUS_NO_DATA_BLOCK_RECEIVED,
  SCRIPT_UPLOADER_STATUS_INVALID_OFFSET_IN_WRITE_ACK,
  SCRIPT_UPLOADER_STATUS_NO_RESPONSE_REBOOT_RPC,
  SCRIPT_UPLOADER_STATUS_NO_RESPONSE_SCRIPT_NAME_RPC,
  SCRIPT_UPLOADER_STATUS_INVALID_SCRIPT_NAME,
  SCRIPT_UPLOADER_STATUS_NO_RESPONSE_SCRIPT_CRC_RPC,
  SCRIPT_UPLOADER_STATUS_INVALID_SCRIPT_CRC,
  SCRIPT_UPLOADER_STATUS_INVALID_STATUS_CODE
} Snap_Script_Uploader_Status;

/*
 * Configuration functions
 */

/**
 * Returns the API version number.
 * The API version number will follow a “<major>.<minor>.<maintenance>” numbering
 * scheme.  An increment of the first number indicates very different (and very
 * likely incompatible) functionality.  An increment of the second number indicates
 * the addition of new features that do not break compatibility.  An increment of
 * the third number indicates bug fixes.
 */
const Snap_U8 * snap_script_uploader_version(void);

/*
 * Status functions
 */

/**
 * Provides the current error status of the API.
 * @return the last uncleared error, or if no errors, returns PS_STATUS_OK.
 */
Snap_Script_Uploader_Status snap_script_uploader_status(void);

/**
 * Clears the error status of the API.
 * Future calls to snap_script_uploader_status will return PS_STATUS_OK until
 * the next error occurs.
 */
void snap_script_uploader_clear_error(void);

/**
 * Sends a request for the CRC of the current script.
 * The CRC request is sent to the attached SNAP node.  When the node responds,
 * the response is passed to the provided callback function.
 * @param script_uploader_script_checksum the function to call with the returned checksum.
 */
void snap_script_uploader_request_script_checksum(void);

/**
 * Checks if the script checksum has been received from the SNAP node.
 * @return 1 if the checksum is available, otherwise 0.
 */
Snap_U8 snap_script_uploader_checksum_available(void);

/**
 * Provides the received script checksum from the SNAP node.  If no checksum has
 * been provided, it returns 0.
 * @return the received script checksum.
 */
Snap_U16 snap_script_uploader_checksum(void);

/*
 * Upload control functions
 */

/**
 * Starts the script upload process.
 * Resets the upload process and calls the appropriate RPC functions over serial
 * to begin the upload process.  Information about the bytecode stream is
 * provided and stored for later use.  The script name and CRC will be checked
 * at the end of the process to confirm that the correct script was uploaded
 * successfully.
 * @param script_name the name of the script being uploaded
 * @param expected_crc the CRC of the script being uploaded
 * @param format the format ID of the incoming bytecode stream
 */
void snap_script_uploader_start_upload(const Snap_U8 * script_name, Snap_U16 expected_crc, Snap_U8 format);

/**
 *  Provides the next block of date from the bytecode stream.
 *  This should be called by a helper function to provide a previously requested
 *  block of data from the bytecode stream.
 *  @param data the block of data.  The data buffer is owned by the caller upon return.
 *  @param length the length of the provided block
 *  @note The length of the provided block may be less than the requested length
 *  if it is the last block of data.
 */
void snap_script_uploader_data_block(const Snap_U8 * data, Snap_U8 length);

/**
 * Notifies the script upload process that there is no more script data to upload.
 * Called by a helper function when the end of the script data is reached.
 */
void snap_script_uploader_end_of_data(void);

/**
 *  Notifies the script upload process that time has elapsed.
 *  Provides an indication of the passage of time to the script upload process.
 *  This allows for timeouts, retries, and the flagging of errors if there is
 *  a communication failure during the upload process.
 *  This may be called from the main loop of the application or from a timer ISR.
 */
void snap_script_uploader_tick_100ms(void);

/**
 * Aborts any currently running script upload process.
 * The error status will not be modified, so the current error state at the time of the
 * call can be checked.  The target SNAP module will be left with no script on it.
 */
void snap_script_uploader_abort(void);

/**
 * Checks if the script upload process is complete without error.
 * @return 1 if the script upload process completed without error, otherwise 0.
 */
Snap_U8 snap_script_uploader_completed(void);

/*
 * Incoming serial handler
 */

/**
 * Processes incoming serial data from the attached SNAP node.
 * Parses and interprets the packet serial RPC calls coming from the SNAP node and
 * triggers responses accordingly.
 * @param data the incoming serial data to process.  The data buffer is owned by the
 * caller upon return.
 * @param len the length of the incoming serial data
 */
void snap_script_uploader_serial_rx(const Snap_U8 * data, Snap_U16 len);

#endif /* _script_uploader_h_ */

