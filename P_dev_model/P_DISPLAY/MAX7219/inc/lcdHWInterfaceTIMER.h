/********************************************************************************
 * @file    lcdHWInterfaceTIMER.h
 * @author  Gerasimchuk A.
 * @version V1.0.0
 * @date    26-December-2017
 * @brief
 */
#ifndef LCD_HW_INTERFACE_TIMER_H_
#define LCD_HW_INTERFACE_TIMER_H_

#ifdef USE_QT

#define printD(...)     printf(__VA_ARGS__)
#define dataSetPin()    printf("-")
#define dataClearPin()  printf("_")
#define clockSetPin()   printf("1")
#define clockClearPin() printf("0")
#define stopTimer()     printf("S\n")
#define startGenLD_()   printf("startLD")
#define stopGenLD_()    printf("stopLD\n")

#else

#define printD(...)
#define dataSetPin_()
#define dataClearPin_()
#define clockSetPin_()
#define clockClearPin_()
#define stopTimer_()
#define startGenLD_()
#define stopGenLD_()

#endif

#define BIT_SELECTOR_MASK 0b1000000000000000

#define NUMBER_OF_BIT    16

#define CLOCK_TIMER       TIM2

#define CLOCK_FRQ         3000000

// ----------SPI1 pin/port definition---------
#define GPIO_CLOCK_PORT    GPIOB
#define GPIO_CLOCK_PIN     GPIO_Pin_13
#define GPIO_DATA_PORT     GPIOB
#define GPIO_DATA_PIN      GPIO_Pin_15


// ----------LD pin/port definition---------
#define PORT_LD_1  GPIOB
#define PORT_LD_2  GPIOB
#define PORT_LD_3  GPIOB
#define PORT_LD_4  GPIOB

#define PIN_LD_1   GPIO_Pin_12
#define PIN_LD_2   GPIO_Pin_8
#define PIN_LD_3   GPIO_Pin_14
#define PIN_LD_4   GPIO_Pin_9



typedef struct{
	GPIO_TypeDef* port;
	uint16_t     pin;
}LDDescr;





typedef enum {
    CLOCK_STATE_HIGHT = 0,
    CLOCK_STATE_LOW
}CLOCK_STATE;

typedef enum {
    LD_STATE_SET = 0,
    LD_STATE_CLEAR
}LD_STATE;

typedef enum {
    TX_STATE_BIT = 0,
    TX_STATE_LD,
    TX_STATE_COMPLITE
}TX_STATE;

//void transmitCallback(void);

#endif

