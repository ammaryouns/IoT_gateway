//==============================================================================
// Start of file
//==============================================================================
//
//  Buttton.c
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
//  Source:        Button.c
//
//  Project:       Morrison
//
//  Author:        Muhammad Shuaib
//
//  Date:          2016/05/12
//
//  Revision:      1.0
//
//==============================================================================
//  FILE DESCRIPTION
//==============================================================================
//
//! \file
//! This is button module.
//
//==============================================================================
//  REVISION HISTORY
//==============================================================================
//
// Revision: 1.0 2016/05/12 Muhammad Shuaib
//           Initial Version
//
//==============================================================================

//==============================================================================
//  INCLUDES
//==============================================================================
//
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "GPIO.h"
#include "Board.h"
#include "EK_TM4C1294XL.h"
#include "Button.h"
#include "Main.h"
#include "TM4CTIMER.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
#include <ti/sysbios/hal/Timer.h>
//
//==============================================================================
// CONSTANTAS, DEFINES AND MACROS
//==============================================================================

#define POWER_BUTTON_TIME               20u                             //!< Button debounce time in msec
#define POWER_BUTTON_COUNT              ((1*1000)/POWER_BUTTON_TIME)    //!< Power button count for 1000 msec
#define POWER_BUTTON_250MS_COUNTS       (1u*(POWER_BUTTON_COUNT/4)+1u)  //!< Power button count for 250 msec
#define BUTTON_COUNT_2S                 ((POWER_BUTTON_COUNT*2u)+2u)    //!< Button press count for 2000 msec

//!< Button structure declaration
typedef struct
{
    unsigned int buffer[BUTTON_BUFFER_SIZE];        //!< Button buffer 
    unsigned int bufferInIndex;                     //!< Index into button buffer where next scan code will be inserted
    unsigned int bufferOutIndex;                    //!< Index into button buffer where next scan code will be removed 
    unsigned int number;                            //!< Number of buttons read from the button buffer
    unsigned int time;                              //!< Current button pressed time, depends on the scan rate
    unsigned int pressed;                           //!< Button pressed status, 0 means no button pressed
    unsigned int repeatStartDelay;                  //!< Time of scan before auto repeat is started      
    unsigned int repeatDelay;                       //!< Time of scan before auto repeat executes again  
    unsigned int scanState;                         //!< Current state of button scanning function                     
} BUTTON_STRUCT;

#define BUTTON_PRESSED          0u                  //!< Define for button pressed state
#define BUTTON_NOT_PRESSED      1u                  //!< Define for button not pressed state

//==============================================================================
// LOCAL DATA DECLARATIONS
//==============================================================================

//==============================================================================
// LOCAL DATA DEFINITIONS
//==============================================================================
//
static BUTTON_STRUCT buttonStructure;                    //!< Structure contains the buttons related parameters
static unsigned char powerButtonPressedCount    = 0u;    //!< This variable is used as Power button press count
static unsigned char powerButtonDebounceCount   = 0u;    //!< This variable is used as Power button debounce count
static unsigned char resetButtonPressedCount    = 0u;    //!< This variable is used as Reset button press count
static unsigned char resetButtonDebounceCount   = 0u;    //!< This variable is used as Reset button debounce count
static bool flagButtonInPeeking                 = false; //!< This flag is used to turn on Battery LED upon Power button press
static unsigned char normalModeButtonPressState = 0;     //!< This variable is used in detection of button long pres in normal mode
static bool isAnyButtonPressed                  = false; //!< This variable is used to save the status of if any button is pressed
static unsigned int oldCode                     = 0;
static unsigned int latestButtonCode            = 0;
static bool canStartInitTask = false;
//
//==============================================================================
// LOCAL FUNCTION PROTOTYPES
//==============================================================================
//------------------------------------------------------------------------------
//   PushButton(unsigned int code)
//
//   Author:   Muhammad Shuaib
// 
//   Date:     2016/06/12
//
//!  This function pushes pressed button code onto button buffer
//
//------------------------------------------------------------------------------

static void PushButton(unsigned int code);

//==============================================================================
// LOCAL FUNCTION DEFINITIONS
//==============================================================================

//==============================================================================
// GLOBAL  FUNCTION IMPLEMENTATIONS
//==============================================================================

//==============================================================================
//
//  ButtonInit(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/05/12
//
//! This function configures the GPIO interrupts for Power and Reset buttons
//
//==============================================================================

