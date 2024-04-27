/*
* EE4524_project2.c
*
* Created: 21/03/2019 11:30:31
* Author : Reece Gavin 17197589
*/


//***********************************************************************************************************************************************************************************************************************************//
//#Include Section
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdlib.h>



#define F_CPU 16000000UL  //CPU clock rate set at 16MHz
#define BAUD 115200  //BAUD Rate set at 115200
#define BAUD_TOL 3	 //Baud tolerance on 3%



//***********************************************************************************************************************************************************************************************************************************//

//VARIABLE INITALISATION SECTION
volatile unsigned int adc_reading;	    // Global Variable to read the ADC value
unsigned long ADCmillivolts;			// Global Variable to store the ADC value in Millivolts
unsigned char timer1_overflow;			// Timer1 Overflow variable
unsigned int start_edge, end_edge;		// Global variables for the start and end of the clock pulse edges.
unsigned long clocks;					// Global variable for the counts of clocks in a pulse
unsigned long Time_Period;		        // Global Variable to store the Time period of the 555 timer
unsigned int Time_Period_High;		    // Global Variable to store the High Time period of the 555 timer
unsigned int Time_Period_Low;			// Global Variable to store the Low Time period of the 555 timer
char input;								// Global variable used for switch statement
unsigned char qcntr = 0,sndcntr = 0;    // Indexes into the queue
unsigned char queue[50];				// Global character array for the character queue
volatile int ADCflag;					// Global Variable for the ADC Flag
int TimerFlag;						    // Global Variable for the Timer Flag
int continuous_adc;						// Global variable used for if continuous ADC mode is selected 
int continuous_timer;	                // Global variable used for if continuous Timer mode is selected 
int setWithADC;                         // Global variable used for if the motor position is to be set with ADC Value 

//***********************************************************************************************************************************************************************************************************************************//
//FUNCTION DECLARATION SECTION
void init_USART();		// Declare function to initialize USART
void sendmsg (char *s);		// Declare function to initialize sendmsg
void reportTimer();			// Declare function to initialize reportTimer to display Timer period
void reportLowPeriod();		// Declare function to initialize reportLowPeriod to display the low period of the Timer
void reportHighPeriod();    // Declare function to initialize reportLowPeriod to display the low period of the Timer
void reportADC();		    // Declare function to initialize reportADC to display ADC reading
void reportADCmV();			// Declare function to initialize reportADCmV to display ADC reading in mV
void reportDegrees() ;		// Declare function to initialize reportDegrees to display motor position/OCR2A value in degrees
void reportOCR();			// Declare function to initialize reportOCR to display OCR2A value
void initialise_portb();	// Declare function to initialize PORTB
void initialise_Timer0();   // Declare function to initialize Timer 0
void initialise_Timer1();   // Declare function to initialize Timer 1
void initialise_Timer2();   // Declare function to initialize Timer 2
void initialise_ADC();      // Declare function to initialize the ADC

//***********************************************************************************************************************************************************************************************************************************//

//MESSAGE ARRAY SECTION
char msg1[100];																																	// Generic array used to display a message for most of the functions
char msg2[200] ={"The motor is in the starting position. Press a number from 1-8 to change its position or Press 'r/R' to change with ADC"};	// Array used to display a message for when the servo motor is in the starting position
char msg3[50] ={"The motor is controlled by the ADC value"};																					// Array used to display a message for when the servo motor is being controlled by the ADC

//***********************************************************************************************************************************************************************************************************************************//

