#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>



#if 1
unsigned long g_counter = 0; ////int datatype is used with g_counter since I am keeping track of seconds and there are 86400 in 1 day
unsigned char sec1 =0;
unsigned char sec2 =0;
unsigned char min1 =0;
unsigned char min2 =0;
unsigned char hr1 =0;
unsigned char hr2 =0;
unsigned char day = 0;

unsigned char g_sec1_interrupt_flag = 0;
unsigned char g_sec2_interrupt_flag = 0;
unsigned char g_min1_interrupt_flag = 0;
unsigned char g_min2_interrupt_flag = 0;
unsigned char g_hr1_interrupt_flag = 0;
unsigned char g_hr2_interrupt_flag = 0;

#endif


//for testing use this link  to perform the following https://www.omnicalculator.com/conversion/minutes-to-hours-converter
//1) calculate the equivalent of the time needed in seconds(use link above) and assign it to g_counter
//2) set the equivalent time to the corresponding variables: hr2 hr1   min2 min1  sec2 sec1

#if 0
//#define g_counter1  540;

unsigned long g_counter = 0;
unsigned char sec1 = 0;
unsigned char sec2 = 0;
unsigned char min1 = 0;
unsigned char min2 = 0;
unsigned char hr1 = 0;
unsigned char hr2 = 0;
unsigned char day = 0;

unsigned char g_sec1_interrupt_flag = 0;
unsigned char g_sec2_interrupt_flag = 0;
unsigned char g_min1_interrupt_flag = 0;
unsigned char g_min2_interrupt_flag = 0;
unsigned char g_hr1_interrupt_flag = 0;
unsigned char g_hr2_interrupt_flag = 0;

#endif



void Timer_Init(void);
void StopWatch_Updater(void);
void StopWatch_Display(void);

//-------------------------------------------------MAIN---------------------------------------------------

int main(void)
{
	//initialize port a pins 0-->5 as output for the enable/disable pins of the 7 segment
	DDRA |= 0x3f;


	//Initialize portc pins 0 --> 3 as output for displaying numbers
	DDRC |= 0x0f;

	//Configure port b pin 2 as input + activate internal resistance
	DDRB &= ~(1<<PB2); // direction is set
	PORTB |= (1<<PB2); // internal pull up activated

	MCUCSR &= ~(1<<ISC2);
	GICR |= (1<<INT2);


	//Configure port d as input
	DDRD &= ~(1<<PD3) & ~(1<<PD2);

	// PD3 configuration
	MCUCR |= (1<<ISC11) | (1<< ISC10);
	GICR |= (1<<INT1);
//	SREG |= (1<<7);

	//PD2 configuration
	MCUCR |= ISC01;
	GICR |= (1<<INT0);


	//Configure Timer
	Timer_Init();

	while(1)
	{
		StopWatch_Updater();


		//enable led
		//set value that this led will hold
		//add very small delay(in microsecs) to be able to see the value before disabling the led
		//disable led
		StopWatch_Display();
	}
}

//------------------------------------------END OF MAIN---------------------------------------------------


//--------------------------------------------Functions---------------------------------------------------

//Timer Initialization
void Timer_Init(void)
{
	TCNT1 = 0;

	OCR1A = 976;

	TIMSK |= (1<<OCIE1A);

	TCCR1A = 0x00;
	TCCR1B = 0x0D;

	SREG|= (1<<7);
}

void StopWatch_Updater(void)
{
	if(g_sec1_interrupt_flag)
	{
		sec1++;
		g_sec1_interrupt_flag = 0;

		if(sec1 > 9)
		{
			sec1 = 0;
		}
	}

	if( g_sec2_interrupt_flag )
	{
		sec2++;
		g_sec2_interrupt_flag = 0;

		// max value is 6 which is turned directly to +1 min1
		if(sec2 > 5)
		{
			sec2 = 0;
		}
	}

	if( g_min1_interrupt_flag )
	{
		min1++;
		g_min1_interrupt_flag= 0;

		// max value is 9
		if( min1> 9)
		{
			min1= 0;
		}
	}

	if( g_min2_interrupt_flag )
	{
		min2++;
		g_min2_interrupt_flag= 0;

		// max value is 6 which is turned directly to +1 hr
		if( min2> 5)
		{
			min2= 0;
		}
	}

	if( g_hr1_interrupt_flag )
	{
		hr1++;
		g_hr1_interrupt_flag= 0;

		/*
		 -->max value is either 9: in case of 00hr to 09hr / 10hr to 19hr
		 -->or 4: in case 20hr to 24hr(which is turned directly to 00:00:00:)
		 */
		if( hr1> 9 || (hr2 ==2 && hr1 >3 ))
		{
			if(hr2 ==2 && hr1 >3)
			{
				day =1;
				hr2 =0;
			}
			hr1= 0;
		}
	}

	if( g_hr2_interrupt_flag )
	{
		hr2++;
		g_hr2_interrupt_flag= 0;


		if( (hr2 == 2))
		{
			hr2= 0;
		}
	}

}

