//*****************************************************************************
//
// blinky.c - Simple example to blink the on-board LED.
//
// Copyright (c) 2013-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.2.0.295 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"



//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Blinky (blinky)</h1>
//!
//! A very simple example that blinks the on-board LED using direct register
//! access.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

volatile uint32_t g_ui32Count = 0; //initialize counter for seconds

void SysTickIntHandler(void){    //Add counts
    g_ui32Count++;
}



//*****************************************************************************
//
// Blink the on-board LED.
//
//*****************************************************************************
int
main(void)

{
    volatile uint32_t ui32Loop;       
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);  // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)){}
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);// Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)){}  // Enable the GPIO pin for the LED (PN0).  Set the direction as output, and enable the GPIO pin for digital function.
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTJ_BASE ,GPIO_PIN_0,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    
    
    // initialization
    uint32_t ui32Time=0;
    uint32_t value=0;
    IntMasterEnable();
    SysTickIntRegister(SysTickIntHandler);  
    SysTickPeriodSet(20000000);  //set board Period
    SysTickIntEnable();
    SysTickEnable();
    g_ui32Count=0; //set second counter to 0
    
    //start game
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0); //turn off the led
    while(g_ui32Count<24){} //1 second counter
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0); //turn on the led
    value= GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0);
    ui32Time = SysTickValueGet();
    g_ui32Count=0;
    
    // time check
    while(value==1 && g_ui32Count<72)//3 second counter
    {
    value= GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0);     
    }
    
    //calculation
    uint32_t reaction_time= SysTickValueGet()-ui32Time + 20000000*g_ui32Count;
    
    //result
    if(g_ui32Count<72)
    printf("Reaction Time: %.4f secs \n", (reaction_time*0.00000000233));
    
    else
    printf("Fail \n");
}
