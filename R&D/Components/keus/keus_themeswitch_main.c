#include "keus_themeswitch_main.h"
#include "keus_themeswitchmini_constants.h"
#include "keus_gpio_util.h"
//#include "keus_timer_util.h"
#include "OSAL.h"
#include "OSAL_Nv.h"
#include "OSAL_Clock.h"
#include "OSAL_PwrMgr.h"
#include "NLMEDE.h"
#include "hal_adc.h"

#include "bdb_interface.h"

//#define ENABLE_BUTTON_CONFIG 1

void KeusThemeSwitchMiniIgnoreInterrupt(uint8 timerId) ;

/* Memory Related */
bool KeusThemeSwitchMiniMemoryInit(void);
bool KeusThemeSwitchMiniReadConfigDataIntoMemory(void);

/* Event related */

void KeusThemeSwitchMiniExecuteScene(void);


/* GPIO Activity */
void KeusThemeSwitchMiniButtonPressed(uint8 btnId);
void KeusThemeSwitchMiniButtonReleased(uint8 btnId);
void KeusThemeSwitchMiniButtonPressed_awake(uint8 btnId);
void KeusThemeSwitchMiniButtonReleased_awake(uint8 btnId);

/*Power related */
uint8 KeusThemeSwitchMini_wake( void );
uint8 KeusThemeSwitchMini_sleep( void );

void KeusThemeSwitchMiniClearSceneExecutions(void);
void KeusThemeSwitchMiniConfigInactive(void);

bool awake = true;
bool ignoreInterrupts = true;
bool ignoreButtonPress = false;

bool ssOutOfNetwork = true;
bool blinkRed = false;
bool allowRetry = false;
uint8 timerStatus = SUCCESS;

KeusThemeSwitchMiniManager themeManager = {
  {0, 1, 2, 3 },
  {
    {BUTTON1_REL_PORT, BUTTON1_REL_BIT, GPIO_INPUT, false, BUTTON_INACTIVE},
    {BUTTON2_REL_PORT, BUTTON2_REL_BIT, GPIO_INPUT, false, BUTTON_INACTIVE},
    {BUTTON3_REL_PORT, BUTTON3_REL_BIT, GPIO_INPUT, false, BUTTON_INACTIVE},
    {BUTTON4_REL_PORT, BUTTON4_REL_BIT, GPIO_INPUT, false, BUTTON_INACTIVE},
  },
  {BLINK_LED_PORT, BLINK_LED_BIT, GPIO_OUTPUT, false, LED_INACTIVE},
  {AWAKE_LED_PORT, AWAKE_LED_BIT, GPIO_OUTPUT, false, LED_INACTIVE},
  {255, 255, 255, 255}
};

//KeusTimerConfig blinkTimerConfig = {
//  &KeusThemeSwitchMiniLEDBlinker,
//  KEUS_THEMESWITCH_LED_BLINK_TIME,
//  true,
//  -1,
//  0
//};
//
//KeusTimerConfig ignoreInterruptTimerConfig = {
//  &KeusThemeSwitchMiniIgnoreInterrupt,
//  KEUS_THEMESWITCH_IGNORE_INTERRUPT_TIME,
//  true,
//  -1,
//  0
//};

bool init = false;

#define LAST_CONFIG_INDEX       7
uint8 configActivationCode[] = {1,2,3,0,1,0,2,3}; 
uint8 activationCodePos = 0;
bool configMessageInProgress = false;
bool executingScenes = false;

bool buttonHeld = false;
bool configModeActive = false;

uint8 lastClick = 255;

static uint8 KeusThemeSwitchMiniTaskId = 0x00;

/*********************************************************************
 * @fn          KeusThemeSwitchMiniInit
 * @brief       Initialize Themeswitch
 * @return      
 */

