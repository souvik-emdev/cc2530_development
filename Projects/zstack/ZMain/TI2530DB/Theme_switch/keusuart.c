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
#include "keusuart.h"

void initUart()
{
  halUARTCfg_t uartConfig;
  uartConfig.configured = TRUE;
  uartConfig.baudRate = HAL_UART_BR_115200;
  uartConfig.flowControl = FALSE;
  uartConfig.flowControlThreshold = 48;
  uartConfig.rx.maxBufSize = 128;
  uartConfig.tx.maxBufSize = 128;
  uartConfig.idleTimeout = 6;
  uartConfig.intEnable = TRUE;
  uartConfig.callBackFunc = uartRxCb;
  HalUARTOpen(HAL_UART_PORT_0, &uartConfig);
}


void uartRxCb(uint8 port, uint8 event)
{ 
  //Example syntax: 28 03 01 03 01 29
  uint8 u8InChar;

  while (Hal_UART_RxBufLen(port))
  {
    // Read one byte from UART to ch
    HalUARTRead(port, &u8InChar, 1);

    // if (u8InChar == 's')
    // {
    //   ledPin.state = LED_ON;
    //   KeusGPIOSetPinValue(&ledPin);
    // }
    // else if (u8InChar == 'r')
    // {
    //   ledPin.state = LED_OFF;
    //   KeusGPIOSetPinValue(&ledPin);
    // }

    if (u8InChar == KEUS_UART_MSG_INITIATOR && g0_endMessageIndex == 0)
    {
      g0_shouldSaveToBuffer = 1;
      g0_bufferCounter = 0;
    }
    else if (g0_endMessageIndex == 0 && g0_shouldSaveToBuffer)
    {
      g0_endMessageIndex = u8InChar;
    }
    else if (u8InChar == KEUS_UART_MSG_TERMINATOR && g0_endMessageIndex == g0_bufferCounter)
    {
      g0_shouldSaveToBuffer = 0;
      g0_endMessageIndex = 0;

      //message_received = 1; //flag to process data
      keusAppEvents |= KEUS_UART;
    }
    else if (g0_bufferCounter > g0_endMessageIndex)
    {
      g0_shouldSaveToBuffer = 0;
      g0_endMessageIndex = 0;
    }

    else if (g0_shouldSaveToBuffer)
    {
      g0_u8RecData[g0_bufferCounter++] = u8InChar;
    }
    else
      g0_endMessageIndex = 0;
  }
}

void KeusUartAckMsg(uint8 data)
{
  uint8 ackMsgData[32];
  uint8 dataLen = 0;

  ackMsgData[dataLen++] = KEUS_UART_MSG_INITIATOR;

  ackMsgData[dataLen++] = 0x01; //one byte of ackdata

  ackMsgData[dataLen++] = data;

  ackMsgData[dataLen++] = KEUS_UART_MSG_TERMINATOR;

  //UART_Write(UART0, ackMsgData, dataLen);
  HalUARTWrite(HAL_UART_PORT_0, ackMsgData, dataLen);
}
