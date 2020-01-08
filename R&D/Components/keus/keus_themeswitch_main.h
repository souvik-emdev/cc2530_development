#ifndef KEUS_THEMESWITCHMINI_MAIN_H
#define KEUS_THEMESWITCHMINI_MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ZComDef.h"
#include "hal_mcu.h"
#include "keus_themeswitchmini_constants.h"
#include "keus_themeswitch_main.h"
#include "keus_gpio_util.h"

#if DEVICE_TYPE == KEUS_SS3_MINI
typedef struct {
  uint8 btnThemeMap[SCENE_COUNT];
  KeusGPIOPin btnReleasedConfig[THEME_BUTTON_COUNT];
  KeusGPIOPin blinkLEDPin;
  KeusGPIOPin awakeLEDPin;
  uint8 executeScene[SCENE_COUNT];
} KeusThemeSwitchMiniManager;
#endif

extern KeusThemeSwitchMiniManager themeManager;
extern bool allowRetry;
extern bool ssOutOfNetwork;

// main source here
extern void KeusThemeSwitchInit(void);
extern void KeusThemeSwitchMiniRegisterTaskId(uint8 taskId);
extern void KeusChangeThemeBtnConfig(uint8 btnId, uint8 sceneId);
extern bool KeusThemeSwitchMiniWriteConfigDataIntoMemory(void);

extern void KeusThemeSwitchMiniStartCommissioning(void);
extern void KeusThemeSwitchMiniStopCommissioning(void);

extern uint8 KeusThemeSwitchMiniGetScenesToBeExecuted(void);

extern void KeusThemeSwitchMiniLEDBlinker(void);
extern void KeusThemeSwitchMiniClearIgnoreButtonPress(void);
extern void KeusThemeSwitchMiniClearBlinkLED(void);
extern void KeusThemeSwitchMiniConfigActive(void);

extern uint8 KeusThemeSwitchGetBatteryStatus(void);

extern void KeusThemeSwitchMiniLedOn(void);
extern void KeusThemeSwitchMiniLedOff(void);

/* Reference voltage:   Internal 1.15 V,  
Resolution: 12 bits,  
ADC input: VDD/3 (VDD is the battery voltage) */ 

// Max ADC input voltage = reference voltage => 
// (VDD/3) max = 1.15 V => max VDD = 3.45 V 
// 12 bits resolution means that max ADC value = 0x07FF = 2047 (dec)   
// (the ADC value is 2?s complement) 
// Battery voltage, VDD = adc value * (3.45 / 2047) 
// To avoid using a float, the below function will return the battery voltage * 10  
// Battery voltage * 10 = adc value * (3.45 / 2047) * 10 

#define VOLT_MULTIPLIER 0.0168539  // (3.45 / 2047) * 100


#ifdef __cplusplus
}
#endif

#endif