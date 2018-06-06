
/*
Edawrd Dickhoff 
SID: 861167330
Date: 5/22/2018

Status: IP

Blocker: NONE 

Description:
Combining the Usart with the Joy stick to send data to the audrino to navigate the LED matrix



*/
#define F_CPU 8000000UL
//Avr included files
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>
//UCR included files
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include "LCD_16x2_C_file.c"	//included from added directory :D
#include "LCD_16x2_H_file.h"

#include "adc.h"
#include "adc.c"
#include "usart.h"

#define  Trigger_pin	PA0/* Trigger pin */
int TimerOverflow = 0;


ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;		/* Increment Timer Overflow count */
}



//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;	
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}
//--------End find GCD function ----------------------------------------------

//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

//--------End Task scheduler data structure-----------------------------------

//--------Shared Variables----------------------------------------------------
		unsigned char Global_Direction = 0;
		char POS = -1;	//player position
		unsigned char System_Flag = 0;		//is the system on or off

//--------End Shared Variables------------------------------------------------

//--------start generate data function------------------------------------------------
//simple function that make a data of nums 1 - 100
//vars
// char data_counter = 0;
// char Generate_Data(){
// 
// 	if (data_counter < 200)
// 	{
// 		data_counter++;
// 	}
// 	else
// 	{
// 		data_counter = 0;
// 	}
// 	return data_counter;
// }


//--------End generate data-----------------------------------------------


//----------Usart Sensor FSM Begin------------------------------------------------

enum PlayerStates {start_Usart, Get_data, Wait_Ready, Send_Data, wait_Sent};
unsigned char Tx_Status;
unsigned char Rx_Status;
unsigned char data;
int UART(int state){
	
	switch (state)
	{
		case start_Usart:
		state = Get_data;
		break;
		
		case Get_data:
		state = Wait_Ready;
		break;
		
		
		case Wait_Ready:
		Rx_Status = USART_IsSendReady(0);	//checks if the TX register is clear to be sent
		if (Rx_Status == 1)
		{
			state = Send_Data;
		}
		else
		{
			USART_Flush(0);
			state = Send_Data;
		}
		break;
		
		case Send_Data:
		state = wait_Sent;		//send the data on the TX
		break;
		
		case wait_Sent:
		Tx_Status = USART_HasTransmitted(0);		// checks if the data was sent from the TX register
		if (Tx_Status == 1)
		{
			state = Get_data;	//after sent we need to put new data in
		}
		else
		{
			USART_Flush(0);
			state = Get_data;
		}
		break;
		
		default: state = start_Usart;
		
	}
	
	switch(state)
	{
		case start_Usart: 
		//PORTB = 0x01;
		break;
		
		case Get_data:
		data = Global_Direction;		// calls a function that puts in data
	//	PORTB = 0x02;
		break;
		
		case Wait_Ready:	//tests if its ready
		//PORTB = 0x04;
		break;
		
		case Send_Data:		//send data
		//PORTB = 0x08;
		USART_Send(data, 0);
		break;
		
		case wait_Sent:
		//PORTB = 0x10;
		break;
	}
	return state;
}
//----------usart scroll FSM END------------------------------------------------



// Begin ------------ Joystick FSM----------
// global

unsigned long x, y = 0;

enum joy_States {start_joy, Init_joy, ReadX, ReadY};
	unsigned char tempb = 0;
int JoyStick(int state){
	unsigned long temp = 50;


	
	switch(state){
		case start_joy: state = Init_joy;	break;
		case Init_joy:	state = ReadX;		break;
		case ReadX:		state = ReadY;		break;
		case ReadY:		state = ReadX;		break;
		default:		state = start_joy;	break;
	}
	
	
	switch(state){
		case start_joy: break;
		case Init_joy: PORTB = 0x00; break;
		case ReadX:
		x = ADC_ReadData(5);
		
		
		if (x >=temp && x < 2*temp);
		{
			tempb = 2;
		}
		if (x >= 2*temp && x < 3*temp)
		{
			tempb = 0;
		}
		if (x >= 3*temp + 10 && x < 6*temp)
		{
			tempb = 4;
		}

		break;
		
		case ReadY:
		y = ADC_ReadData(6);

		if (y < 30)
		{
			tempb += 0x01;
		}
		else  if (y >= temp && y < 80)
		{
			tempb += 0x00;
		}
		else if (y >= 4*temp && y < 6*temp)
		{
			tempb += 0x08;
		}
		Global_Direction = tempb;
		tempb = 0;
		break;
		
		
	}
	PORTB = Global_Direction;
	return state;
}

//End Joystick-------------------------------




//----------IR Sensor FSM Begin------------------------------------------------

enum IRstates {start_IR, System_Off, System_On, wait_IR, wait_Off_IR};