//MAIN METHOD
int main(void)
{
	timer1_overflow=0;	// Timer 1 overflow initially set to 0
	ADCflag=0;	        // ADC flag initially set to 0
	TimerFlag=0;		// Timer flag initially set to 0

	
	start_edge = 0;  // Starting edge of clock pulse initially set to 0;
	
	init_USART();               // USART initialization function is called
	initialise_portb();			// PORTB initialization function is called
	initialise_Timer0();		// Timer0 initialization function is called
	initialise_Timer1();		// Timer1 initialization function is called
	initialise_Timer2();		// Timer2 initialization function is called
	initialise_ADC();			// ADC initialization function is called
	

	sei();						//Global Interrupts are enabled

	OCR2A=15;					//Servo Motor position/ OCR2A set to starting position
	
	while (1)
	{


		if (UCSR0A & (1<<RXC0))					//Check for character received
		{
			input = UDR0;						//Get character sent from PC
			
			switch (input)						//Switch statement created
			
			
			{
				case '1': OCR2A = 16;		//If '1' is received from PC, set OCR2A to the value shown
				break;
				
					case '2': OCR2A = 18;		//If '2' is received from PC, set OCR2A to the value shown
					break;
				
						case '3': OCR2A = 20;		//If '3' is received from PC, set OCR2A to the value shown
						break;
				
							case '4': OCR2A = 22;		//If '4' is received from PC, set OCR2A to the value shown
							break;
				
								case '5': OCR2A = 25;		//If '5' is received from PC, set OCR2A to the value shown
								break;
				
									case '6': OCR2A = 27;		//If '6' is received from PC, set OCR2A to the value shown
									break;
				
										case '7': OCR2A = 29;		//If '7' is received from PC, set OCR2A to the value shown
										break;
				
											case '8': OCR2A = 31;		//If '8' is received from PC, set OCR2A to the value shown
											break;
				
				
				case 't':					//If 't' OR 'T' is received from PC call function reportTimer()
				case 'T':
					reportTimer();
				break;
				
						case 'l':					//If 'l' OR 'L' is received from PC call function reportLowPeriod()
						case 'L':
							reportLowPeriod();
						break;
				
								case 'h':					//If 'h' OR 'H' is received from PC call function reportHighPeriod()
								case 'H':
									reportHighPeriod();
								break;
				
										case 'c':					//If 'c' OR 'c' is received from PC set variable continuous_timer to 1
										case 'C':
											continuous_timer=1;
										break;
				
												case 'e':				//If 'e' OR 'E' is received from PC set continuous_timer to 0
												case 'E':
													continuous_timer=0;
												break;
				
														case 'a':					//If 'a' OR 'A' is received from PC call function reportADC()
														case 'A':
															reportADC();
														break;
				
																case 'v':					//If 'v' OR 'V' is received from PC call function reportADCmV()
																case 'V':
																	reportADCmV();
																break;
						
																		case 'w':					//If 'w' OR 'W' is received from PC set continuous_adc to 1
																		case 'W':
																			continuous_adc=1;
																		break;
				
				
																				case 'q':					//If 'w' OR 'W' is received from PC set continuous_adc to 1
																				case 'Q':
																					continuous_adc=0;
																				break;
				
																						case 'd':				//If 'd' OR 'D' is received from PC call function reportDegrees()
																						case 'D':
																							reportDegrees();
																						break;
					
					
																								case 's':				//If 's' OR 'S' is received from PC call function reportOCR()
																								case 'S':
																									reportOCR();
																								break;
				
				
																										case 'r':					//If 'r' OR 'R' is received from PC set setWithADC to 1
																										case 'R':
																											setWithADC=1;
																										break;
				
									
				
																												case 'y':					//If 'y' OR 'Y' is received from PC set setWithADC to 0
																												case 'Y':
																													setWithADC=0;
																												break;
			
													
																												default:					 //Default statement is to do nothing
																												break;
			} // end of switch statement
			
		} // end of if statement 
		
						if(continuous_adc==1)
					{
						if(ADCflag==1)			
						{
							if(qcntr==sndcntr)		
							reportADCmV();			// Call function reportADCmV
							ADCflag=0;				// Set ADC flag to 0
						}
					}
		
		
						if(continuous_timer==1)   
						{
							if(TimerFlag==1)
							{
								if(qcntr==sndcntr)
								reportTimer();		// Call function reportTimer
								TimerFlag=0;		// Set timer flag to 0
							}
						}
				
	}	// end of while loop
}	// end of main method 

//***********************************************************************************************************************************************************************************************************************************//

//FUNCTION DEFINITION SECTION