void ButtonInit(void)
{  
    // Initialize button structure
    buttonStructure.bufferInIndex = (unsigned int)0;
    buttonStructure.bufferOutIndex = (unsigned int)0;
    buttonStructure.number = (unsigned int)0;
    buttonStructure.time = (unsigned int)0;
    buttonStructure.pressed = (unsigned int)0;
    // Install Power button callback
    GPIO_setCallback(Board_BUTTON0, ButtonIRQHandler);
    // Enable Power button interrupt
    GPIO_enableInt(Board_BUTTON0);
    // Install Reset button callback  
    GPIO_setCallback(Board_BUTTON1, ButtonIRQHandler);  
    // Enable Reset button interrupt
    GPIO_enableInt(Board_BUTTON1);  
}

//==============================================================================
//
//  ButtonIRQHandler(unsigned int index)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/05/12
//
//! This function handles Power and Reset button interrupts
//
//==============================================================================

void ButtonIRQHandler(unsigned int index)
{ 
    // Variable to store the save the status of device peeking state
    bool isDeviceInPeeking = false;
    // Variable to save the status of Power button pressed state
    unsigned int isPowerButtonPressed = 0;
    // Variable to save the status of Reset button pressed state
    unsigned int isResetButtonPressed = 0;
    
    
    // If interrupt was caused by Power button press
    if(index == 0)
    {
        // Check if device is in peeking state and Power button is pressed
        isDeviceInPeeking = GetIsDeviceInPeeking();    
        // If device is in peeking state and Power button is pressed
        if(isDeviceInPeeking == true)
        {      
            // Reset Power button pressed count
            powerButtonPressedCount = 0;
            // Reset Power button debounce count
            powerButtonDebounceCount = 0;
            // Read the Power button status to see if buttons is still pressed
            isPowerButtonPressed = GPIO_read(Board_BUTTON0);
            // If Power button is in pressed state
            if(isPowerButtonPressed == BUTTON_PRESSED)
            {
                // Configure timer for 20ms intrrupt                
                Timer_start(buttonTimerHandle);
                
            }
            else
            {
                // Reset button pressed in peeking flag
                flagButtonInPeeking = false;
            }
        }
        // If device in normal state and Power button is pressed
        else if(isDeviceInPeeking == false)
        {
            // Initialize button time
            buttonStructure.time = 0u ;
            // Configure button timer for 20ms interrupt                        
            Timer_start(buttonTimerHandle);          
        }    
    }
    // If interrupt was caused by Reset button press
    else if(index == 1)
    {
        // Check if device is in peeking state and Reset button is pressed
        isDeviceInPeeking = GetIsDeviceInPeeking();    
        // If device is in peeking state and Power button is pressed
        if(isDeviceInPeeking == true)
        {      
            // !Todo Decide what to do when reset button is pressed in peeking state
        }
        // If device in normal state and Power button is pressed
        else if(isDeviceInPeeking == false)
        {
            // Initialize button time
            buttonStructure.time = 0u ;
            // Configure button timer for 20ms interrupt                        
            Timer_start(buttonTimerHandle);            
        }
    }  
}

//------------------------------------------------------------------------------
//   ButtonProcessTimerInterrupt(void)
//
//   Author:   Muhammad Shuaib
//   Date:     2016/05/12
//
//!  This function handles the timer interrupt activated due to button press
//
//------------------------------------------------------------------------------

void ButtonProcessTimerInterrupt(void)
{   
    // Variable to save the status of if instrument is in peeking state
    bool isDeviceAtPeeking = false;
    
    // Check if instrument is in peeking state
    isDeviceAtPeeking = GetIsDeviceInPeeking();
    // If instrument is in normal mode
    if ( isDeviceAtPeeking == false)
    {
        // Process button press in normal mode
        DetectButtonNormalMode(); 
    }    
    // If device is in peeking mode
    else
    {
        // Process button press in peeking mode to see if device should be powered on
        DetectButtonPeekingMode();     
    }
}

//==============================================================================
//
//  DetectButtonPeekingMode(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/05/12
//
//! This function detecs pressed button in peeking mode
//
//==============================================================================

