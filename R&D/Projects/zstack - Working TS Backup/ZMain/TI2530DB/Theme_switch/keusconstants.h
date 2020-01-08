#ifndef KEUSCONSTANTS_H_INCLUDED
#define KEUSCONSTANTS_H_INCLUDED

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



#define KEUS_BUTTON1 0x1
#define KEUS_BUTTON2 0x2
#define KEUS_BUTTON3 0x4
#define KEUS_BUTTON4 0x8
#define KEUS_UART 0x10
#define KEUS_APP_EVT1 0X20
#define KEUS_APP_EVT2 0X40
#define KEUS_APP_EVT3 0X80
#define KEUS_APP_EVT4 0X100

#define BUTTON1_ID 1
#define BUTTON2_ID 2
#define BUTTON3_ID 3
#define BUTTON4_ID 4

extern void keusButtonInit(void);
extern void exitDebugMode(void);

extern void parseUart(void);
extern void initUart(void);
extern void keusLEDInit(void);
extern void buttonPressAction(uint8 buttonId);
void KEUS_init(uint8 taskID);





#ifndef KEUS_COMMON_ENDPOINT
  #define KEUS_COMMON_ENDPOINT 8
  #define KEUS_COMMON_ACK_ENDPOINT 11
#endif

  #define CLICK_TYPES 4
//  #define THEME_BUTTON_COUNT 4
  #define SCENE_COUNT 4
  #define NUM_LEDS 2

#define LED_ACTIVE              GPIO_LOW
#define LED_INACTIVE            GPIO_HIGH

#define BUTTON_ACTIVE           GPIO_LOW
#define BUTTON_INACTIVE         GPIO_HIGH

#define BUTTON_PRESS            GPIO_FALLING_EDGE
#define BUTTON_RELEASE          GPIO_RISING_EDGE

#define KEUS_START_COMMISSIONING_EVENT                  0x0001
#define KEUS_RESET_DEVICE_EVENT                         0x0002
//0x0004 used for end device rejoin event
//#define KEUS_INITIAL_COMMISSIONOFF_EVENT                0x0008

#define KEUS_THEMESWITCH_CONFIG_EVENT                   0x0010       
//skipping ignore button press event
//#define KEUS_THEMESWITCH_IGNOREBUTTON_EVENT             0x0020
#define KEUS_THEMESWITCH_BLINKER_EVENT                  0x0020
#define KEUS_THEMESWITCH_BLINK_EVENT                    0x0040
#define KEUS_EXECUTE_SCENE_EVENT                        0x0080

#define KEUS_THEMESWITCH_MASTER_INFORM_EVENT           0x0100
#define KEUS_THEMESWITCH_BATTERY_UPDATE_EVENT           0x0400

#define KEUS_INITIAL_COMMISSIONOFF_EVENT_TIME           5000
#define KEUS_INITIAL_COMMISSIONOFF_EVENT_DELAY          2000
#define KEUS_THEMESWITCH_CONFIG_EVENT_TIME              500
#define KEUS_THEMESWITCH_BLINK_EVENT_TIME               70
#define KEUS_THEMESWITCH_IGNOREBUTTON_EVENT_TIME        100
#define KEUS_THEMESWITCH_MASTER_INFORM_INTERVAL         2500
#define KEUS_THEMESWITCH_BATTERY_UPDATE_EVENT_TIME      900

#define KEUS_THEMESWITCH_BATTERY_UPDATE_EVENT_COUNT     10

#define KEUS_THEMESWITCH_LED_BLINK_TIME 500
#define KEUS_THEMESWITCH_IGNORE_INTERRUPT_TIME 700

#define KEUS_THEMESWITCH_SCENE_EXECUTE_INTERVAL 100

#define KEUS_EMBEDDEDSWITCH_BUTTONSCENEIDMAP_MEMORYID 0x405

#if DEVICE_TYPE == KEUS_SS3_MINI
  // Button mappings
  #define BUTTON1_REL_PORT        0
  #define BUTTON1_REL_BIT         0
  
  #define BUTTON2_REL_PORT        2
  #define BUTTON2_REL_BIT         0
  
  #define BUTTON3_REL_PORT        0
  #define BUTTON3_REL_BIT         1     
  
  #define BUTTON4_REL_PORT        1
  #define BUTTON4_REL_BIT         1

  #define BLINK_LED_PORT          2
  #define BLINK_LED_BIT           1
  
  #define AWAKE_LED_PORT          1
  #define AWAKE_LED_BIT           0

#endif

//#define KEUS_THEMESWITCH_SLEEP_POLLRATE         240000
//#define KEUS_THEMESWITCH_REJOIN_POLLRATE        40000
//#define KEUS_THEMESWITCH_ACTIVE_POLLRATE        2000        

#define KEUS_THEMESWITCH_SLEEP_POLLRATE         (140000 + osal_rand())
#define KEUS_THEMESWITCH_REJOIN_POLLRATE        (25000 + (osal_rand()%30000))
#define KEUS_THEMESWITCH_ACTIVE_POLLRATE        (1500 + (osal_rand()%1000))        


#endif