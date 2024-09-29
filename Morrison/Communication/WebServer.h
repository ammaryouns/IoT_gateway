//==============================================================================
// Start of file
//==============================================================================
//
//  WebServer.h
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
//  Source:        WebServer.h
//
//  Project:       Morrison
//
//  Author:        Fehan Arif
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
//! This is Webserver module. It contains global functions which updates 
//! webserver html files in the firmware. 
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//
// Revision: 1.0 2016/21/11 Fehan Arif
//           Initial Version
//
//==============================================================================

#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__
//==============================================================================
//  INCLUDES
//==============================================================================
//
#include <stdint.h>
#include <stdbool.h>
//
//==============================================================================
// CONSTANTS, DEFINES AND MACROS
//==============================================================================

//==============================================================================
// GLOBAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
// GLOBAL  FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
//
//  void WebServerUpdateFile (void)
//
//  Author:     Fehan Arif Malik
//  Date:       2016/21/11
//
//! This functions updates the html file in the firmware 
//
//==============================================================================

void WebServerUpdateFile (void);

//==============================================================================
//
//  void WebServerRemoveFile(void)
//
//  Author:     Fehan Arif Malik
//  Date:       2016/21/11
//
//! This functions removes the html file from the firmware 
//
//==============================================================================

void WebServerRemoveFile(void);

//==============================================================================
//  End Of File
//==============================================================================
#endif // __WEBSERVER_H__