static void DetectButtonPeekingMode(void)
{
    // Variable to save the status of Power button pressed state
    unsigned int isPowerButtonPressed = 0;
    // Variable to save the status of Reset button pressed state
    unsigned int isResetButtonPressed = 0;
    // Variable to save Power button pressed count and debouce count sum
    unsigned int powerButtonCountSum = 0;
    // Variable to save Reset button pressed count and debouce count sum
    unsigned int resetButtonCountSum = 0;
    
    // Read the Power button status to see if it is pressed
    isPowerButtonPressed = GPIO_read(Board_BUTTON0);
    // Read the Power button status to see if it is pressed
    isResetButtonPressed = GPIO_read(Board_BUTTON1);
    
    // If Power button is in pressed state
    if(isPowerButtonPressed == BUTTON_PRESSED)
    {
        // Increment Power button pressed count
        powerButtonPressedCount++;
    }
    // Otherwise
    else
    {
        // Increment Power button debounce count
        powerButtonDebounceCount++;
    }
    // If Reset button is in pressed state
    if(isResetButtonPressed == BUTTON_PRESSED)
    {
        // Increment Reset button pressed count
        resetButtonPressedCount++;
    }
    // Otherwise
    else
    {
        // Increment Reset button debounce count
        resetButtonDebounceCount++;
    }
    // Calculate the Power button pressed count and debounce count sum
    powerButtonCountSum = powerButtonPressedCount + powerButtonDebounceCount;
    // Calculate the Reset button pressed count and debounce count sum
    resetButtonCountSum = resetButtonPressedCount + resetButtonDebounceCount;
    // Check if Power button has been pressed for 250ms
    if(powerButtonCountSum >= POWER_BUTTON_250MS_COUNTS)
    {
        // Check if Power button pressed count is greater than or equal to debounce count
        if(powerButtonPressedCount > powerButtonDebounceCount)
        {
            // Set the flag to enable the Battery LED and start power on
            flagButtonInPeeking = true;            
        }    
    }
    else
    {
        // Clear the flag to enable the Battery LED and start power on
        flagButtonInPeeking = false;
    }
    
    // Check if only Power button is pressed for more than 2 seconds
    if( ( powerButtonCountSum >= BUTTON_COUNT_2S ) && ( resetButtonCountSum >= BUTTON_COUNT_2S ))
    {
        // If mode button press count is greater than or equal to debounced count then 
        // consider mode button is pressed for three seconds
        if( ( powerButtonPressedCount >= powerButtonDebounceCount ) && \
           ( powerButtonPressedCount >= BUTTON_COUNT_2S ) && \
            (isResetButtonPressed == BUTTON_NOT_PRESSED))
        {
            // Set peeking mode flag to false
            SetIsDeviceInPeeking(false);
            // Reset peeking mode button flag
            flagButtonInPeeking = false;
            // Reset Power button press count
            powerButtonPressedCount = 0u;
            // Reset Power button debounced count
            powerButtonDebounceCount = 0u;
            // Reset Reset button press count
            resetButtonPressedCount = 0u;
            // Reset Reset button debounced count
            resetButtonDebounceCount = 0u;            
            // Post button semaphore to start initialization task
            Semaphore_post(buttonSemaphoreHandle);
        }
        else if(( powerButtonPressedCount >= powerButtonDebounceCount )&& \
                 (isResetButtonPressed == BUTTON_PRESSED))
        {
            // Configure timer for 20ms
            Timer_start(buttonTimerHandle);
        }
        else
        {
            // Reset peeking mode button flag
            flagButtonInPeeking = false;
            // Reset Power button press count
            powerButtonPressedCount = 0u;
            // Reset Power button debounced count
            powerButtonDebounceCount = 0u;
            // Reset Reset button press count
            resetButtonPressedCount = 0u;
            // Reset Reset button debounced count
            resetButtonDebounceCount = 0u;
        }
    }
    else
    {
        // Configure timer for 20ms        
        Timer_start(buttonTimerHandle);
    }
}

//==============================================================================
//
//  DetectButtonNormalMode(void)
//
//  Author:     Muhammad Shuaib
//  Date:       2016/05/12
//
//! This function detecs pressed button in normal mode
//
//==============================================================================

