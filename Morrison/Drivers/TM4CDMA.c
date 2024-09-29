//==============================================================================
// Start of file
//==============================================================================
//
//  TM4CDMA.c
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
//  Source:        TM4CDMA.c
//
//  Project:       Morrison
//
//  Author:        Muhammad Saleem
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
//! This is DMA Device Driver module. 
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//
// Revision: 1.0 2016/21/11 Muhammad Saleem
//           Initial Version
//
//==============================================================================

//==============================================================================
//  INCLUDES
//==============================================================================
//
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include "driverlib/PWM.h"
#include "driverlib/rom_map.h"

//
//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================

// Buffer definitions
#define UART_TXBUF_SIZE 16
#define UART_RXBUF_SIZE 16
static uint8_t g_pui8TxBuf[UART_TXBUF_SIZE];
static uint8_t g_pui8RxPing[UART_RXBUF_SIZE];
static uint8_t g_pui8RxPong[UART_RXBUF_SIZE];
// Error counter
static uint32_t g_ui32DMAErrCount = 0;
// Transfer counters
static uint32_t g_ui32RxPingCount = 0;
static uint32_t g_ui32RxPongCount = 0;

// uDMA control table aligned to 1024-byte boundary
#pragma DATA_ALIGN(ucControlTable, 1024)
uint8_t ucControlTable[1024];

// Library error routine
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


//==============================================================================
// LOCAL FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
// LOCAL FUNCTIONS DEFINITIONS
//==============================================================================

void PWMInit1(void);

//==============================================================================
// GLOBAL  FUNCTIONS PROTOTYPES
//==============================================================================

//==============================================================================
//
//  void uDMAErrorHandler(void)
//
//  Author:     Muhammad Saleem
//  Date:       2016/21/11
//
//! uDMA error handler
//
//============================================================================== 

void uDMAErrorHandler(void)
{
   uint32_t ui32Status;

   ui32Status = ROM_uDMAErrorStatusGet();

   if(ui32Status)
   {
     ROM_uDMAErrorStatusClear();
     g_ui32DMAErrCount++;
   }
}

//==============================================================================
//
//  void DMA1IntHandler(void)
//
//  Author:     Muhammad Saleem
//  Date:       2016/21/11
//
//! DMA1 interrupt handler. Called on completion of uDMA transfer
//
//============================================================================== 

void DMA1IntHandler(void)
{
   uint32_t ui32Status;
   uint32_t ui32Mode;

   GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
    // ui32Status = ROM_UARTIntStatus(UART1_BASE, 1);

   GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_3);
   ui32Mode = ROM_uDMAChannelModeGet(UDMA_CH14_GPIOE | UDMA_PRI_SELECT);

   if(ui32Mode == UDMA_MODE_STOP)
   {
     g_ui32RxPingCount++;
     ROM_uDMAChannelTransferSet(UDMA_CH14_GPIOE | UDMA_PRI_SELECT,
     UDMA_MODE_PINGPONG,
     g_pui8TxBuf+2,
     g_pui8RxPing, sizeof(g_pui8RxPing));
    }

   ui32Mode = ROM_uDMAChannelModeGet(UDMA_CH14_GPIOE | UDMA_ALT_SELECT);

    if(ui32Mode == UDMA_MODE_STOP)
    {
       g_ui32RxPongCount++;

      // ROM_uDMAChannelTransferSet(UDMA_CH14_GPIOE | UDMA_ALT_SELECT,UDMA_MODE_PINGPONG,g_pui8TxBuf), g_pui8RxPong, sizeof(g_pui8RxPong) );
    }

    if(!ROM_uDMAChannelIsEnabled(UDMA_CH14_GPIOE))
    {

       ROM_uDMAChannelTransferSet(UDMA_CH14_GPIOE | UDMA_PRI_SELECT,
       UDMA_MODE_PINGPONG,
       g_pui8TxBuf,
       g_pui8RxPing, sizeof(g_pui8RxPing));
       ROM_uDMAChannelEnable(UDMA_CH14_GPIOE);
    }
}
//==============================================================================
//
//  void DMA1IntHandler(void)
//
//  Author:     Muhammad Saleem
//  Date:       2016/21/11
//
//! // Initialize UART uDMA transfer
//
//============================================================================== 

