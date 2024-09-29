//==============================================================================
// Start of file
//==============================================================================
//
//  WebServer.c
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
//  Source:        WebServer.c
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

//==============================================================================
//  INCLUDES
//==============================================================================
//
#include <stdint.h>
#include "WebServer.h"
#include "Board.h"
#include <ti/ndk/inc/netmain.h>
#include "index.h"
//
//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================


//==============================================================================
// LOCAL FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
// LOCAL FUNCTIONS DEFINITIONS
//==============================================================================

//==============================================================================
// GLOBAL  FUNCTIONS IMPLEMENTATIONS
//==============================================================================

//==============================================================================
// GLOBAL  FUNCTIONS IMPLEMENTATIONS
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

void WebServerUpdateFile (void)
{
    //Note: both INDEX_SIZE and INDEX are defined in index.h
    efs_createfile("index.html", INDEX_SIZE, (UINT8 *)INDEX);
}

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

void WebServerRemoveFile(void)
{
    efs_destroyfile("index.html");
}
