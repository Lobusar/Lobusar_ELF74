#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <ctype.h> 

//includes from diverlib
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "inc/hw_memmap.h"

// defines GPIOS and Buffer
#define BUFFER_SIZE 8        
#define GPIO_PA0_U0RX           0x00000001
#define GPIO_PA1_U0TX           0x00000401

// sets buffers and checkers
uint8_t buffer[BUFFER_SIZE];
uint8_t buffer_position;
uint8_t buffercheck;

void UART_Interruption_Handler(void){ 
  buffer[buffer_position%BUFFER_SIZE] = (uint8_t)UARTCharGetNonBlocking(UART0_BASE); //get the character from UART casted to uint to buffer
  UARTIntClear(UART0_BASE,UARTIntStatus(UART0_BASE,true)); //clear the interruptions on uart
  buffer_position++; //increment the position
}

static uint8_t lowercase (uint8_t ch)
{
  if(isupper(ch))    //check if the values is upper
  {
    ch = (uint8_t)(ch - 'A'); // turn the value into making 0-26 
    ch = (uint8_t)(ch + 'a'); // the sum make it corrected to a lower case
  }
  return ch;
}

int main()
{
  
  //enable Systick for UART
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));
   //configuration of UART   
  // 8N1 120mhz 115200 bps
  UARTConfigSetExpClk(UART0_BASE,(uint32_t)120000000,(uint32_t)115200,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
  UARTFIFODisable(UART0_BASE);
  UARTIntEnable(UART0_BASE,UART_INT_RX|UART_INT_RT);
  UARTIntRegister(UART0_BASE,UART_Interruption_Handler);
 
  
  //enable Systick for GPIO
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
  //configuration of GPIO
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE,(GPIO_PIN_0|GPIO_PIN_1));
  
 
  buffer_position = 0;
  buffercheck = 0;
  uint8_t *charIO; //to modify the  

  while(1){

    if(buffercheck!=buffer_position){
      charIO = &buffer[buffercheck%BUFFER_SIZE];   //get the mensage address in the buffer

      *charIO = lowercase(*charIO); //funcion to turn into lower case
     
      if(UARTCharPutNonBlocking(UART0_BASE,buffer[buffercheck%BUFFER_SIZE])){
        buffercheck++; //update the buffer value
      }
    }      
    
  }
}