void reportTimer() //Definition for reportTimer function
{
	sprintf(msg1, "The period of the 555 timer = %lu us", Time_Period);
	sendmsg(msg1);
}

			void reportLowPeriod() //Definition for reportLowPeriod function
			{
				sprintf(msg1,"Low time period is %u",Time_Period_Low);
				sendmsg(msg1);
			}

						void reportHighPeriod()  //Definition for reportHighPeriod function
						{
							sprintf(msg1,"High time period is %u",Time_Period_High);
							sendmsg(msg1);
						}

								void reportADC() //Definition for reportADC function
								{
									sprintf(msg1, "The ADC reading is = %d", adc_reading);
									sendmsg(msg1);
								}

										void reportADCmV() //Definition for reportADCmV function
										{
											ADCmillivolts = (unsigned long) adc_reading*5000/1024;
											sprintf(msg1, "The ADC reading in mV is = %lu mV",ADCmillivolts);
											sendmsg(msg1);

										}
												void reportOCR() //Definition for report OCR function
												{
													sprintf(msg1, "The OCR2A value is = %d", OCR2A);
													sendmsg(msg1);
												}

														void reportDegrees() //Definition for reportDegrees function
														{
		
															unsigned int motorsDegree;
	
															for(int i = 0; i < 16; i++)
															{
																if(OCR2A == (i+16))
																{
																	motorsDegree = (90+(i*6));
																	sprintf(msg1,"Servo motor position is %u degrees",motorsDegree);
																	sendmsg(msg1);
																	break;
																}
															}
	
																	if(OCR2A==15)
																	{
																		sendmsg(msg2);
																	}
															}

//***********************************************************************************************************************************************************************************************************************************//

ISR (ADC_vect)//handles ADC interrupts
{
	
	adc_reading = ADC;   // Adc_reading is assigned to the value of the ADC
	ADCflag=1;			// ADCFlag is set to 1
	TCNT0=100;			//T Count 0 is set to 100
	TIFR0=(1<<TOV0);	
	

	
	if(setWithADC)  //If setWithADC is true, the carry out the code below
	{

		
		//For each line below, the ADC reading is compared with a value, and if the ADC reading is less than the value, the motor position will be changed.
		if(adc_reading < 64)
		OCR2A = 16;
		else if(adc_reading < 128)
		OCR2A = 17;
		else if(adc_reading < 192)
		OCR2A = 18;
		else if(adc_reading < 256)
		OCR2A = 19;
		else if(adc_reading < 320)
		OCR2A = 20;
		else if(adc_reading < 384)
		OCR2A = 21;
		else if(adc_reading < 448)
		OCR2A = 22;
		else if(adc_reading< 512)
		OCR2A = 23;
		else if(adc_reading < 576)
		OCR2A = 24;
		else if(adc_reading < 640)
		OCR2A = 25;
		else if(adc_reading < 704)
		OCR2A = 26;
		else if(adc_reading < 768)
		OCR2A = 27;
		else if(adc_reading < 832)
		OCR2A = 28;
		else if(adc_reading < 896)
		OCR2A = 29;
		else if(adc_reading < 960)
		OCR2A = 30;
		else
		OCR2A = 31;
		
		//Switch input created in order to send message to the user saying that the motor is controlled by the ADC
		switch(input)
		{
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			if(qcntr==sndcntr)
			sendmsg(msg3);	// Message 3 is sent
			input=0;
			break;
			
		}
	}

	
}



ISR(TIMER1_OVF_vect)
{
	++timer1_overflow;			//count the number of times the interrupt has been reached
}



ISR(TIMER1_CAPT_vect)
{
	static unsigned long last_time_period;
	end_edge = ICR1;	// The C compiler reads two 8bit regs for us
	
	clocks = ((unsigned long)timer1_overflow * 65536) + (unsigned long)end_edge - (unsigned long)start_edge;
	PORTD = (clocks/2000);		// Result in milliseconds
	
	
	
	timer1_overflow = 0;					// Clear timer1_overflow for next time around
	start_edge = end_edge;			// We're counting rising to rising, so this end = next start
	// Save its time for next time through here
	
	if(TCCR1B &(1<<ICES1))
	{
		Time_Period_Low = clocks/2;		// in microseconds
		TCCR1B ^= 1<<ICES1;
	}
	else
	{
		Time_Period_High = clocks/2;		// in microseconds
		TCCR1B ^= 1<<ICES1;
	}

	Time_Period = Time_Period_High + Time_Period_Low;
	
	if(last_time_period != Time_Period)
	{
		TimerFlag = 1;
	}
	last_time_period = Time_Period;
	
	
}


