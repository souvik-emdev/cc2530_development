#ifndef KEUSACTIONS_H_INCLUDED
#define KEUSACTIONS_H_INCLUDED

//#include "keusconstants.h"


#define MAX_UART_DATA_BUFFER 12
#define TASK_MAP_SWITCH 1
#define TASK_ENTER_DEBUG 2
#define TASK_EXIT_DEBUG 3
#define MAX_NO_OF_BUTTON 4

#define KEUS_BUTTON1 0x1
#define KEUS_BUTTON2 0x2
#define KEUS_BUTTON3 0x4
#define KEUS_BUTTON4 0x8

#define KEUS_APP_EVT1 0X20
#define KEUS_APP_EVT2 0X40
#define KEUS_APP_EVT3 0X80
#define KEUS_APP_EVT4 0X100

#define BUTTON1_ID 1
#define BUTTON2_ID 2
#define BUTTON3_ID 3
#define BUTTON4_ID 4

#define LED_ON 0x00
#define LED_OFF 0x01

#define KEUS_UART_MSG_ACK 0x05
#define EXECUTE_SCENE_COMMAND 0X01
#define DEBUG_INFORMATION 0X02
#define DEBUG_TURNED_ON 0X01
#define DEBUG_TURNED_OFF 0X02

uint8 commandId, uartDataBuffer[MAX_UART_DATA_BUFFER];

KeusGPIOPin ledPin = {0, 0, GPIO_OUTPUT, false, LED_OFF};
KeusGPIOPin ledPin2 = {0, 1, GPIO_OUTPUT, false, LED_OFF};
extern volatile uint32 keusAppEvents;


void exitDebugMode(void);
void enterDebugMode(void);
void parseUart(void);
void blinkLED(void);
void buttonPressAction(uint8 buttonId);
void taskHandler(void);
void mapSwitch(void);
void keusLEDInit(void);
uint8 scene_arr[MAX_NO_OF_BUTTON] = {1, 2, 3, 4};
void blinkerTimerCbk(uint8 timerId);
void debugSequenceTimerCbk(uint8 timerId);
bool debugModeActive = 0;
uint8 debugSequence = 0;
//volatile uint8 buttonPressOrder[MAX_NO_OF_BUTTON] = 0;

KeusTimerConfig ledBlinker = {
    &blinkerTimerCbk,
    100,
    true,
    -1,
    0};

KeusTimerConfig debugSequenceTimer = {
    &debugSequenceTimerCbk,
    500,
    true,
    -1,
    0};

extern void UART_tx(uint8 arr[], uint8 dataLen);
#define KEUS_UART_BUFFER 32
extern uint8 g0_u8RecData[KEUS_UART_BUFFER];

#endif