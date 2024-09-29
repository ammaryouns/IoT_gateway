//==============================================================================
// Start of file
//==============================================================================
//
//  TM4CADC.c
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
//  Source:        TM4CADC.c
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
//! This is ADC Device Driver module. It contains global function that are used
//! for analogue to digital conversion.
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
//
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "TM4CADC.h"
//
//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================

//==============================================================================
// LOCAL function PROTOTYPES
//==============================================================================

//==============================================================================
// LOCAL function DEFINITIONS
//==============================================================================

//==============================================================================
// GLOBAL  function IMPLEMENTATIONS
//==============================================================================

//==============================================================================
//
//  ADCInit(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function configures the ADC0 module
//
//==============================================================================

void ADCInit(void)
{  
  // Enable GPIO PORT_E for AIN0(PE3) and AIN1(PE2)
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);  
  // Wait for the module to get ready
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));  
  // Configure pins for ADC  
  GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_3);  
  // Enable ADC peripheral
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);  
  // Wait for the module to get ready
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));  
  // Configure ADC0's Sample Sequencer 1 for software trigger based conversion
  // at highest priority
  ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);  
  // Configure step #1 and #2 for sampling data from AIN0 and AIN1 while 
  // step AIN1 shall be the last, software trigger controlled, step in the sequence
  ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_CH0);  
  ADCSequenceStepConfigure(ADC0_BASE, 2, 1, ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);                
}

//==============================================================================
//
//  ADCReadChannel(
//                 uint32_t *adcReadBuffer
//                  )
//
//  Author:     Muhammad Shuaib
//  Date:       2016/21/11
//
//! This function uses the ADC0 module, reads AIN0 and AIN1 and saves the read
//! values of two channels in adcReadBuffer
//
//==============================================================================

void ADCReadChannel(
                    uint32_t *adcReadBuffer // Pointer to read buffer
                      )
{
  
  // Enable the ADC0 sample sequence 2 before using it for the purpose of conversion
  ADCSequenceEnable(ADC0_BASE, 2);  
  // Clear any pending interrupts(Raw or masked) for sequence 2
  ADCIntClear(ADC0_BASE, 2);  
  // Trigger the ADC conversion.
  ADCProcessorTrigger(ADC0_BASE, 2);  
  // Wait for conversion to be completed.
  while(!ADCIntStatus(ADC0_BASE, 2, false))
  {
  }  
  // Clear the ADC interrupt flag.
  ADCIntClear(ADC0_BASE, 2);  
  // Read ADC Value.
  ADCSequenceDataGet(ADC0_BASE, 2, adcReadBuffer);  
  // Diable the sequence after reading the data
  ADCSequenceDisable(ADC0_BASE, 2);
}

//==============================================================================
//  End Of File
//==============================================================================