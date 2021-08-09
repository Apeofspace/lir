#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
//битность датчика
#define BITNESS 14
//пересчет циклов процессора в микросекунды
#define DWTtoUS(dwt) ((float) dwt *  1000000U/SystemCoreClock)

volatile uint8_t PULSE_COUNT;
uint8_t MAX_PULSE_COUNT;
volatile uint32_t data, tbuf, timestamp;
volatile uint32_t tosend[3];
volatile uint16_t count;

void lirInit();
void DWTinit();
void lirCycle(TIM_HandleTypeDef *htim, uint32_t IRQN);
void lirPulse(TIM_HandleTypeDef *htim, GPIO_TypeDef* GPIO_Data, uint16_t GPIO_Data_Pin, GPIO_TypeDef* GPIO_Clck, uint16_t GPIO_Clck_Pin);
