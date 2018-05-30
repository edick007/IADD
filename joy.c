#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "adc.h"
#include "adc.c"
// void ADC_Init()
// {
// 	DDRA=0x0;			/* Make ADC port as input */
// 	ADCSRA = 0x87;			/* Enable ADC, fr/128  */
// 	ADMUX = 0x40;			/* Vref: Avcc, ADC channel: 0 */
//
// }
//
// int ADC_Read(char channel)
// {
// 	int Ain,AinLow;
//
// 	ADMUX=ADMUX|(channel & 0x0f);	/* Set input channel to read */
//
// 	ADCSRA |= (1<<ADSC);		/* Start conversion */
// 	while((ADCSRA&(1<<ADIF))==0);	/* Monitor end of conversion interrupt */
//
// 	_delay_us(10);
// 	AinLow = (int)ADCL;		/* Read lower byte*/
// 	Ain = (int)ADCH*256;		/* Read higher 2 bits and
// 					Multiply with weight */
// 	Ain = Ain + AinLow;
// 	return(Ain);			/* Return digital value*/
// }



int main()
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	int x, y = 0;

	ADC_Init(5, 1, 1);

	while(1)
	{
		
		unsigned char tempb = 0;
		y = ADC_ReadData(6);
		x = ADC_ReadData(5);	/* Read ADC channel 0 */
		
		unsigned long temp = 50;
		if (y == ADC_ReadData(6)){

			if (y >=temp && y < 2*temp);
			{
				tempb = 0x10;
			}
			if (y >= 2*temp && y < 3*temp)
			{
				tempb += 0x20;
			}
			if (y >= 3*temp && y < 4*temp)
			{
				tempb += 0x30;
			}
			if (y >= 4*temp && y < 5*temp)
			{
				tempb += 0x40;
			}
			if(y>= 5*temp && y < 5 * temp)
			{
				tempb = 0x50;
			}
			if (x >= 6*temp && x < 7*temp)
			{
				tempb += 0x60;
			}
			if (x >= 7*temp && x < 8*temp)
			{
				tempb += 0x70;
			}
			if (x >= 8*temp && x < 9*temp)
			{
				tempb += 0x80;
			}
			if (x >= 9*temp && x < 10*temp)
			{
				tempb += 0x90;
			}
			if (x >= 10*temp && x < 11*temp)
			{
				tempb += 0xA0;
			}
			if (x >= 11*temp && x < 12*temp)
			{
				tempb += 0xB0;
			}
			if (x >= 12*temp && x < 13*temp)
			{
				tempb += 0xC0;
			}
			
			
		}
		// 	if (x == ADC_ReadData(5)){
		// 		if (x >=temp && x < 2*temp);
		// 		{
		// 			tempb = 1;
		// 		}
		// 		if (x >= 2*temp && x < 3*temp)
		// 		{
		// 			tempb = 2;
		// 		}
		// 		if (x >= 3*temp && x < 4*temp)
		// 		{
		// 			tempb = 3;
		// 		}
		// 		if (x >= 4*temp && x < 5*temp)
		// 		{
		// 			tempb = 4;
		// 		}
		// 		if (x >= 5*temp && x < 6*temp)
		// 		{
		// 			tempb = 5;
		// 		}
		// 		if (x >= 6*temp && x < 7*temp)
		// 		{
		// 			tempb = 6;
		// 		}
		// 		if (x >= 7*temp && x < 8*temp)
		// 		{
		// 			tempb = 7;
		// 		}
		// 		if (x >= 8*temp && x < 9*temp)
		// 		{
		// 			tempb = 8;
		// 		}
		// 		if (x >= 9*temp && x < 10*temp)
		// 		{
		// 			tempb = 9;
		// 		}
		// 		if (x >= 10*temp && x < 11*temp)
		// 		{
		// 			tempb = 10;
		// 		}
		// 		if (x >= 11*temp && x < 12*temp)
		// 		{
		// 			tempb = 11;
		// 		}
		// 		if (x >= 12*temp && x < 13*temp)
		// 		{
		// 			tempb = 12;
		// 		}
		// // 		if (x >= 13*temp && x < 14*temp)
		// // 		{
		// // 			tempb = 13;
		// // 		}
		// // 		if (x >= 14*temp && x < 15*temp)
		// // 		{
		// // 			tempb = 14;
		// // 		}
		// // 		if (x >= 15*temp && x < 16*temp)
		// // 		{
		// // 			tempb = 15;
		// // 		}
		// // 		if (x >= 16*temp && x < 17*temp)
		// // 		{
		// // 			tempb = 16;
		// // 		}
		// // 		if (x >= 17*temp && x < 18*temp)
		// // 		{
		// // 			tempb = 17;
		// // 		}
		// // 		if (x >= 18*temp && x < 19*temp)
		// // 		{
		// // 			tempb = 18;
		// // 		}
		// // 		if (x >= 19*temp  && x < 20*temp)
		// // 		{
		// // 			tempb = 19;
		// // 		}
		// // 		else
		// // 		{
		// // 			tempb = 20;
		// // 		}
		//
		// 	}
		
		
		
		PORTB = tempb;
	}
	return 0;
}