void KeusThemeSwitchInit(void) {
  
#ifdef TS200
  KeusThemeSwitchMiniMemoryInit();
  KeusThemeSwitchMiniReadConfigDataIntoMemory();
#endif
  
  for (uint8 i = 0; i < THEME_BUTTON_COUNT; i++) {
    
    KeusGPIOSetDirection(&themeManager.btnReleasedConfig[i]);
    KeusGPIOReadPinValue(&themeManager.btnReleasedConfig[i]);
    KeusGPIOEdgeConfiguration(&themeManager.btnReleasedConfig[i], BUTTON_RELEASE);
    KeusGPIOInterruptEnable(&themeManager.btnReleasedConfig[i]);
  }
  
  for (uint8 i = 0; i < THEME_BUTTON_COUNT; i++) {
    KeusGPIOReadPinValue(&themeManager.btnReleasedConfig[i]);
  }

  KeusGPIOSetDirection(&themeManager.blinkLEDPin);
  themeManager.blinkLEDPin.state = LED_INACTIVE;
  KeusGPIOSetPinValue(&themeManager.blinkLEDPin);
  
  //to ignore initialization interrupts
  osalTimeUpdate();
  timerStatus = osal_start_timerEx(KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_BLINK_EVENT, KEUS_THEMESWITCH_BLINK_EVENT_TIME);
  ignoreButtonPress = false;
  
  
  KeusGPIOSetDirection(&themeManager.awakeLEDPin);
  themeManager.awakeLEDPin.state = LED_INACTIVE;
  KeusGPIOSetPinValue(&themeManager.awakeLEDPin); 
  
  //osalTimeUpdate();
  //KeusThemeSwitchMiniStartCommissioning();
  //osal_start_timerEx(KeusThemeSwitchMiniTaskId, KEUS_INITIAL_COMMISSIONOFF_EVENT, KEUS_INITIAL_COMMISSIONOFF_EVENT_TIME);
  awake = KeusThemeSwitchMini_sleep();
}

/*********************************************************************
 * @fn          KeusThemeSwitchMiniRegisterTaskId
 * @brief       Register taskId for events
 * @return      
 */

void KeusThemeSwitchMiniRegisterTaskId(uint8 taskId) {
  KeusThemeSwitchMiniTaskId = taskId;
}

/*********************************************************************
 * @fn          KeusThemeSwitchMiniLEDBlinker
 * @brief       LED Blinker for commissioning
 * @return      
 */
void KeusThemeSwitchMiniLEDBlinker(void) {

  if(allowRetry)
    themeManager.blinkLEDPin.state ^= 1;
  else
    themeManager.blinkLEDPin.state = LED_INACTIVE;
  
  KeusGPIOSetPinValue(&themeManager.blinkLEDPin);
  osalTimeUpdate();  
}

/*********************************************************************
 * @fn          KeusThemeSwitchMiniIgnoreInterrupt
 * @brief       Ignore initial interrupts
 * @return      
 */
void KeusThemeSwitchMiniIgnoreInterrupt(uint8 timerId) {

  ignoreInterrupts = false;
  //KeusTimerUtilRemoveTimer(&ignoreInterruptTimerConfig); 
  //awake = KeusThemeSwitchMini_sleep(); 
}


/*

Memory

*/

/*********************************************************************
 * @fn          KeusThemeSwitchMiniMemoryInit
 * @brief       Initialize memory for storing clickId-SceneId mapping
 * @return      success/fail
 */

bool KeusThemeSwitchMiniMemoryInit(void) {
  for (uint8 i = 0; i < CLICK_TYPES; i++) {
    themeManager.btnThemeMap[i] = 255;
  }

  uint8 res = osal_nv_item_init(KEUS_EMBEDDEDSWITCH_BUTTONSCENEIDMAP_MEMORYID, CLICK_TYPES, (void *) themeManager.btnThemeMap);
  
  if (res == SUCCESS || res == NV_ITEM_UNINIT) {
    return true;
  } else {
    return false;
  }
}

/*********************************************************************
 * @fn          KeusThemeSwitchMiniReadConfigDataIntoMemory
 * @brief       Read clickId-SceneId mapping from memory
 * @return      success/fail
 */

bool KeusThemeSwitchMiniReadConfigDataIntoMemory(void) {
  uint8 res = osal_nv_read(KEUS_EMBEDDEDSWITCH_BUTTONSCENEIDMAP_MEMORYID, 0, CLICK_TYPES, (void *) themeManager.btnThemeMap);

  if (res == SUCCESS) {
    return true;
  } else {
    return false;
  }
}

/*********************************************************************
 * @fn          KeusThemeSwitchMiniWriteConfigDataIntoMemory
 * @brief       Write clickId-SceneId mapping from memory
 * @return      success/fail
 */

