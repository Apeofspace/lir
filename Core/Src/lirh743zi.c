#include "lirh743zi.h"

void lirCycle(){
	if (PULSE_COUNT<MAX_PULSE_COUNT){
//		генерация клоков
		 GPIOG->ODR ^= GPIO_ODR_OD2; // PG2 (CLCK)
		 PULSE_COUNT++;
//		 чтение бита в data по каждому второму нечетному клоку
		 if ((PULSE_COUNT&0x1UL)&(PULSE_COUNT>2UL)){
		 		  data<<=0x1UL;
		 		  data |= (GPIOG->IDR >> GPIO_IDR_ID3_Pos)&0x01; //read bit pg3
		}
	}
//			завершили чтение всех битов
			else {
//				остановка таймера
				TIM1->CR1 &= ~(TIM_CR1_CEN);
				TIM1->CNT = 0;
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
	DWT->LAR = 0xC5ACCE55; //софтверный анлок??
	DWT->CYCCNT = 0U;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

