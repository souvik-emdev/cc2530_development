#include "hal_adc.h"
#include "hal_flash.h"
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_uart.h"
#include "hal_drivers.h"
#include "OnBoard.h"
#include "OSAL.h"
#include "OSAL_Nv.h"
#include "ZComDef.h"
#include "ZMAC.h"
#include "keus_gpio_util.h"
#include "keus_timer_util.h"
#include "keusbutton.h"

void debounceTimerCbk(uint8 timerId)
{
  debounce_read = true;
  KeusTimerUtilRemoveTimer(&debounceTimer);
}

void keusButtonInit(void)
{
  KeusGPIOSetDirection(&buttonPin);
  KeusGPIOSetDirection(&buttonPin2);
  KeusGPIOSetDirection(&buttonPin3);
  KeusGPIOSetDirection(&buttonPin4);

  KeusGPIOEdgeConfiguration(&buttonPin, GPIO_RISING_EDGE);
  KeusGPIOEdgeConfiguration(&buttonPin2, GPIO_RISING_EDGE);
  KeusGPIOEdgeConfiguration(&buttonPin3, GPIO_RISING_EDGE);
  KeusGPIOEdgeConfiguration(&buttonPin4, GPIO_RISING_EDGE);
  P1IFG = 0; //Fix for one int hitting at first code run
  KeusGPIOInterruptEnable(&buttonPin);
  KeusGPIOInterruptEnable(&buttonPin2);
  KeusGPIOInterruptEnable(&buttonPin3);
  KeusGPIOInterruptEnable(&buttonPin4);
}

HAL_ISR_FUNCTION(halKeusPort1Isr, P1INT_VECTOR)
{
  HAL_ENTER_ISR();

  if (debounce_read)
  {
    debounce_read = false; //make sure it does not read untill the debounce timer CB sets it again

    if (P1IFG & BV(buttonPin.bit))
    {
      keusAppEvents |= KEUS_BUTTON1;
    }

    if (P1IFG & BV(buttonPin2.bit))
    {
      keusAppEvents |= KEUS_BUTTON2;
    }

    if (P1IFG & BV(buttonPin3.bit))
    {
      keusAppEvents |= KEUS_BUTTON3;
    }

    if (P1IFG & BV(buttonPin4.bit))
    {
      keusAppEvents |= KEUS_BUTTON4;
    }

    KeusTimerUtilAddTimer(&debounceTimer);
  }
  /*
   Clear the CPU interrupt flag for Port_0
   PxIFG has to be cleared before PxIF
 */
  P1IFG = 0;
  P1IF = 0;

  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}