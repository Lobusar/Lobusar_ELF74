#include "tx_api.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"


#define DEMO_BYTE_POOL_SIZE     9120
#define DEMO_STACK_SIZE         1024

TX_THREAD               thread_1;
TX_THREAD               thread_2;
TX_THREAD               thread_3;
TX_BYTE_POOL            byte_pool_0;
TX_MUTEX                mutex_0;

UCHAR                   byte_pool_memory[DEMO_BYTE_POOL_SIZE];




void TurnLedsOFF(){
  GPIOPinWrite(GPIO_PORTN_BASE,(GPIO_PIN_0 | GPIO_PIN_1), (GPIO_PIN_0 | GPIO_PIN_1) & (0));
  GPIOPinWrite(GPIO_PORTF_BASE,(GPIO_PIN_0 | GPIO_PIN_4), (GPIO_PIN_0 & 0) | (GPIO_PIN_4 & (0)));
}
void TurnLedsOn(uint8_t led){
  GPIOPinWrite(GPIO_PORTN_BASE,(GPIO_PIN_0 | GPIO_PIN_1), (GPIO_PIN_0 | GPIO_PIN_1) & (led>>2));
  GPIOPinWrite(GPIO_PORTF_BASE,(GPIO_PIN_0 | GPIO_PIN_4), (GPIO_PIN_0 & led) | (GPIO_PIN_4 & (led<<3)));
}

uint8_t scheduler(){
  // set the value for testing
  return 4; // a->0  b->1 c->2 d->3 e->4 
}

uint8_t loop_wait_led(uint8_t led, uint32_t wait_time){

  uint32_t wait_counter = 0;
  

  while(wait_counter<wait_time){
    TurnLedsOn(led);
    wait_counter++;
    TurnLedsOFF();
  }

  return 0;
}

void thread_entry(ULONG thread_init){
  
  if(thread_init==1){
    while(1){
      //Using on D and E
      if(scheduler()==3 | scheduler()==4){
        UINT status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
        if (status != TX_SUCCESS)
            break;
        loop_wait_led(8, 206900);
        tx_thread_sleep(500);   
        status = tx_mutex_put(&mutex_0);
        if (status != TX_SUCCESS)
            break;
      }
      //Using on A, B and C
      else{
        loop_wait_led(8, 206900);
        tx_thread_sleep(500);
      }

    }
  }

  else if(thread_init==2){
    while(1){
      loop_wait_led(4, 344830);
      tx_thread_sleep(750);
    }
  }

  else if(thread_init==3){
    while(1){
      //Using on D and E
      if(scheduler()==3 | scheduler()==4){
        UINT status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
        if (status != TX_SUCCESS)
          break;
        
        loop_wait_led(2, 551725);
        tx_thread_sleep(2000);

        status = tx_mutex_put(&mutex_0);
        if (status != TX_SUCCESS)
          break;

      //Using on A, B and C
      }else{
        loop_wait_led(2, 551725);
        tx_thread_sleep(2000);
      }

    }
  }

}


 int main(){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4 );
  
  TurnLedsOFF();
  
  tx_kernel_enter();
}


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

CHAR    *pointer = TX_NULL;


#ifdef TX_ENABLE_EVENT_TRACE
    tx_trace_enable(trace_buffer, sizeof(trace_buffer), 32);
#endif
    UINT time_slice_threads[3];
    UINT priority_threads[3];
    UINT preemption_threads[3];

    for(int i=0;i<3;i++){
      //Using on A
      if(scheduler()==0){
        priority_threads[i]=0;
        preemption_threads[i]=0;
        time_slice_threads[i]=25;
      }
      //Using on C, D and E
      else{
        priority_threads[i]=i;
        preemption_threads[i]=i;
        time_slice_threads[i] = TX_NO_TIME_SLICE;
      }
    }
    //Using on B
    if(scheduler()==1){
      for(int i=0;i<3;i++){
        preemption_threads[i]=0;
      }
    }
    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", byte_pool_memory, DEMO_BYTE_POOL_SIZE);

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */


    /* Allocate the stack for thread 1.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 1 and 2. These threads pass information through a ThreadX 
       message queue.  It is also interesting to note that these threads have a time
       slice.  */
    tx_thread_create(&thread_1, "thread 1", thread_entry, 1,  
            pointer, DEMO_STACK_SIZE, 
            priority_threads[0], preemption_threads[0], time_slice_threads[0], TX_AUTO_START);

    /* Allocate the stack for thread 2.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&thread_2, "thread 2", thread_entry, 2,  
            pointer, DEMO_STACK_SIZE, 
            priority_threads[1], preemption_threads[1], time_slice_threads[1], TX_AUTO_START);

    // /* Allocate the stack for thread 3.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // /* Create threads 3 and 4.  These threads compete for a ThreadX counting semaphore.  
    //    An interesting thing here is that both threads share the same instruction area.  */
    tx_thread_create(&thread_3, "thread 3", thread_entry, 3,  
            pointer, DEMO_STACK_SIZE, 
            priority_threads[2], preemption_threads[2], time_slice_threads[2], TX_AUTO_START);

    //Mutex creation for D
    if(scheduler()==3){
      tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);
    }
    else if(scheduler()==4){
      tx_mutex_create(&mutex_0, "mutex 0", TX_INHERIT);
    }

   
}
