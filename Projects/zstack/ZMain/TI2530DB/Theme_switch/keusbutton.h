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

//GPIO Info
KeusGPIOPin buttonPin = {1, 2, GPIO_INPUT, true, LED_ON};
KeusGPIOPin buttonPin2 = {1, 3, GPIO_INPUT, true, LED_ON};
KeusGPIOPin buttonPin3 = {1, 4, GPIO_INPUT, true, LED_ON};
KeusGPIOPin buttonPin4 = {1, 5, GPIO_INPUT, true, LED_ON};

volatile bool debounce_read = true; //only read switchPress if this set

void keusButtonInit(void);
extern volatile uint32 keusAppEvents;
void debounceTimerCbk(uint8 timerId);

KeusTimerConfig debounceTimer = {
    &debounceTimerCbk,
    200,
    true,
    -1,
    0};

HAL_ISR_FUNCTION(halKeusPort1Isr, P1INT_VECTOR);

#endif