bool KeusThemeSwitchMiniWriteConfigDataIntoMemory(void) {
  uint8 res = osal_nv_write(KEUS_EMBEDDEDSWITCH_BUTTONSCENEIDMAP_MEMORYID, 0, CLICK_TYPES, (void *) themeManager.btnThemeMap);

  if (res == SUCCESS) {
    return true;
  } else {
    return false;
  }
}


/*

Power Modes

*/

/*********************************************************************
 * @fn          KeusThemeSwitchMini_wake
 * @brief       Wake up end device
 * @return      sleep state
 */
uint8 KeusThemeSwitchMini_wake( void )
{
  osal_pwrmgr_device( PWRMGR_ALWAYS_ON );
  //allowRetry = true;
  NLME_SetPollRate( KEUS_THEMESWITCH_ACTIVE_POLLRATE );
  //NLME_SetQueuedPollRate( KEUS_THEMESWITCH_ACTIVE_POLLRATE );
  //NLME_SetResponseRate( KEUS_THEMESWITCH_ACTIVE_POLLRATE );
  
  osalTimeUpdate();
  //KeusTimerUtilStartTimer();
  
  return SS_AWAKE;
}

/*********************************************************************
 * @fn          KeusThemeSwitchMini_sleep
 * @brief       Sleep end device
 * @return      sleep state
 */
uint8 KeusThemeSwitchMini_sleep( void )
{
  osalTimeUpdate();
  //KeusTimerUtilStopTimer();
  
  if(allowRetry)
    KeusThemeSwitchMiniStopCommissioning();
  NLME_SetPollRate( KEUS_THEMESWITCH_SLEEP_POLLRATE );
  //NLME_SetQueuedPollRate( KEUS_THEMESWITCH_SLEEP_POLLRATE );
  //NLME_SetResponseRate( KEUS_THEMESWITCH_SLEEP_POLLRATE );
  osal_pwrmgr_device( PWRMGR_BATTERY );
  
  return SS_SLEEPING;
}

/*

Events

*/

  uint16 sum = 0;    
  uint16 batteryReading =0;
  float batteryValue =0.0;
  
  float batteryPercent = 0.0;
  uint16 sum_z =0;

/*********************************************************************
 * @fn          KeusThemeSwitchGetBatteryStatus
 * @brief       calculates and Returns battery percentage
 * @return      battery percentage
 */
uint8 KeusThemeSwitchGetBatteryStatus(void)
{
  
//  osal_pwrmgr_device( PWRMGR_ALWAYS_ON );
//  led.state = LED_ACTIVE;
//  KeusGPIOSetPinValue(&led);
  
  HalAdcSetReference(HAL_ADC_REF_125V);
  
  uint8 i =0;
  sum_z = 0;   
  
  while(i++ < 10 )
  {
    
    batteryReading = HalAdcRead(HAL_ADC_CHN_VDD3, HAL_ADC_RESOLUTION_12); 
    batteryValue = batteryReading * VOLT_MULTIPLIER;
    batteryPercent = ((batteryValue-22.0000)/5.00000)*100;
      
    sum_z += (batteryPercent > 0 ? batteryPercent*10 : 0);  
//    sum += (uint8)batteryPercent;
    
    sum = sum_z/10;
  }
  
//  osal_pwrmgr_device( PWRMGR_BATTERY );
  
  return sum/i;
}


/*********************************************************************
 * @fn          KeusThemeSwitchMiniClearIgnoreButtonPress
 * @brief       IgnoreButton Press  = false
 * @return      
 */

void KeusThemeSwitchMiniClearIgnoreButtonPress(void) {

  ignoreButtonPress = false;
}


/*********************************************************************
 * @fn          KeusThemeSwitchMiniClearBlinkLED
 * @brief       TurnOff all LEDs 
 * @return      
 */

