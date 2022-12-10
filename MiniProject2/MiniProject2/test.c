//#include<avr/io.h>
//#include<avr/interrupt.h>
//#include<util/delay.h>
//
//unsigned sec1 =0;
//unsigned sec2 =0;
//unsigned min1 =0;
//unsigned min2 =0;
//unsigned hr1 =0;
//unsigned hr2 =0;
//
//void Timer_Init(void);
//
//int main(void)
//{
//
//	DDRC |= 0x0f;
//	PORTC &= 0xfe;
////	PORTC &= 0xf0;
////	PORTC |= 0x01;
//
//	Timer_Init();
//
//	while(1)
//	{
//
//
//
//	}
//}
//
//
////Timer Initialization
//void Timer_Init(void)
//{
//	TCNT1 = 0; // initial timer value
//
//	OCR1A = 976;  // compare value
//
//	TIMSK |= (1<<OCIE1A);  // enabling interrupt on compare match with OCRA1
//
//	TCCR1A = 0x00;
//	TCCR1B = 0x0D;
//
//	SREG|= (1<<7); // ibit enable
//}
//
//
//ISR(TIMER1_COMPA_vect)
//{
//	PORTC ^= (1<<PC0);
//}