static void DetectButtonNormalMode(void)
{
    // Variable to store the status of long button press
    static bool isLongPressDetected = false;
    // Variable to save the button value to be pushed
    unsigned int pushButtonValue = 0u;
    
    // Perform short button press detection
    if(normalModeButtonPressState == 0u)
    {
        // Check if any button is in pressed state
        ButtonPressed(&isAnyButtonPressed);
        // If any button is in pressed state
        if(isAnyButtonPressed == true)
        {
            // Reset the  button long press detection flag
            isLongPressDetected = false;
            // Change the short button press detection state to long button press detection state
            normalModeButtonPressState = 1u;
            // Determine the button scan code
            ButtonDecode(&oldCode );
            // Configure button timer for 20ms interrupt                   
            Timer_start(buttonTimerHandle);
        }
        else
        {            
            // Reset the long button press detection state
            normalModeButtonPressState = 0;
        }
    }
    // Perform long button press detection
    else if(normalModeButtonPressState > 0u)
    {
        // Increment long button press detection state
        normalModeButtonPressState++;
        // Check if any button is in pressed state
        ButtonPressed(&isAnyButtonPressed);
        // If any button is in pressed state
        if(isAnyButtonPressed == true)
        {
            // Read the pressed button code
            ButtonDecode(&oldCode);
            // Update time for the number of seconds long button was pressed
            buttonStructure.time += BUTTON_TASK_DELAY ;
            // Configure button timer for 20ms interrupt                   
            Timer_start(buttonTimerHandle);
            // Check if long button press detection condition is fulfilled
            if( ((normalModeButtonPressState == LONG_DETECTION_LIMIT + 1u) && (isLongPressDetected == true)) || \
                ((normalModeButtonPressState == LONG_DETECTION_LIMIT_FIRST) && (isLongPressDetected == false)) )
            {
                // Push button long press code onto button buffer
                pushButtonValue|= oldCode << 2u;
                // Push the button code
                PushButton(pushButtonValue);
                // Update button long press detetion state variable
                normalModeButtonPressState = 1;
                // Set button long press detection flag
                isLongPressDetected = true;
            }
        }
        else
        {
            // If long press was not detected
            if( (isLongPressDetected == false)) 
            {
                // Push detected button code
                PushButton(oldCode);
            }
            // Reset button long press detection state
            normalModeButtonPressState = 0u;            
        }
    }
    else
    {
    }
}

//------------------------------------------------------------------------------
//   ButtonDecode(unsigned int *status)
//
//   Author:   Muhammad Shuaib
//
//   Date:    2016/06/12
//
//!  This function returns the status of pressed buttons by reading respective
//!  GPIO interrupts
//
//------------------------------------------------------------------------------

void ButtonPressed(
                   bool *isButtonPressed // Pressed button status
                       )
{
    // Variable to save status of if any button is in pressed state
    unsigned int buttonPressedCode = 0;
    
    // Get the pressed buttons code
    ButtonDecode(&buttonPressedCode);
    // If any button is pressed
    if(buttonPressedCode != 0)
    {
        // Set button pressed status to true
        *isButtonPressed = true;
    }
    // Otherwise
    else
    {
        // Set button presses status to false
        *isButtonPressed = false;
    }
}
//------------------------------------------------------------------------------
//   ButtonDecode(unsigned int *code)
//
//   Author:   Muhammad Shuaib
//
//   Date:    2016/06/12
//
//!  This function returns the code of pressed buttons by reading respective
//!  GPIO interrupts
//
//------------------------------------------------------------------------------

void ButtonDecode(
                  unsigned int *code // Pressed buttons code
                      )
{
    // Power button pressed status
    unsigned int isPowerButtonPressed = 0;
    // Reset button pressed status
    unsigned int isResetButtonPressed = 0;
    // Button code to be returned
    unsigned int buttonCode = 0;
    
    // Read Power button interrupt status
    isPowerButtonPressed = GPIO_read(Board_BUTTON0);
    // Read Reset button interrupt status
    isResetButtonPressed = GPIO_read(Board_BUTTON1);
    // If Power button is in pressed state
    if(isPowerButtonPressed == BUTTON_PRESSED)
    {
        // Save button pressed state
        isPowerButtonPressed = POWER_BUTTON_CODE;
    }
    else
    {
        // Save button pressed state
        isPowerButtonPressed = BUTTON_CODE_NO_BUTTON;   
    }
    // If Reset button is in pressed state
    if(isResetButtonPressed == BUTTON_PRESSED)
    {
        // Save button pressed state
        isResetButtonPressed = RESET_BUTTON_CODE;
    }
    else
    {
        // Save button pressed state
        isResetButtonPressed = BUTTON_CODE_NO_BUTTON;
    }
    // Append Power button and Reset button status into button code variable
    buttonCode = (isPowerButtonPressed | isResetButtonPressed );
    // Return the button code
    *code = buttonCode;
}

//------------------------------------------------------------------------------
//   PushButton(unsigned int code)
//
//   Author:   Muhammad Shuaib
// 
//   Date:     2016/06/12
//
//!  This function pushes pressed button code onto button buffer
//
//------------------------------------------------------------------------------

static void PushButton(
                       unsigned int code   //!< Button code to be pushed
                           )
{
    // Check for buffer overflow
    if ( buttonStructure.number < BUTTON_BUFFER_SIZE )
    {
        // Increment buffer index
        buttonStructure.number++;
        // Push the button code
        buttonStructure.buffer[buttonStructure.bufferInIndex++] = (unsigned int)code;
        // Update latest button value
        latestButtonCode = (unsigned int)code;
        // Adjust index to the next scan code to put in buffer
        buttonStructure.bufferInIndex %= BUTTON_BUFFER_SIZE;            
    }     
}