void StopWatch_Display(void)
{
	PORTA |= 0x01;
	PORTC = (PORTC & 0xf0) | sec1;
	_delay_us(20);
	PORTA &= 0xfE; // 1111 1110



	PORTA |= 0x02;
	PORTC = (PORTC & 0xf0) | sec2;
	_delay_us(20);
	PORTA &= 0xfD; // 1111 1101

	PORTA |= 0x04;
	PORTC = (PORTC & 0xf0) | min1 ;
	_delay_us(20);
	PORTA &= 0xFB; // 1111 1011

	PORTA |= 0x08;
	PORTC = (PORTC & 0xf0) | min2;
	_delay_us(20);
	PORTA &= 0xF7; // 1111 0111

	PORTA |= 0x10;
	PORTC = (PORTC & 0xf0) | hr1;
	_delay_us(20);
	PORTA &= 0xEF; // 1110 1111

	PORTA |= 0x20;
	PORTC = (PORTC & 0xf0) | hr2;
	_delay_us(20);
	PORTA &= 0xDF; // 1101 1111


}
//--------------------------------------------END OF Functions--------------------------------------------


//--------------------------------------------------ISR---------------------------------------------------

//ISR for Timer Compare Match
ISR(TIMER1_COMPA_vect)
{
	/*Logic used:
	--> Seconds(sec1) are updated each interrupt
	--> g_counter is the incremented value of seconds with each interrupt
	--> g_counter(which is the actual number of seconds elapsed) determine the flag of each (sec2, min1,...,hr2)
	--> according to which flag is raised, the display value is adjusted in the main()
	*/

	//{_ _  _ _  _(_)}   sec1 increments every 1 seconds
	g_sec1_interrupt_flag =1;
	g_counter ++;

	//{_ _  _ _  (_)_}   sec2 increments every 10 seconds
	if(g_counter % 10 ==0)
	{
		g_sec2_interrupt_flag =1;
	}

	//{_ _  _(_)  _ _}   min1 increments every 60 seconds
	if(g_counter % 60 == 0 )
	{
		g_min1_interrupt_flag =1;
	}

	//{_ _  (_)_  _ _}   min2 increments every 600 seconds
	if(g_counter % 600 == 0 )
	{
		g_min2_interrupt_flag=1;
	}

	//{_(_)  _ _  _ _}   hr1 increments every 3600 seconds
	if(g_counter % 3600 == 0 )
	{
		g_hr1_interrupt_flag =1;
	}

	//{(_)_  _ _  _ _}   hr2 increments every 3600 seconds
	if(g_counter % 36000 == 0 )
	{
		g_hr2_interrupt_flag=1;
	}
}

//ISR for resuming time count(rising edge)
ISR(INT1_vect)
{
	if(TCCR1B == 0x0D)
	TCCR1B = 0x08;
}

//ISR for stopping time count(falling edge)
ISR(INT2_vect)
{
	if( TCCR1B == 0x08)
	{
	TCCR1B = 0x0D;
	}
}


//ISR for reset button(falling edge)
ISR(INT0_vect)
{
	//reset the timer initial value to 0
	TCNT1 = 0;

	//reset g_counter and all the values(sec1, sec2,...etc) to 0
	sec1 =0;
	sec2 =0;
	min1 =0;
	min2 =0;
	hr1 =0;
	hr2 =0;
	day = 0;

	g_counter = 0;

}


//--------------------------------------------------END OF ISR--------------------------------------------
