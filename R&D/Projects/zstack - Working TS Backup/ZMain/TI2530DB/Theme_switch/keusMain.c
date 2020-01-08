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
#include "keusconstants.h"

volatile uint32 keusAppEvents = 0;

uint8 KEUS_taskID = 0;

// //Event bit masks
// #define KEUS_BUTTON1 0x1
// #define KEUS_BUTTON2 0x2
// #define KEUS_BUTTON3 0x4
// #define KEUS_BUTTON4 0x8
// #define KEUS_UART 0x10
// #define KEUS_APP_EVT1 0X20
// #define KEUS_APP_EVT2 0X40
// #define KEUS_APP_EVT3 0X80
// #define KEUS_APP_EVT4 0X100

// #define BUTTON1_ID 1
// #define BUTTON2_ID 2
// #define BUTTON3_ID 3
// #define BUTTON4_ID 4

extern void keusButtonInit(void);
extern void exitDebugMode(void);

extern void parseUart(void);
extern void initUart(void);
extern void keusLEDInit(void);
void buttonPressAction(uint8 buttonId);

//void KEUS_delayms(uint16 ms);
//extern void KEUS_init(uin);
void KEUS_init(uint8 taskID);
void KEUS_loop(void); //the actual polling

void KEUS_init(uint8 taskID)
{
  KEUS_taskID = taskID;
  initUart();
  keusButtonInit();
  keusLEDInit();

  KeusTimerUtilInit();
  KeusTimerUtilStartTimer();
  //initStatus = KeusThemeSwitchMiniMemoryInit();
  //osal_nv_item_init(SCN_ARR_NVIC_LOCATION, sizeof(arr_scene), (void *)arr_scene);
  //readStatus = KeusThemeSwitchMiniReadConfigDataIntoMemory();
  //restoring scenes back to memory at startup
  //osal_nv_read(SCN_ARR_NVIC_LOCATION, 0, sizeof(arr_scene), (void *)arr_scene);
  HalUARTWrite(HAL_UART_PORT_0, "KEUS INIT", (byte)osal_strlen("KEUS INIT"));
}


void KEUS_loop()
{
  while (1)
  {
    HalUARTPoll();
    //KEUS_delayms(1000);

    if (keusAppEvents & KEUS_UART)
    {
      parseUart();
      keusAppEvents ^= KEUS_UART;
    }
    else if (keusAppEvents & KEUS_BUTTON1)
    {
      buttonPressAction(BUTTON1_ID);
      keusAppEvents ^= KEUS_BUTTON1;
    }
    else if (keusAppEvents & KEUS_BUTTON2)
    {
      buttonPressAction(BUTTON2_ID);
      keusAppEvents ^= KEUS_BUTTON2;
    }
    else if (keusAppEvents & KEUS_BUTTON3)
    {
      buttonPressAction(BUTTON3_ID);
      keusAppEvents ^= KEUS_BUTTON3;
    }
    else if (keusAppEvents & KEUS_BUTTON4)
    {
      buttonPressAction(BUTTON4_ID);
      keusAppEvents ^= KEUS_BUTTON4;
    }
    else if (keusAppEvents & KEUS_APP_EVT1)
    {
      //action here
      keusAppEvents ^= KEUS_APP_EVT1;
    }
    else if (keusAppEvents & KEUS_APP_EVT2)
    {
      //action here
      keusAppEvents ^= KEUS_APP_EVT2;
    }
    else if (keusAppEvents & KEUS_APP_EVT3)
    {
      //action here
      keusAppEvents ^= KEUS_APP_EVT3;
    }
    else if (keusAppEvents & KEUS_APP_EVT4)
    {
      exitDebugMode();
      keusAppEvents ^= KEUS_APP_EVT4;
    }
  }
}