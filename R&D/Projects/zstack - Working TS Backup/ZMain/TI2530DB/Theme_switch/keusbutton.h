#ifndef KEUSBUTTON_H_INCLUDED
#define KEUSBUTTON_H_INCLUDED



//Board LED works on Negative Logic
#define LED_ON 0x00
#define LED_OFF 0x01

//Event bit masks
#define KEUS_BUTTON1 0x1
#define KEUS_BUTTON2 0x2
#define KEUS_BUTTON3 0x4
#define KEUS_BUTTON4 0x8

#define BUTTON1_ID 1
#define BUTTON2_ID 2
#define BUTTON3_ID 3
#define BUTTON4_ID 4





void keusButtonInit(void);
extern volatile uint32 keusAppEvents;
void debounceTimerCbk(uint8 timerId);


HAL_ISR_FUNCTION(halKeusPort1Isr, P1INT_VECTOR);

#endif