/*
Edawrd Dickhoff 
SID: 861167330
Date: 5/22/2018

Status: IP

Blocker: NONE 

Description:
Part one complexity for IADD



*/
//Avr included files
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
//UCR included files
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include "io.c"	//included from added directory :D
#include "io.h"

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

		char POS = -1;	//player position
		unsigned char System_Flag = 0;		//is the system on or off

//--------End Shared Variables------------------------------------------------



//----------IR Sensor FSM Begin------------------------------------------------

enum PlayerStates {start_IR, System_Off, System_On, wait_IR, wait_Off_IR};

int IR(int state){
	
	unsigned char IR_A0 = (~PINA & 0x01);
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
			System_Flag = 1;	//system is on
		break;
		
		case wait_IR:
			System_Flag = 1;
		break;
		
		case wait_Off_IR:
			System_Flag = 0;
		break;
	}
	PORTB = System_Flag;
	
	return state;
}
//----------Player scroll FSM END------------------------------------------------
int main()
{
	
	DDRA = 0x00; PORTA = 0xFF; // IR input
	DDRB = 0xFF; PORTB = 0x00; // output for system signal


	// Initializes the LCD display
	LCD_init();
	
	
	// Period for the tasks
	unsigned long int IR_calc = 500;
	
	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = IR_calc;
	//tmpGCD = findGCD(Player_calc, LCD_calc);			
		//only 1 tasks
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;


	//Recalculate GCD periods for scheduler
	unsigned long int IR_Period = IR_calc/GCD;

	
	//Declare an array of tasks
	static task task1; //, task4;          only 3 task
	task *tasks[] = {&task1} ; //, &task4	 ..
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	

	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = IR_Period;//Task Period.
	task1.elapsedTime = IR_Period;//Task current elapsed time.
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