void InitDMATransfer(void)
{
   uint32_t ui32Idx;

   // Initialize transmit buffer with some data
   for(ui32Idx = 0; ui32Idx < UART_TXBUF_SIZE; ui32Idx++)
   {
      g_pui8TxBuf[ui32Idx] = ui32Idx;
   }

   // Enable UART1 and make sure it can run while the CPU sleeps
   ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
   ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOE);

   // Configure and enable the UART with DMA


   ROM_GPIOPinTypeGPIOInput(GPIO_PORTE_BASE,GPIO_PIN_3);
   ROM_GPIOIntTypeSet(GPIO_PORTE_BASE,GPIO_PIN_3, GPIO_RISING_EDGE);
   ROM_GPIOPadConfigSet(GPIO_PORTE_BASE,GPIO_PIN_3,GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD_WPD);
   GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_3);
   ROM_GPIODMATriggerEnable(GPIO_PORTE_BASE, GPIO_PIN_3);
   GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_3);
   //ROM_IntEnable(INT_GPIOE); // SALEEM
   IntMasterEnable();


   // Receive channel setup for ping and pong
   ROM_uDMAChannelAttributeDisable(UDMA_CH14_GPIOE,
   UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
   UDMA_ATTR_HIGH_PRIORITY |
   UDMA_ATTR_REQMASK);

   ROM_uDMAChannelControlSet(UDMA_CH14_GPIOE | UDMA_PRI_SELECT,
   UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_8 |
   UDMA_ARB_8);

   ROM_uDMAChannelControlSet(UDMA_CH14_GPIOE | UDMA_ALT_SELECT,
   UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_8 |
   UDMA_ARB_8);

   ROM_uDMAChannelTransferSet(UDMA_CH14_GPIOE | UDMA_PRI_SELECT,
   UDMA_MODE_PINGPONG,
   g_pui8TxBuf+2,
   g_pui8RxPing, sizeof(g_pui8RxPing));

   ROM_uDMAChannelTransferSet(UDMA_CH14_GPIOE | UDMA_ALT_SELECT,
   UDMA_MODE_PINGPONG,
   (g_pui8TxBuf+2),
   g_pui8RxPong, sizeof(g_pui8RxPong));

   // Transmit channel setup for a basic transfer

   // Enable both channels
   ROM_uDMAChannelEnable(UDMA_CH14_GPIOE);
   // ROM_uDMAChannelRequest(UDMA_CH14_GPIOE);
}
//==============================================================================
//
//  void DMA1IntHandler(void)
//
//  Author:     Muhammad Saleem
//  Date:       2016/21/11
//
//! Main code
//
//============================================================================== 

void initDMAmain(void)
{
   volatile uint32_t ui32Loop;

   ROM_FPULazyStackingEnable();

   ROM_SysCtlClockSet(SYSCTL_SYSDIV_64 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
   SYSCTL_XTAL_16MHZ);

   ROM_SysCtlPeripheralClockGating(true);

   // GPIO setup for LEDs
   ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
   ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_1);

   // Enable uDMA
   ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
   ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UDMA);
   //ROM_IntEnable(INT_UDMAERR); SALEEM
   ROM_uDMAEnable();
   ROM_uDMAControlBaseSet(ucControlTable);

   // Initialize the uDMA/UART transfer
   InitDMATransfer();

   PWMInit1();
// Blink the LED while the transfers occur
 while(1)
 {
   GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
   SysCtlDelay(SysCtlClockGet() / 20 / 3);
   GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
   SysCtlDelay(SysCtlClockGet() / 20 / 3);
   GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);

  }
}