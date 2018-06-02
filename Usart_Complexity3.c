
/*
Edawrd Dickhoff 
SID: 861167330
Date: 5/22/2018

Status: IP

Blocker: NONE 

Description:
Tester for uart, a simple SM that generates and sends data accross the TX 
line of a 1284 in a USART way.



*/
//Avr included files
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
//UCR included files
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include "usart.h"			//needed for this



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

		

//--------End Shared Variables------------------------------------------------

//--------start generate data function------------------------------------------------
//simple function that make a data of nums 1 - 200
//vars
char data_counter = 0;
char Generate_Data(){

	if (data_counter < 200)
	{
		data_counter++;
	}
	else
	{
		data_counter = 0;
	}
	return data_counter;
}
//--------End generate data-----------------------------------------------
//

//----------IR Sensor FSM Begin------------------------------------------------

enum PlayerStates {start_Usart, Get_data, Wait_Ready, Send_Data, wait_Sent};
unsigned char Tx_Status;
unsigned char Rx_Status;
unsigned char data;
int IR(int state){
	
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
		PORTB = 0x01;
		break;
		
		case Get_data:
		data = Generate_Data();		// calls a function that puts in data
		PORTB = 0x02;
		break;
		
		case Wait_Ready:	//tests if its ready
		PORTB = 0x04;
		break;
		
		case Send_Data:		//send data
		PORTB = 0x08;
		USART_Send(data, 0);
		break;
		
		case wait_Sent:
		PORTB = 0x10;
		break;
	}
	
	
	
	
	return state;
}
//----------Player scroll FSM END------------------------------------------------
int main()
{
	DDRD = 0xFF; PORTD = 0x00;
	DDRA = 0x00; PORTA = 0xFF; // IR input
	DDRB = 0xFF; PORTB = 0x00; // output for system signal


	// Initializes the LCD display
	LCD_init();
	initUSART(0);
	
	// Period for the tasks
	unsigned long int Usart_calc = 20;
	
	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = Usart_calc;
	//tmpGCD = findGCD(Player_calc, LCD_calc);			
		//only 1 tasks
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;


	//Recalculate GCD periods for scheduler
	unsigned long int Usart_Period = Usart_calc/GCD;

	
	//Declare an array of tasks
	static task task1; //, task4;          only 3 task
	task *tasks[] = {&task1} ; //, &task4	 ..
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	

	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = Usart_Period;//Task Period.
	task1.elapsedTime = Usart_Period;//Task current elapsed time.
	task1.TickFct = &IR;//Function pointer for the tick.
	
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


