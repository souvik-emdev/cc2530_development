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
#include "keusactions.h"

void debugSequenceTimerCbk(uint8 timerId)
{
    debugSequence = 0;
    KeusTimerUtilRemoveTimer(&debugSequenceTimer);
}

void enterDebugMode(void)
{
    debugModeActive = 1;
    ledPin.state = LED_ON;
    KeusGPIOSetPinValue(&ledPin);
    uint8 sendReply[2] = {DEBUG_INFORMATION, DEBUG_TURNED_ON};
    UART_tx(sendReply, 2);
}

void exitDebugMode(void)
{
    debugModeActive = 0;
    ledPin.state = LED_OFF;
    ledPin2.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin);
    KeusGPIOSetPinValue(&ledPin2);
    uint8 sendReply[2] = {DEBUG_INFORMATION, DEBUG_TURNED_OFF};
    UART_tx(sendReply, 2);
}

void blinkerTimerCbk(uint8 timerId)
{
    if (!debugModeActive)
    {
        ledPin.state = LED_OFF;
        KeusGPIOSetPinValue(&ledPin);
    }
    else
    {
        ledPin2.state = LED_OFF;
        KeusGPIOSetPinValue(&ledPin2);
    }
    KeusTimerUtilRemoveTimer(&ledBlinker);
}

void blinkLED(void)
{
    if (!debugModeActive)
    {
        ledPin.state = LED_ON;
        KeusGPIOSetPinValue(&ledPin);
    }
    else
    {
        ledPin2.state = LED_ON;
        KeusGPIOSetPinValue(&ledPin2);
    }
    KeusTimerUtilAddTimer(&ledBlinker);
}

void keusLEDInit(void)
{
    KeusGPIOSetDirection(&ledPin);
    KeusGPIOSetDirection(&ledPin2);
    KeusGPIOSetPinValue(&ledPin);
    KeusGPIOSetPinValue(&ledPin2);
}

void parseUart(void)
{
    int i = 0;
    commandId = g0_u8RecData[0];

    for (i = 0; i < MAX_UART_DATA_BUFFER; i++)
    {
        uartDataBuffer[i] = g0_u8RecData[i + 1];
    }

    taskHandler();
}

//Calls the required function as needed by UART received message
void taskHandler(void)
{
    switch (commandId)
    {
    case TASK_MAP_SWITCH:
        mapSwitch();
        break;
    case TASK_ENTER_DEBUG:
        enterDebugMode();
        break;
    case TASK_EXIT_DEBUG:
        exitDebugMode();
        break;

    default:
        break;
    }
}

void mapSwitch(void)
{
    //Example msg: 28 06 01 | 02 01 54 02 64 | 29
    int noOfSwitch = uartDataBuffer[0];
    uint8 position = 0, switchId, sceneID;

    for (int i = 0; i < noOfSwitch; i++)
    {
        switchId = uartDataBuffer[position + 1];
        sceneID = uartDataBuffer[position + 2];
        position += 2;
        switch (switchId)
        {
        case BUTTON1_ID:
            scene_arr[0] = sceneID;
            break;
        case BUTTON2_ID:
            scene_arr[1] = sceneID;
            break;
        case BUTTON3_ID:
            scene_arr[2] = sceneID;
            break;
        case BUTTON4_ID:
            scene_arr[3] = sceneID;
            break;

        default:
            break;
        }
    }
    uint8 sendReply[1] = {KEUS_UART_MSG_ACK};
    UART_tx(sendReply, 1);
    //KeusUartAckMsg(KEUS_UART_MSG_ACK);
}

void buttonPressAction(uint8 buttonId)
{
    uint8 sendReply[2] = {EXECUTE_SCENE_COMMAND, 0};
    if (debugModeActive)
    {
        switch (buttonId)
        {
        case BUTTON1_ID:
            keusAppEvents |= KEUS_APP_EVT1;
            break;

        case BUTTON2_ID:
            keusAppEvents |= KEUS_APP_EVT2;
            break;

        case BUTTON3_ID:
            keusAppEvents |= KEUS_APP_EVT3;
            break;

        case BUTTON4_ID:
            keusAppEvents |= KEUS_APP_EVT4;
            break;
        default:
            break;
        }
    }
    else
    {
        switch (buttonId)
        {
        case BUTTON1_ID:
            if (debugSequence & KEUS_BUTTON4)
            {
                KeusTimerUtilRemoveTimer(&debugSequenceTimer);
                KeusTimerUtilAddTimer(&debugSequenceTimer);
                debugSequence |= KEUS_BUTTON1;
            }
            else
            {
                sendReply[1] = scene_arr[0];
                UART_tx(sendReply, 2);
            }
            break;

        case BUTTON2_ID:
            if (debugSequence == 15)
            {
                enterDebugMode();
            }
            else
            {
                sendReply[1] = scene_arr[1];
                UART_tx(sendReply, 2);
                KeusTimerUtilAddTimer(&debugSequenceTimer);
                debugSequence |= KEUS_BUTTON2;
            }
            break;

        case BUTTON3_ID:
            if (debugSequence & KEUS_BUTTON2)
            {
                KeusTimerUtilRemoveTimer(&debugSequenceTimer);
                KeusTimerUtilAddTimer(&debugSequenceTimer);
                debugSequence |= KEUS_BUTTON3;
            }
            else
            {
                sendReply[1] = scene_arr[2];
                UART_tx(sendReply, 2);
            }
            break;

        case BUTTON4_ID:
            if (debugSequence & KEUS_BUTTON3)
            {
                KeusTimerUtilRemoveTimer(&debugSequenceTimer);
                KeusTimerUtilAddTimer(&debugSequenceTimer);
                debugSequence |= KEUS_BUTTON4;
            }
            else
            {
                sendReply[1] = scene_arr[3];
                UART_tx(sendReply, 2);
            }
            break;
        default:
            break;
        }
        // blinkLED();
    }
    blinkLED();
}