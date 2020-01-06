#ifndef KEUSACTIONS_H_INCLUDED
#define KEUSACTIONS_H_INCLUDED

#define MAX_UART_DATA_BUFFER 12
#define TASK_MAP_SWITCH 1
#define MAX_NO_OF_BUTTON 4

#define BUTTON1_ID 1
#define BUTTON2_ID 2
#define BUTTON3_ID 3
#define BUTTON4_ID 4

#define KEUS_UART_MSG_ACK 0x05

uint8 commandId, uartDataBuffer[MAX_UART_DATA_BUFFER];

void parseUart(void);
void taskHandler(void);
void mapSwitch(void);
uint8 scene_arr[MAX_NO_OF_BUTTON];

extern void KeusUartAckMsg(uint8 data);
#define KEUS_UART_BUFFER 32
extern uint8 g0_u8RecData[KEUS_UART_BUFFER];


#endif