void KeusThemeSwitchMiniClearBlinkLED(void) {

  if(!configModeActive)
  {
    ignoreInterrupts = false;
    themeManager.awakeLEDPin.state = LED_INACTIVE;
    KeusGPIOSetPinValue(&themeManager.awakeLEDPin);
    
    if(blinkRed)
    {
      themeManager.blinkLEDPin.state = LED_ACTIVE;
      KeusGPIOSetPinValue(&themeManager.blinkLEDPin);
      osal_start_timerEx(KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_BLINK_EVENT, KEUS_THEMESWITCH_BLINK_EVENT_TIME);
      blinkRed = false;
    }
    else
    {
      themeManager.blinkLEDPin.state = LED_INACTIVE;
      KeusGPIOSetPinValue(&themeManager.blinkLEDPin);
    }
    
  }
  else
  {
    themeManager.blinkLEDPin.state = LED_INACTIVE;
    KeusGPIOSetPinValue(&themeManager.blinkLEDPin);
    ignoreInterrupts = false;
  }
  

    //themeManager.blinkLEDPin.state = LED_INACTIVE;
    //KeusGPIOSetPinValue(&themeManager.blinkLEDPin);
}


/*********************************************************************
 * @fn          KeusThemeSwitchMiniGetScenesToBeExecuted
 * @brief       go through execute scene list and return scenes to be executed
 * @return      sceneID if true, 255 if no scene to be executed
 */

uint8 KeusThemeSwitchMiniGetScenesToBeExecuted(void) {
  
  for(uint8 i = 0; i < CLICK_TYPES ; i++)
  {
    if(themeManager.executeScene[i] == 0)
    {
      themeManager.executeScene[i] = 255;
      timerStatus = osal_start_timerEx(KeusThemeSwitchMiniTaskId, KEUS_EXECUTE_SCENE_EVENT, KEUS_THEMESWITCH_SCENE_EXECUTE_INTERVAL);
      return themeManager.btnThemeMap[i];
    }
  }
  executingScenes = false;
  return 255;

}

/*********************************************************************
 * @fn          KeusThemeSwitchMiniConfigActive
 * @brief       Trigger to set the config into active mode
 * @return      
 */

void KeusThemeSwitchMiniConfigActive(void) {
  
  configMessageInProgress = false;
  activationCodePos = 0; 
  KeusThemeSwitchMiniExecuteScene();
}

/*********************************************************************
 * @fn          KeusThemeSwitchMiniConfigInactive
 * @brief       Trigger to set the config into inactive mode and set the device back to sleep mode
 * @return      
 */

void KeusThemeSwitchMiniConfigInactive(void) {
  
  themeManager.awakeLEDPin.state = LED_INACTIVE;
  KeusGPIOSetPinValue(&themeManager.awakeLEDPin);
 
  awake = KeusThemeSwitchMini_sleep();
  
  configModeActive = false;
}


/*

Awake Functions

*/

/*********************************************************************
 * @fn          KeusThemeSwitchMiniButtonReleased
 * @brief       On button release, start double click timer to set execute scene event
 * @return      
 */

void KeusThemeSwitchMiniButtonReleased_awake(uint8 btnId)
{
  osalTimeUpdate();
  themeManager.blinkLEDPin.state = LED_ACTIVE;
  KeusGPIOSetPinValue(&themeManager.blinkLEDPin);
  
  timerStatus = osal_start_timerEx(KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_BLINK_EVENT, KEUS_THEMESWITCH_BLINK_EVENT_TIME);
  
   switch(btnId)
  {
  case 0:
    //not using this as it is used to enter configMode, might interfere
    KeusThemeSwitchMiniClearSceneExecutions();
    //osal_set_event( KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_BATTERY_UPDATE_EVENT);
    break;
    
  case 1:
    //start/stop commissioning
    if(allowRetry)
      KeusThemeSwitchMiniStopCommissioning();
    else
      KeusThemeSwitchMiniStartCommissioning();
    
    break;
    
  case 2:
    if(allowRetry)
      KeusThemeSwitchMiniStopCommissioning();
    //reset
    osal_set_event( KeusThemeSwitchMiniTaskId, KEUS_RESET_DEVICE_EVENT);
    break;
    
  case 3:
    //quit config mode
    KeusThemeSwitchMiniConfigInactive();
    themeManager.blinkLEDPin.state = LED_INACTIVE;
    KeusGPIOSetPinValue(&themeManager.blinkLEDPin);    
    break;
    
  default:
    //no other button can be pressed
    break;
    
  }
  
}


/*********************************************************************
 * @fn          KeusThemeSwitchMiniStartCommissioning
 * @brief       Check waake status and start commissioning, add a blinker 
 * @return      
 */

