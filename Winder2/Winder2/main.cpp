/*
 * Winder2.cpp
 *
 * Created: 8/17/2017 4:50:47 PM
 * Author : CNTS04
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>		// pentru a putea utiliza functia sprintf
#include "hd44780.c"	// libraria LCD

// Voltage Reference: AVCC pin
#define ADC_VREF_TYPE ((0<<REFS1) | (1<<REFS0) | (0<<ADLAR))
#define Inceputul_liniei_1 0
#define Inceputul_liniei_2 64

enum {btnRIGHT, btnUP, btnDOWN, btnLEFT, btnSELECT, btnNONE};
	
char buffer [50];
uint8_t pozitia_cursor=0, imaginea=0;
uint8_t Button = btnNONE;	

// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input)
	{
		ADMUX=adc_input | ADC_VREF_TYPE;
		// Delay needed for the stabilization of the ADC input voltage
		_delay_us(10);
		// Start the AD conversion
		ADCSRA|=(1<<ADSC);
		// Wait for the AD conversion to complete
		while ((ADCSRA & (1<<ADIF))==0);
		ADCSRA|=(1<<ADIF);
		return ADCW;
	}

// read the buttons
int read_LCD_buttons()
	{
		int adc_key_in  = 0;
		adc_key_in = read_adc(0);      // read the value from the sensor 
		// my buttons when read are centered at these valies: 0, 144, 329, 504, 741
		// we add approx 50 to those values and check to see if we are close
		if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
		// For V1.1 us this threshold
		if (adc_key_in < 50)   return btnRIGHT;  
		if (adc_key_in < 250)  return btnUP; 
		if (adc_key_in < 450)  return btnDOWN; 
		if (adc_key_in < 650)  return btnLEFT; 
		if (adc_key_in < 850)  return btnSELECT;  

		return btnNONE;  // when all others fail, return this...
	}
	
void Afisare(int Cursor, char* Date_Afisare)
	{
		lcd_goto(Cursor);
		lcd_puts(Date_Afisare);
	}	

inline void MeniulInitializare()
	{
		lcd_clrscr();
		lcd_puts("WinderMachineV.2");
	}
inline void MeniulPrincipal()
	{
		bool Eliberare_Buton=false;
		lcd_clrscr();
		Afisare(Inceputul_liniei_1, ">");
		Afisare(Inceputul_liniei_1+2, "Start");
		Afisare(Inceputul_liniei_2+2, "Settings");
		do
			{
				do
					{
						do
							{
								Button = read_LCD_buttons();
								if(Button==btnNONE)
									Eliberare_Buton=false;
							}while(Button != btnNONE && Eliberare_Buton == true);

						_delay_ms(10);
					}while(Button == btnNONE);
				
				Eliberare_Buton=true;
				if(Button == btnDOWN)
					{
						Afisare(Inceputul_liniei_1, " ");
						Afisare(Inceputul_liniei_2, ">");
						pozitia_cursor=1;
					}
				if(Button == btnUP)
					{
						Afisare(Inceputul_liniei_1, ">");
						Afisare(Inceputul_liniei_2, " ");
						pozitia_cursor=0;
					}
			}while(Button != btnRIGHT && Button != btnSELECT);
	}
inline void MeniulSecundar()
{
	lcd_clrscr();

	Button = btnNONE;
	pozitia_cursor=0;
	do
	{
		Button = read_LCD_buttons();
		if(Button == btnDOWN)
		{
			if(pozitia_cursor==0)
			pozitia_cursor++;
			else
			if(imaginea<3)
			imaginea++;
		}
		if(Button == btnUP)
		{
			if(pozitia_cursor==1)
			pozitia_cursor--;
			else
			if(imaginea>0)
			imaginea--;
		}
		
		lcd_clrscr();
		if(pozitia_cursor==0)
		{
			Afisare(Inceputul_liniei_1, ">");
			Afisare(Inceputul_liniei_2, " ");
		}
		else
		{
			Afisare(Inceputul_liniei_1, " ");
			Afisare(Inceputul_liniei_2, ">");
		}
		
		switch(imaginea)
		{
			case 0:
			{
				sprintf(buffer, "Diam.s=");
				Afisare(Inceputul_liniei_1+2, buffer);
				sprintf(buffer, "Lung.b=");
				Afisare(Inceputul_liniei_2+2, buffer);
				break;
			}
			case 1:
			{
				sprintf(buffer, "Lung.b=");
				Afisare(Inceputul_liniei_1+2, buffer);
				sprintf(buffer, "Nr.spire=");
				Afisare(Inceputul_liniei_2+2, buffer);
				break;
			}
			case 2:
			{
				sprintf(buffer, "Nr.spire=");
				Afisare(Inceputul_liniei_1+2, buffer);
				sprintf(buffer, "Coef.d=");
				Afisare(Inceputul_liniei_2+2, buffer);
				break;
			}
			case 3:
			{
				sprintf(buffer, "Coef.d=");
				Afisare(Inceputul_liniei_1+2, buffer);
				sprintf(buffer, "Bobin/s=");
				Afisare(Inceputul_liniei_2+2, buffer);
				break;
			}
		}
		do
		{
			Button = read_LCD_buttons();
			_delay_ms(1);
		}
		while(Button != btnNONE);
		
	}
	while (Button != btnRIGHT && Button != btnSELECT);
	lcd_clrscr();
	_delay_ms(400);


}	
int main(void)
	{
		// Input/Output Ports initialization
		// Port B initialization
		// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
		DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
		// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T
		PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

		// Port C initialization
		// Function: Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
		DDRC=(0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
		// State: Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T
		PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

		// Port D initialization
		// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
		DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
		// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T
		PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

		// Timer/Counter 0 initialization
		// Clock source: System Clock
		// Clock value: Timer 0 Stopped
		// Mode: Normal top=0xFF
		// OC0A output: Disconnected
		// OC0B output: Disconnected
		TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (0<<WGM00);
		TCCR0B=(0<<WGM02) | (0<<CS02) | (0<<CS01) | (0<<CS00);
		TCNT0=0x00;
		OCR0A=0x00;
		OCR0B=0x00;

		// Timer/Counter 1 initialization
		// Clock source: System Clock
		// Clock value: Timer1 Stopped
		// Mode: Normal top=0xFFFF
		// OC1A output: Disconnected
		// OC1B output: Disconnected
		// Noise Canceler: Off
		// Input Capture on Falling Edge
		// Timer1 Overflow Interrupt: Off
		// Input Capture Interrupt: Off
		// Compare A Match Interrupt: Off
		// Compare B Match Interrupt: Off
		TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
		TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
		TCNT1H=0x00;
		TCNT1L=0x00;
		ICR1H=0x00;
		ICR1L=0x00;
		OCR1AH=0x00;
		OCR1AL=0x00;
		OCR1BH=0x00;
		OCR1BL=0x00;

		// Timer/Counter 2 initialization
		// Clock source: System Clock
		// Clock value: Timer2 Stopped
		// Mode: Normal top=0xFF
		// OC2A output: Disconnected
		// OC2B output: Disconnected
		ASSR=(0<<EXCLK) | (0<<AS2);
		TCCR2A=(0<<COM2A1) | (0<<COM2A0) | (0<<COM2B1) | (0<<COM2B0) | (0<<WGM21) | (0<<WGM20);
		TCCR2B=(0<<WGM22) | (0<<CS22) | (0<<CS21) | (0<<CS20);
		TCNT2=0x00;
		OCR2A=0x00;
		OCR2B=0x00;

		// Timer/Counter 0 Interrupt(s) initialization
		TIMSK0=(0<<OCIE0B) | (0<<OCIE0A) | (0<<TOIE0);

		// Timer/Counter 1 Interrupt(s) initialization
		TIMSK1=(0<<ICIE1) | (0<<OCIE1B) | (0<<OCIE1A) | (0<<TOIE1);

		// Timer/Counter 2 Interrupt(s) initialization
		TIMSK2=(0<<OCIE2B) | (0<<OCIE2A) | (0<<TOIE2);

		// External Interrupt(s) initialization
		// INT0: Off
		// INT1: Off
		// Interrupt on any change on pins PCINT0-7: Off
		// Interrupt on any change on pins PCINT8-14: Off
		// Interrupt on any change on pins PCINT16-23: Off
		EICRA=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
		EIMSK=(0<<INT1) | (0<<INT0);
		PCICR=(0<<PCIE2) | (0<<PCIE1) | (0<<PCIE0);

		// USART initialization
		// USART disabled
		UCSR0B=(0<<RXCIE0) | (0<<TXCIE0) | (0<<UDRIE0) | (0<<RXEN0) | (0<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);

		// Analog Comparator initialization
		// Analog Comparator: Off
		// The Analog Comparator's positive input is
		// connected to the AIN0 pin
		// The Analog Comparator's negative input is
		// connected to the AIN1 pin
		ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
		ADCSRB=(0<<ACME);
		// Digital input buffer on AIN0: On
		// Digital input buffer on AIN1: On
		DIDR1=(0<<AIN0D) | (0<<AIN1D);

		// ADC initialization
		// ADC Clock frequency: 1000.000 kHz
		// ADC Voltage Reference: AVCC pin
		// ADC Auto Trigger Source: ADC Stopped
		// Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
		// ADC4: On, ADC5: On
		DIDR0=(0<<ADC5D) | (0<<ADC4D) | (0<<ADC3D) | (0<<ADC2D) | (0<<ADC1D) | (0<<ADC0D);
		ADMUX=ADC_VREF_TYPE;
		ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (1<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);
		ADCSRB=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

		// SPI initialization
		// SPI disabled
		SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

		// TWI initialization
		// TWI disabled
		TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);
    
		//===================================== LCD =====================================================//
		//************ SETARILE DE CONECTARE A LCD SUNT IN FISIERUL hd44780_setings.h *******************//
		DDRD=0xFF;	// LCD este conectat la Portul D
		//***********************************************************************************************//
		lcd_init();

		MeniulInitializare();
		_delay_ms(10);
		MeniulPrincipal();
		MeniulSecundar();
		
		
		
		while (1) 
			{
				lcd_clrscr();
				switch (read_LCD_buttons())               // depending on which button was pushed, we perform an action
					{
						case btnRIGHT:
							{
								sprintf(buffer,"Butonul = %d", btnRIGHT);
								break;
							}
						case btnLEFT:
							{
								sprintf(buffer,"Butonul = %d", btnLEFT);
								break;
							}
						case btnUP:
							{
								sprintf(buffer,"Butonul = %d", btnUP);
								break;
							}
						case btnDOWN:
							{
								sprintf(buffer,"Butonul = %d", btnDOWN);
								break;
							}
						case btnSELECT:
							{
								sprintf(buffer,"Butonul = %d", btnSELECT);
								break;
							}
						case btnNONE:
							{
								sprintf(buffer,"Butonul = %d", btnNONE);
								break;
							}
					}	
				lcd_puts(buffer);	
				_delay_ms(5);
			
			}
	}
	