//------------------------------------------------------------------------------
//   ButtonHit(bool *isButtonHit)
//
//   Author:   Muhammad Shuaib
//
//   Date:     2016/06/12
//
//!  This function checks button buffer to see if pressed button exists
//
//------------------------------------------------------------------------------

void ButtonHit(
               bool *isButtonHit    //!< Contains the status of hit button
                   )
{
    // Get the status of button
    *isButtonHit = (buttonStructure.number > 0) ? true : false;
}

//------------------------------------------------------------------------------
//   ButtonPop(unsigned int *code)
//
//   Author:   Muhammad Shuaib
//
//   Date:     2016/06/12
//
//!  This function extracts latest button code form buffer buffer
//
//------------------------------------------------------------------------------

void ButtonPop(
               unsigned int *code   //!< Contains the button code
                   )
{
    bool isButtonPress = false;    // Contains status of if button is pressed
    
    // If any pressed button exists in button buffer
    if ( buttonStructure.number > 0u )
    {
        // Decrement button buffer index
        buttonStructure.number--;
        // Get scan code from the buffer 
        *code = buttonStructure.buffer[buttonStructure.bufferOutIndex];       
        buttonStructure.bufferOutIndex++;
        // Adjust index into the button buffer 
        buttonStructure.bufferOutIndex %= BUTTON_BUFFER_SIZE;
        ButtonPressed(&isButtonPress );
        // Check if button is not pressed
        if ( ( isButtonPress == false ) && ( ( (*code) << 2u ) != 0u ) )
        {
            // Return the scan code of the button pressed 
            *code = (unsigned int)0;
        }
    }
    else
    {
        // No scan codes in the buffer, return 0
        *code = (unsigned int)0;
    }
}
//------------------------------------------------------------------------------
//   ButtonClearBuffer(void)
//
//   Author:   Muhammad Shuaib
//
//   Date:     2016/06/12
//
//!  This function clears all buttons from button buffer
//
//------------------------------------------------------------------------------

void ButtonClearBuffer(void)
{
    bool isButtonHit = false;           // Contains the status of button hit, true or false
    unsigned int buttonCode = 0u;       // Contains the code return through ButtonPop
    // Check if button is hit or not
    ButtonHit( &isButtonHit );
    // While there are buttons in button buffer
    while(isButtonHit == true)
    {
        // Check if there is button at current index in button buffer
        ButtonHit( &isButtonHit );        
        // Pop the button code
        ButtonPop( &buttonCode );
    }
}


//------------------------------------------------------------------------------
//   ButtonPressedTime(unsigned int *pressedTime)
//
//   Author:   Fehan Arif
// 
//   Date:     2014/10/15
//
//!  This function determines the current button pressed time
//
//------------------------------------------------------------------------------

void ButtonPressedTime(
                       unsigned int *pressedTime    //!< Contains the button pressed time 
                           )
{
    // Get the current button pressed time
    *pressedTime = buttonStructure.time;
}

//------------------------------------------------------------------------------
//   ButtonMostRecentPop(unsigned int *code)
//
//   Author:   Muhammad Shuaib
//
//   Date:    2016/07/12
//
//!  This function clears all button buffer but not most recent and returns most recent status
//
//----------------------------------------------------------------------------

void ButtonMostRecentPop(
                         unsigned int *code   //!< Contains the scanned code of the button
                             )
{
    bool isButtonHit = false;           // Contains the status of button hit, true or false
    unsigned int buttonCode = 0u;       // Contains the code return through ButtonPop
    
    // Checks whether the button is hit or not
    ButtonHit( &isButtonHit );
    // While there are buttons in the buffer
    while ( isButtonHit == true )
    {
        // Checks whether the button is hit or not
        ButtonHit( &isButtonHit );
        if ( isButtonHit != false )
        {
            // Extract the next button code from the buffer
            ButtonPop( &buttonCode );
        }
    }
    *code = buttonCode;
}

//------------------------------------------------------------------------------
//   bool ButtonGetCanStartInitTask(void)
//
//   Author:   Muhammad Shuaib
//   Date:     2016/15/12
//
//!  This function gets the variable that is used to start initialization task
//
//------------------------------------------------------------------------------

bool ButtonGetCanStartInitTask(void)
{
    return canStartInitTask;
}

//==============================================================================
//  End Of File
//==============================================================================