void KeusThemeSwitchMiniStartCommissioning(void)
{
   if(!awake) 
    awake = KeusThemeSwitchMini_wake();
   osal_set_event( KeusThemeSwitchMiniTaskId, KEUS_START_COMMISSIONING_EVENT);
   
   osalTimeUpdate();
   osal_start_reload_timer(KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_BLINKER_EVENT, KEUS_THEMESWITCH_LED_BLINK_TIME);
   
   //KeusTimerUtilRemoveTimer(&blinkTimerConfig); 
   //KeusTimerUtilAddTimer(&blinkTimerConfig); 
   
   allowRetry = true;
}

/*********************************************************************
 * @fn          f
 * @brief       Check waake status and start commissioning, add a blinker 
 * @return      
 */

void KeusThemeSwitchMiniStopCommissioning(void)
{
  
  //KeusTimerUtilRemoveTimer(&blinkTimerConfig);
  
  osal_start_timerEx(KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_BLINKER_EVENT, 20);
  osal_stop_timerEx(KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_BLINKER_EVENT);
  
  osalTimeUpdate();
  themeManager.blinkLEDPin.state = LED_INACTIVE;
  KeusGPIOSetPinValue(&themeManager.blinkLEDPin);
  
  allowRetry = false;
  if(!configModeActive)
    awake = KeusThemeSwitchMini_sleep(); 
}

/*********************************************************************
 * @fn          KeusChangeThemeBtnConfig
 * @brief       Change clickId-sceneiD mapping
 * @return      
 */

void KeusChangeThemeBtnConfig(uint8 btnId, uint8 sceneId) {
  if (btnId < CLICK_TYPES) {
    #ifdef TS200
      themeManager.btnThemeMap[btnId] = sceneId;
    #endif
    
  }
}

/*********************************************************************
 * @fn          KeusThemeSwitchMiniClearSceneExecutions
 * @brief       set all scene executions to 255
 * @return      
 */

void KeusThemeSwitchMiniClearSceneExecutions(void) {

  for (uint8 i = 0; i < CLICK_TYPES; i++) {
    themeManager.executeScene [i] = 255;
  }
}


/*

Sleep Functions

*/


/*********************************************************************
 * @fn          KeusThemeSwitchMiniButtonReleased
 * @brief       On button release, start double click timer to set execute scene event
 * @return      
 */

uint16 batteryCounter = 0;


void KeusThemeSwitchMiniButtonReleased(uint8 btnId)
{
  osalTimeUpdate();
  
  if(!ignoreButtonPress)
  {
    themeManager.executeScene[btnId] = 0;

    if(!bdb_isDeviceNonFactoryNew())
    {
      themeManager.blinkLEDPin.state = LED_ACTIVE;
      KeusGPIOSetPinValue(&themeManager.blinkLEDPin);

    }
    else {
      themeManager.awakeLEDPin.state = LED_ACTIVE;
      KeusGPIOSetPinValue(&themeManager.awakeLEDPin);
      if(ssOutOfNetwork)
        blinkRed = true;
    }
    
  }
  
  if(activationCodePos == 0 && btnId == configActivationCode[activationCodePos] && !ignoreButtonPress)
  {
    configMessageInProgress = true;
    timerStatus = osal_start_timerEx(KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_CONFIG_EVENT, KEUS_THEMESWITCH_CONFIG_EVENT_TIME);    
    activationCodePos++;
  }
  else if(configMessageInProgress && btnId == configActivationCode[activationCodePos])
  {
    
    if(activationCodePos == LAST_CONFIG_INDEX)
    {
      // config Mode active
      
      awake = KeusThemeSwitchMini_wake();
      
      configModeActive = true;
      themeManager.awakeLEDPin.state = LED_ACTIVE;
      KeusGPIOSetPinValue(&themeManager.awakeLEDPin);
      KeusThemeSwitchMiniClearSceneExecutions();
      
      //configMessageInProgress = false;
      //activationCodePos = 0;
      
      timerStatus = osal_start_timerEx(KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_CONFIG_EVENT, 10);    
    }
    else
    {
      //configMessageInProgress = true;
      timerStatus = osal_start_timerEx(KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_CONFIG_EVENT, KEUS_THEMESWITCH_CONFIG_EVENT_TIME);    
      activationCodePos++;    
    }
  }
  else
  {
      configMessageInProgress = false;
      activationCodePos = 0;  
      
      //batteryCounter++;
      if(batteryCounter > KEUS_THEMESWITCH_BATTERY_UPDATE_EVENT_COUNT)
      {
        batteryCounter = 0;
        osal_start_timerEx(KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_BATTERY_UPDATE_EVENT, KEUS_THEMESWITCH_BATTERY_UPDATE_EVENT_TIME);    
        osal_pwrmgr_device( PWRMGR_ALWAYS_ON );
      }
  }  
  
  if(!configMessageInProgress && !executingScenes && !ignoreButtonPress)
  {
    executingScenes = true;
    KeusThemeSwitchMiniExecuteScene();
  }
  
  timerStatus = osal_start_timerEx(KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_BLINK_EVENT, KEUS_THEMESWITCH_BLINK_EVENT_TIME);
  ignoreButtonPress = false;
  //ignoreButtonPress = true;
  //timerStatus = osal_start_timerEx(KeusThemeSwitchMiniTaskId, KEUS_THEMESWITCH_IGNOREBUTTON_EVENT, KEUS_THEMESWITCH_IGNOREBUTTON_EVENT_TIME);
  
}