int IR(int state){
	
	unsigned char IR_A0 = (~PINA & 0x02);
	switch (state)
	{
		case start_IR: state = System_Off; break;
		
		case System_Off:
		if (IR_A0 == 1)
		{
			state = System_On;
			break;
		}
		else if (IR_A0 == 0x00)
		{
			state = System_Off;
			break;
		}
		else
		{
			state = System_Off;
			break;
		}
		break;
		
		case System_On:
		if (IR_A0 == 0x01)
		{
			state = System_On;
		}
		else if (IR_A0 == 0x00)
		{
			state = wait_IR;
		}
		break;
		
		case wait_IR:
		if (IR_A0 == 0)
		{
			state = wait_IR;
		}
		else if(IR_A0 == 1)
		{
			state = wait_Off_IR;
		}
		break;
		
		case wait_Off_IR:
		if (IR_A0 == 1)
		{
			state = wait_Off_IR;
		}
		else
		{
			state = System_Off;
		}
		
		default: state = start_IR; break;
	}
	
	switch(state){
		case start_IR: break;
		
		case System_Off:
		System_Flag = 0;	//system is off
		break;
		
		case System_On:
		System_Flag = 100;	//system is on
		break;
		
		case wait_IR:
		System_Flag = 100;
		break;
		
		case wait_Off_IR:
		System_Flag = 0;
		break;
	}
	PORTB = System_Flag;
	Global_Direction += System_Flag;
	
	return state;
}
//----------Player scroll FSM END------------------------------------------------


//----------Ultra sonic Start--------------------------------------
//globals
	char string[10];
	long count;
	double distance;
	unsigned char alarm = 0;
 enum Ultrastates {startUltra, Base_Ultra, measure, Send_Ultra};
	//enum unta {one, two};
int Sonic(int state){
// 	switch(state)
// 	{
// 		case one:
// 		state = two;
// 		break;
// 		case two:
// 			state = two;
// 		break;
// 		
// 		default: state = one; break;
// 	}
// 	switch(state)
// 	{
// 		case one: break;
// 		case two:
// 		PORTC = 0x08;
// 
// 			PORTA |= (1 << Trigger_pin);/* Give 10us trigger pulse on trig. pin to HC-SR04 */
// 			_delay_us(10);
// 			PORTA &= (~(1 << Trigger_pin));
// 			
// 			TCNT1 = 0;			/* Clear Timer counter */
// 			TCCR1B = 0x41;		/* Setting for capture rising edge, No pre-scaler*/
// 			TIFR1 = 1<<ICF1;		/* Clear ICP flag (Input Capture flag) */
// 			TIFR1 = 1<<TOV1;		/* Clear Timer Overflow flag */
// 
// 			/*Calculate width of Echo by Input Capture (ICP) on PortD PD6 */
// 			
// 			while ((TIFR1 & (1 << ICF1)) == 0);	/* Wait for rising edge */
// 		PORTC = 0x10;
// 			TCNT1 = 0;			/* Clear Timer counter */
// 			TCCR1B = 0x01;		/* Setting for capture falling edge, No pre-scaler */
// 			TIFR1 = 1<<ICF1;		/* Clear ICP flag (Input Capture flag) */
// 			TIFR1 = 1<<TOV1;		/* Clear Timer Overflow flag */
// 			TimerOverflow = 0;	/* Clear Timer overflow count */
// 
// 			while ((TIFR1 & (1 << ICF1)) == 0); /* Wait for falling edge */
// 		PORTC = 0x20;
// 			
// 			count = ICR1 + (65535 * TimerOverflow);	/* Take value of capture register */
// 			/* 8MHz Timer freq, sound speed =343 m/s, calculation mentioned in doc. */
// 			distance = (double)count / 466.47;
// 		PORTC = 0x40;
// 			dtostrf(distance, 2, 2, string);/* Convert distance into string */
// 			//strcat(string, " cm   ");
// 			LCD_String_xy(2, 0, "Shookness = ");
// 			LCD_String_xy(2, 12, string);	/* Print distance on LDC16x2 */
// 			_delay_ms(200);
// 		PORTC = 0x80;
// 		break;
// 	}
	switch(state){
		case startUltra: state = Base_Ultra;	break;
		
		case Base_Ultra: state = measure;		break;
		
		case measure:
			state = Send_Ultra;
		break;
		
		case Send_Ultra: 
			state = Base_Ultra;
		break;
		
		default: state = startUltra; break;
	}
	
	switch(state){
		case startUltra: break;
		
		case Base_Ultra: 
			PORTA |= (1 << Trigger_pin);/* Give 10us trigger pulse on trig. pin to HC-SR04 */
			_delay_us(10);
			PORTA &= (~(1 << Trigger_pin));
		
			TCNT1 = 0;			/* Clear Timer counter */
			TCCR1B = 0x41;		/* Setting for capture rising edge, No pre-scaler*/
			TIFR1 = 1<<ICF1;		/* Clear ICP flag (Input Capture flag) */
			TIFR1 = 1<<TOV1;		/* Clear Timer Overflow flag */
		
			while ((TIFR1 & (1 << ICF1)) == 0);	/* Wait for rising edge */
			TCNT1 = 0;			/* Clear Timer counter */
			TCCR1B = 0x01;		/* Setting for capture falling edge, No pre-scaler */
			TIFR1 = 1<<ICF1;		/* Clear ICP flag (Input Capture flag) */
			TIFR1 = 1<<TOV1;		/* Clear Timer Overflow flag */
			TimerOverflow = 0;	/* Clear Timer overflow count */

			while ((TIFR1 & (1 << ICF1)) == 0); /* Wait for falling edge */
		PORTC = 0x08;
		break;
		
		case measure:
			
			count = ICR1 + (65535 * TimerOverflow);	/* Take value of capture register */
			/* 8MHz Timer freq, sound speed =343 m/s, calculation mentioned in doc. */
			distance = (double)count / 466.47;
		break;
		
		case Send_Ultra: 
			count = ICR1 + (65535 * TimerOverflow);	/* Take value of capture register */
			/* 8MHz Timer freq, sound speed =343 m/s, calculation mentioned in doc. */
			distance = (double)count / 466.47;

			dtostrf(distance, 2, 2, string);/* Convert distance into string */
			//strcat(string, " cm   ");
			LCD_String_xy(2, 0, "Shookness = ");
			LCD_String_xy(2, 12, string);	/* Print distance on LDC16x2 */
			_delay_ms(200);
			
			if(distance < 30){
				if (Global_Direction >= 200)
				{
					Global_Direction += 0;
				}
				else{
					Global_Direction += 200;		//if a dood is too close alert the masses	
				}
				alarm = 1;
			}
			
			else
			{
				alarm = 0;	// for sounding alarm
			}
		PORTC = 0x80;
		break;
	}
	return state;
}
//---------------Ultra Sonic End-----------------------------