void sendmsg (char *s)
{
	qcntr = 0;    /*preset indices*/
	sndcntr = 1;  /*set to one because first character already sent*/
	
	queue[qcntr++] = 0x0d;   /*put CRLF into the queue first*/
	queue[qcntr++] = 0x0a;
	while (*s)
	queue[qcntr++] = *s++;   /*put characters into queue*/
	
	UDR0 = queue[0];  /*send first character to start process*/
	UCSR0B	= (1<<RXEN0) | (1<<TXEN0) | (1<<UDRIE0);
}


/********************************************************************************/
/* Interrupt Service Routines													*/
/********************************************************************************/

/*this interrupt occurs whenever the */
/*USART has completed sending a character*/

ISR(USART_UDRE_vect)
{
	/*send next character and increment index*/
	if (qcntr != sndcntr)
	UDR0 = queue[sndcntr++];
	else
	UCSR0B	= (1<<RXEN0) | (1<<TXEN0) ;  /* Turn off UDRIE0 */
	
}

void initialise_portb()
{
	DDRB = 0b00001000;		/* Bit 3 output, OC2A output rest inputs  */
	
}

void initialise_Timer0()
{
	
	TCCR0A = 0; // Disable all o/p waveforms
	TCCR0B = (5<<CS00);	/* Set T0 Source =Clock (16MHz)/1024 = 15625kHz  and put Timer in Normal mode	*/
	TIMSK0 =(0<<TOIE0); // Timer 0 Overflow interrupt disabled
	TCNT0 = 100;		/* Recall: 256-100 = 156 & 156*64us = 9.984ms, approx 10ms		*/
	
}

void initialise_Timer1()
{
	TCCR1A = 0; // Disable all o/p waveforms
	TCCR1B=(2<<CS10 | 0<<ICNC1 | 0<<ICES1); /*Set T1 Source = Clock (16MHz)/8 = 2MHz and put Timer in Normal mode
	Input noise canceler disabled
	Input edge selected to FALLING edge*/
	TIMSK1 = (1<<ICIE1)|(1<<TOIE1); // capture and overflow interrupts enabled
}
void initialise_Timer2()
{
	TCCR2A=(2<<COM2A0 | 3<<WGM20); // This sets OC2RA output enabled on bit 3, Fast PWM Mode , Clear 0C2A on compare match(non-inverting)
	TCCR2B=(7<<CS20); /* Set T2 Source =Clock (16MHz)/1024 = 15625kHz  and put Timer in Normal mode	*/
}

void initialise_ADC()
{
	ADMUX = ((1<<REFS0) | (2<<MUX0));  /* AVCC selected for VREF, ADC channel 2 as ADC input  */
	ADCSRA = ((1<<ADEN)|(1<<ADATE)|(1<<ADIE)|(7<<ADPS0));
	ADCSRB = (1<<ADTS2); //Select AutoTrigger Source to Timer 0 overflow
	/* Enable ADC,Start Conversion,Auto Trigger enabled,
	Interrupt enabled, Prescaler = 128  */
	
}
void init_USART()
{
	//Code to set Baud rate correctly
	#include <util/setbaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	#if USE_U2X
	UCSR0A |= (1<<U2X0);
	#else
	UCSR0A &= ~(1<<U2X0);
	#endif
	

	UCSR0A=(1<<U2X0); //Double Speed enabled
	UCSR0C |= (3 << UCSZ00); //8 bit data
	UCSR0B	|= (1<<RXEN0) | (1<<TXEN0)|(1<<TXC0) ; /*enable receiver, transmitter and transmit interrupt*/


	
}
