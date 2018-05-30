#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "adc.h"
#include "adc.c"
#include "scheduler.h"
#include "timer.h"


// global
unsigned long x, y = 0;

enum joy_States {start_joy, Init_joy, ReadX, ReadY};

int JoyStick(int state){
	unsigned long temp = 50;
	unsigned char tempb = 0;

	
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
			if (x >= 3*temp && x < 6*temp)
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

		break;
			
		
	}
	PORTB = tempb;
	return state;
}

int main()
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	ADC_Init(5, 1, 1);

// Period for the tasks
unsigned long int joy_calc = 200;
//unsigned long int Player_calc = 100;

//Calculating GCD
unsigned long int tmpGCD = 1;
tmpGCD = joy_calc;
//tmpGCD = findGCD(Player_calc, LCD_calc);
//only 2 tasks

//Greatest common divisor for all tasks or smallest time unit for tasks.
unsigned long int GCD = tmpGCD;


//Recalculate GCD periods for scheduler
unsigned long int LCD_Period = joy_calc/GCD;
//unsigned long int Player_Period = Player_calc/GCD;

//Declare an array of tasks
static task task1; //, task4;          only 3 task
task *tasks[] = {&task1} ; //, &task4	 ..
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task 1
task1.state = -1;//Task initial state.
task1.period = LCD_Period;//Task Period.
task1.elapsedTime = LCD_Period;//Task current elapsed time.
task1.TickFct = &JoyStick;//Function pointer for the tick.

// Task 2
// task2.state = -1;//Task initial state.
// task2.period = Player_Period;//Task Period.
// task2.elapsedTime = Player_Period;//Task current elapsed time.
// task2.TickFct = &Player;//Function pointer for the tick.

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