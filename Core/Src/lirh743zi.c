#include "lirh743zi.h"

void lirPulse(TIM_HandleTypeDef *htim, GPIO_TypeDef* GPIO_Data, uint16_t GPIO_Data_Pin, GPIO_TypeDef* GPIO_Clck, uint16_t GPIO_Clck_Pin){
	if (PULSE_COUNT<MAX_PULSE_COUNT){
//		генерация клоков
		 GPIO_Clck->ODR ^= (0x1UL << (GPIO_Clck_Pin)); // PG2(CLCK)
		 PULSE_COUNT++;
//		 чтение бита в data по каждому нечетному клоку,пропуская первый
		 if ((PULSE_COUNT&0x1UL)&(PULSE_COUNT>2UL)){
		 		  data<<=0x1UL;
		 		  data |= (GPIO_Data->IDR >> GPIO_Data_Pin)&0x01; //read bit pG3
		}
	}
//			завершили чтение всех битов
			else {
//				остановка таймера
				htim->Instance->CR1 &= ~(TIM_CR1_CEN);
				htim->Instance->CNT = 0;
//				взять таймстамп в виде циклов процессора от момента предыдущего замера
				tbuf = DWT->CYCCNT;
				DWT->CYCCNT = 0UL;
//				берем 14 бит данных и кладем их в буфер отправки
				tosend[0]= data&0x3FFFUL;
//				кладем в буфер номер показания для проверки целостности в свободные 16 бит рядом с данными.
				tosend[0]<<=16;
				tosend[0]|= ++count;
//				таймстамп в микросекундах от начала работы программы
				timestamp += DWTtoUS(tbuf);
				tosend[1]= timestamp; //переменной этой хватит на час с чем-то, если хранить микросекунды
//				отправка
				CDC_Transmit_FS((uint8_t*)tosend, 12);
//				обнуление
				data = 0;
				PULSE_COUNT = 0;
			}
}

void lirCycle(TIM_HandleTypeDef *htim, uint32_t IRQN){
	htim->Instance->CR1 |= TIM_CR1_CEN;
	htim->Instance->DIER|= IRQN;
}

void lirInit(){
	PULSE_COUNT = 0;
	MAX_PULSE_COUNT = (BITNESS + 1) * 2;
	data = 0;
	tbuf = 0;
	timestamp = 0;
	count = 0;
//	стоп слово для определения конца посылки
	tosend[2]=0xFAFBFCFD;
}

void DWTinit(){
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
//	DWT->LAR = 0xC5ACCE55; //софтверный анлок??
	DWT->CYCCNT = 0U;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

