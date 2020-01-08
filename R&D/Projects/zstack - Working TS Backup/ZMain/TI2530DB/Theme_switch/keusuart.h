#ifndef KEUSUART_H_INCLUDED
#define KEUSUART_H_INCLUDED

extern uint8 KEUS_taskID;

#define KEUS_UART_MSG_INITIATOR 0x28
#define KEUS_UART_MSG_TERMINATOR 0x29

//Event bit masks
#define KEUS_BUTTON1 0x1
#define KEUS_BUTTON2 0x2
#define KEUS_BUTTON3 0x4
#define KEUS_BUTTON4 0x8
#define KEUS_UART 0x10

//Communication variables
#define KEUS_UART_BUFFER 32
uint8 g0_u8RecData[KEUS_UART_BUFFER] = {0};
uint8 g0_bufferCounter = 0;
uint8 g0_shouldSaveToBuffer = 0;
uint8 g0_endMessageIndex = 0;
uint8 message_received = 0;

extern volatile uint32 keusAppEvents;

void initUart(void);
void uartRxCb(uint8 port, uint8 event);
void UART_tx(uint8 arr[], uint8 dataLen);

#endif