int main()
{
	DDRD = 0x0F; PORTD = 0xF0;  /*DDRD = 0x00; PORTD = 0xFF;*/
	DDRA = 0x01; PORTA = 0xFE; // IR input
	DDRB = 0xFF; PORTB = 0x00; // output for system signal
	DDRC = 0xFF; PORTC = 0x00; // output for sw, e, sr, and lights


	// Initializes the LCD display
	LCD_Init();
	LCD_String_xy(1, 0, "Personal Space");

	initUSART(0);
	ADC_Init(5, 1, 1);
	
	sei();					/* Enable global interrupt */
	TIMSK1 = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
	TCCR1A = 0;				/* Set all bit to zero Normal operation */
	
	// Period for the tasks
	unsigned long int Usart_calc = 20;
	unsigned long int Joy_calc = 10;
	unsigned long int IR_calc = 5;
	unsigned long int Ultra_calc = 5;	// needs to go through 5 states within one usart calc
	
	//Calculating GCD
	unsigned long int tmpGCD = 1;
	//tmpGCD = Usart_calc;
	tmpGCD = findGCD(Usart_calc, Joy_calc);	
	tmpGCD = findGCD(tmpGCD, IR_calc);		
	tmpGCD = findGCD(tmpGCD, Ultra_calc);
		//only 4 tasks
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;


	//Recalculate GCD periods for scheduler
	unsigned long int Usart_Period = Usart_calc/GCD;
	unsigned long int Joy_Period = Joy_calc/GCD;
	unsigned long int IR_Period = IR_calc/GCD;
	unsigned long int Ultra_Period = Ultra_calc/GCD;
	

	
	//Declare an array of tasks
	static task task1, task2, task3, task4; //, task4;          only 3 task
	task *tasks[] = {&task1, &task2, &task3, &task4} ; //, &task4	 ..
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	

	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = Usart_Period;//Task Period.
	task1.elapsedTime = Usart_Period;//Task current elapsed time.
	task1.TickFct = &UART;//Function pointer for the tick.
	
		// Task 2
		task2.state = -1;//Task initial state.
		task2.period = Joy_Period;//Task Period.
		task2.elapsedTime = Joy_Period;//Task current elapsed time.
		task2.TickFct = &JoyStick;//Function pointer for the tick.
		
				// Task 3
				task3.state = -1;//Task initial state.
				task3.period = IR_Period;//Task Period.
				task3.elapsedTime = IR_Period;//Task current elapsed time.
				task3.TickFct = &IR;//Function pointer for the tick.
				
								// Task 4
								task4.state = -1;//Task initial state.
								task4.period = Ultra_Period;//Task Period.
								task4.elapsedTime = Ultra_Period;//Task current elapsed time.
								task4.TickFct = &Sonic;//Function pointer for the tick.
	
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();

	
	unsigned short i; // Scheduler for-loop iterator	
	
	while(1) {
		
	
		
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime >= tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}

	// Error: Program should not exit!
	return 0;
	}