/*********************************************************************
 * @fn          KeusThemeSwitchMiniExecuteScene
 * @brief       Set the execute scene Event
 * @return      
 */


void KeusThemeSwitchMiniExecuteScene(void) {
  osal_set_event( KeusThemeSwitchMiniTaskId, KEUS_EXECUTE_SCENE_EVENT);
}


/*

Interrupts

*/


HAL_ISR_FUNCTION( halKeusPort0Isr, P0INT_VECTOR )
{
  HAL_ENTER_ISR();
  
  if(!ignoreInterrupts)
  {
    for(uint8 i= 0; i< THEME_BUTTON_COUNT; i++ )
    {
      if ((themeManager.btnReleasedConfig[i].port == 0) && (P0IFG & BV( themeManager.btnReleasedConfig[i].bit ))) 
      {
        ignoreInterrupts = true;
        
        if(configModeActive)
          KeusThemeSwitchMiniButtonReleased_awake(i);
        else 
          KeusThemeSwitchMiniButtonReleased(i);
        
      }  
    }  
  }
  
  /*
  Clear the CPU interrupt flag for Port_0
  PxIFG has to be cleared before PxIF
  */
  P0IFG = 0;
  P0IF = 0;
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}

HAL_ISR_FUNCTION( halKeusPort1Isr, P1INT_VECTOR )
{
  HAL_ENTER_ISR();
  
  if(!ignoreInterrupts)
  {
    for(uint8 i= 0; i< THEME_BUTTON_COUNT; i++ )
    {
      if ((themeManager.btnReleasedConfig[i].port == 1) && (P1IFG & BV( themeManager.btnReleasedConfig[i].bit ))) 
      {
        ignoreInterrupts = true;
        
        if(configModeActive)
          KeusThemeSwitchMiniButtonReleased_awake(i);
        else 
          KeusThemeSwitchMiniButtonReleased(i);
        
      }  
    }  
  }
  /*
  Clear the CPU interrupt flag for Port_2
  PxIFG has to be cleared before PxIF
  Notes: P2_1 and P2_2 are debug lines.
  */
  P1IFG = 0;
  P1IF = 0;
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}

HAL_ISR_FUNCTION( halKeusPort2Isr, P2INT_VECTOR ) {
  HAL_ENTER_ISR();
  
  if(!ignoreInterrupts)
  {
    //   Incase of generic interrupt checker  
    for(uint8 i =0; i < THEME_BUTTON_COUNT; i++)
    {
      if((themeManager.btnReleasedConfig[i].port == 2) && (P2IFG & BV( themeManager.btnReleasedConfig[i].bit )) )
      {
        
        ignoreInterrupts = true;
        
        if(configModeActive)
          KeusThemeSwitchMiniButtonReleased_awake(i);
        else
          KeusThemeSwitchMiniButtonReleased(i);
        
        
      }
    }
  }
  /*
  Clear the CPU interrupt flag for Port_0
  PxIFG has to be cleared before PxIF
  */
  P2IFG = 0;
  P2IF = 0;
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}