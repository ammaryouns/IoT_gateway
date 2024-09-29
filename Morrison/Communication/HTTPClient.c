//==============================================================================
// Start of file
//==============================================================================
//
//  HTTPClient.c
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
//  Date:          2016/12/09
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This is HTTP Client module.  
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//
// Revision: 1.0 2016/12/09 Fehan Arif
//           Initial Version
//
//==============================================================================

//==============================================================================
//  INCLUDES
//==============================================================================
//
#include <string.h>
#include <time.h>

/* XDCtools Header files */
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <ti/sysbios/hal/Seconds.h>
#include <ti/drivers/GPIO.h>
#include <ti/net/http/httpcli.h>
#include <ti/net/sntp/sntp.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>

/* Example/Board Header file */
#include "Board.h"

#include <sys/socket.h>

#define HOSTNAME         "www.example.com:443"
#define REQUEST_URI      "/"
#define USER_AGENT       "HTTPCli (ARM; TI-RTOS)"
#define NTP_HOSTNAME     "pool.ntp.org"
#define NTP_PORT         "123"
#define NTP_SERVERS      3
#define NTP_SERVERS_SIZE (NTP_SERVERS * sizeof(struct sockaddr_in))
#define HTTPTASKSTACKSIZE 32768

extern Event_Struct evtStruct;
extern Event_Handle evtHandle;
/*
* USER STEP: Copy the lines in the root CA certificate between
*            -----BEGIN CERTIFICATE-----
*            ...
*            -----END CERTIFICATE-----
*/
uint8_t ca[] ="MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\
d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\
ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\
MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\
LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\
RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\
+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\
PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\
xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\
Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\
hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\
EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\
MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\
FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\
nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\
eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\
hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\
Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\
vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\
+OkuE6N36B9K";

uint32_t calen = sizeof(ca);
unsigned char ntpServers[NTP_SERVERS_SIZE];
static Semaphore_Handle semHandle = NULL;

/*
*  ======== printError ========
*/
void printError(char *errString, int code)
{
    System_printf("Error! code = %d, desc = %s\n", code, errString);
//    BIOS_exit(code);
}

/*
*  ======== timeUpdateHook ========
*  Called after NTP time sync
*/
void timeUpdateHook(void *p)
{
    Semaphore_post(semHandle);
}

/*
*  ======== startNTP ========
*/
void startNTP(void)
{
    int ret;
    int currPos;
    time_t ts;
    struct sockaddr_in ntpAddr;
    struct addrinfo hints;
    struct addrinfo *addrs;
    struct addrinfo *currAddr;
    Semaphore_Params semParams;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    
    ret = getaddrinfo(NTP_HOSTNAME, NTP_PORT, NULL, &addrs);
    if (ret != 0) {
        printError("startNTP: NTP host cannot be resolved!", ret);
    }
    
    currPos = 0;
    
    for (currAddr = addrs; currAddr != NULL; currAddr = currAddr->ai_next) {
        if (currPos < NTP_SERVERS_SIZE) {
            ntpAddr = *(struct sockaddr_in *)(currAddr->ai_addr);
            memcpy(ntpServers + currPos, &ntpAddr, sizeof(struct sockaddr_in));
            currPos += sizeof(struct sockaddr_in);
        }
        else {
            break;
        }
    }
    
    freeaddrinfo(addrs);
    
    ret = SNTP_start(Seconds_get, Seconds_set, timeUpdateHook,
                     (struct sockaddr *)&ntpServers, NTP_SERVERS, 0);
    if (ret == 0) {
        printError("startNTP: SNTP cannot be started!", -1);
    }
    
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    semHandle = Semaphore_create(0, &semParams, NULL);
    if (semHandle == NULL) {
        printError("startNTP: Cannot create semaphore!", -1);
    }
    
    SNTP_forceTimeSync();
    Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
    
    ts = time(NULL);
    System_printf("Current time: %s\n", ctime(&ts));
}

/*
*  ======== httpsTask ========
*  Makes an HTTP GET request
*/
Void httpsTask(UArg arg0, UArg arg1)
{
    bool moreFlag = false;
    char data[64];
    int ret;
    int len;
    struct sockaddr_in addr;
    
    startNTP();
    while (1)
    {
        Event_pend(evtHandle, Event_Id_00, Event_Id_NONE,BIOS_WAIT_FOREVER);
        
        TLS_Params tlsParams;
        TLS_Handle tls;
        
        HTTPCli_Params params;
        HTTPCli_Struct cli;
        HTTPCli_Field fields[3] = {
            { HTTPStd_FIELD_NAME_HOST, HOSTNAME },
            { HTTPStd_FIELD_NAME_USER_AGENT, USER_AGENT },
            { NULL, NULL }
        };
        
        
        
        System_printf("Sending a HTTPS GET request to '%s'\n", HOSTNAME);
        System_flush();
        
        TLS_Params_init(&tlsParams);
        tlsParams.ca = ca;
        tlsParams.calen = calen;
        
        tls = TLS_create(TLS_METHOD_CLIENT_TLSV1_2, &tlsParams, NULL);
        if (!tls) {
            printError("httpsTask: TLS create failed", -1);
        }
        
        HTTPCli_construct(&cli);
        
        HTTPCli_setRequestFields(&cli, fields);
        
        ret = HTTPCli_initSockAddr((struct sockaddr *)&addr, HOSTNAME, 0);
        if (ret < 0) {
            printError("httpsTask: address resolution failed", ret);
        }
        
        HTTPCli_Params_init(&params);
        params.tls = tls;
        
        ret = HTTPCli_connect(&cli, (struct sockaddr *)&addr, 0, &params);
        if (ret < 0) {
            printError("httpsTask: connect failed", ret);
        }
        
        ret = HTTPCli_sendRequest(&cli, HTTPStd_GET, REQUEST_URI, false);
        if (ret < 0) {
            printError("httpsTask: send failed", ret);
        }
        
        ret = HTTPCli_getResponseStatus(&cli);
        if (ret != HTTPStd_OK) {
            printError("httpsTask: cannot get status", ret);
        }
        
        System_printf("HTTP Response Status Code: %d\n", ret);
        
        ret = HTTPCli_getResponseField(&cli, data, sizeof(data), &moreFlag);
        if (ret != HTTPCli_FIELD_ID_END) {
            printError("httpsTask: response field processing failed", ret);
        }
        
        len = 0;
        do {
            ret = HTTPCli_readResponseBody(&cli, data, sizeof(data), &moreFlag);
            if (ret < 0) {
                printError("httpsTask: response body processing failed", ret);
            }
            len += ret;
        } while (moreFlag);

        System_printf("Recieved %d bytes of payload\n", len);
        System_flush();
        HTTPCli_disconnect(&cli);
        HTTPCli_destruct(&cli);
        
        TLS_delete(&tls);
    }
    
}

/*
*  ======== netIPAddrHook ========
*  This function is called when IP Addr is added/deleted
*/
void netIPAddrHook(unsigned int IPAddr, unsigned int IfIdx, unsigned int fAdd)
{
    static Task_Handle taskHandle;
    Task_Params taskParams;
    Error_Block eb;
    
    /* Create a HTTP task when the IP address is added */
    if (fAdd && !taskHandle) {
        Error_init(&eb);
        
        Task_Params_init(&taskParams);
        taskParams.stackSize = HTTPTASKSTACKSIZE;
        taskParams.priority = 6;
        taskHandle = Task_create((Task_FuncPtr)httpsTask, &taskParams, &eb);
        if (taskHandle == NULL) {
            printError("netIPAddrHook: Failed to create HTTP Task\n", -1);
        }
    }
}