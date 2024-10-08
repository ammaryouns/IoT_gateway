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
 * pursuant to this agreement (collectively referred to as �Licensed Technology�) 
 * shall be owned by Synapse and licensed to Customer according to the terms 
 * outlined in this proposal and with the following limitations.
 * 
 * No Intellectual Property (IP) Rights To Any Core Synapse Software And Other 
 * Existing Technology Are Granted
 * 
 * The Licensed Technology may be designed to interface or otherwise operate with 
 * products owned or marketed by Synapse (�Synapse Products�), including but not 
 * limited to software and modules for implementing, controlling, and monitoring 
 * wireless networks and for executing and interpreting instructions of the 
 * Software. Synapse Products shall include, but are not limited to, modules 
 * currently or previously marketed by Synapse under the mark RF Engine� and the 
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

/* (c) Copyright 2014, Synapse Wireless Inc */

#ifndef _syn_types_h_
#define _syn_types_h_

/*
 * Data type definitions for SNAP
 * If stdint is not available on the target platform,
 * build with CUSTOM_PLATFORM_TYPES defined and provide
 * a custom_syn_types.h with the appropriate definitions.
 */

#ifndef CUSTOM_PLATFORM_TYPES

#include <stdint.h>

typedef uint8_t Snap_U8;
typedef int8_t Snap_S8;
typedef uint16_t Snap_U16;
typedef int16_t Snap_S16;
typedef uint32_t Snap_U32;
typedef int32_t Snap_S32;

#else

#include "custom_syn_types.h"

#endif

#endif /* _syn_types_h_ */
