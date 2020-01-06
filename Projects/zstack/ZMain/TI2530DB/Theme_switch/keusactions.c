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
#include "keusactions.h"

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

    KeusUartAckMsg(KEUS_UART_MSG_ACK);
}