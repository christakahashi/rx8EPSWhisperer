/*
 * epsWhisper.c
 *
 * Created: 12/7/2020 10:03:09 PM
 * Author : chris
 */ 

#include <avr/io.h>
#include <avr/cpufunc.h>
#include "config.h"  //for f_cpu needeb by delay
#include <util/delay.h>
#include "can_lib.h" 

//utility to convert kph to the 2byte can 
#define KPH2SIG(x) ( (uint16_t)((x*100)+10000) )

st_cmd_t msg201;
//load with default known working values.
uint8_t payload[8] = {0x0d,0xd4,0xff,0xff,0x27,0x10,0x14,0xff};


void suart_tx(uint8_t);
void init_suart();
void init_ADC(void);
uint8_t read_ADC(void);
void print_byte(uint8_t ad);

int main(void)
{ 
	uint8_t ad;
	//just in case make sure the clk is unscaled.
	CLKPR = 0x80;
	CLKPR = 0x00;
	
	init_suart();
	can_init(0);

	sei();  //enable interrupts now
	init_ADC();
	
	 
    while (1) {

		ad = read_ADC()>>2;
		payload[4] =0x27+ad;  //0-100mph.
		
		msg201.pt_data = payload;
		msg201.ctrl.ide = 0; //2.0A std frame.
		msg201.dlc = sizeof(payload); //number of bytes sent.
		msg201.id.std = 0x201;
		msg201.cmd = CMD_TX; 
		//spin on cmd until accepted, should never spin in my code though
		// as I'm not sending a lot of messages and the bus should be mostly free.
		
		while(can_cmd(&msg201) != CAN_CMD_ACCEPTED);
		
		
		
		//print_byte(ad);
		//suart_tx('\n');


		//send every 100ms TODO: make this exactly periodic
		// supposed to be 10ms, but 100ms is fine and will save some power(heat).
		_delay_ms(100); 
		
		//call needed no matter what.  it frees the message.
		while(can_get_status(&msg201) == CAN_STATUS_NOT_COMPLETED); // Wait for Tx to complete
		
	}
}

void print_byte(uint8_t ad){
	char c;
	//some testing stuff
	if ( (ad>>4 &0x0f) >9){
		c = (ad>>4 &0x0f)-10+'A';
		} else {
		c = (ad>>4 &0x0f)+'0';
	}
	suart_tx(c);
	if ( (ad & 0x0f) >9){
		c = (ad  &0x0f)-10+'A';
		} else {
		c = (ad  &0x0f)+'0';
	}
	suart_tx(c);
}

void init_ADC() {
	//Avcc no ext cap.  ADC port 6.  left align bytes.
	ADMUX =  (1<<REFS0)| (1<<ADLAR) | (1<<MUX2) | (1<<MUX1);
	DIDR0 = 1<<ADC6D; //turn off digital input on adc6 
	//enable adc, set clk 125khz
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1); 
	ADCSRB = 0; //just incase.
	ADCSRA |= 1<<ADSC;  //start a conversion just to set things up
	while(ADCSRA & (1<<ADSC));  //wait for conv to end.
	
}

uint8_t read_ADC() {
	ADCSRA |= 1<<ADSC;  //start a conversion just to set things up
	while(ADCSRA & (1<<ADSC));  //wait for conv to end.
	
	return ADCH;
}




#define SS_DDR_PORT DDRB
#define SS_PORT PORTB
#define SS_PIN 2


void init_suart(){
	SS_DDR_PORT |= 1<<SS_PIN;
}

void suart_tx(uint8_t c) { 
	//139 clocks at 8mhz is (close to) 57600 bps =17.3611us
	
	//START BIT
	SS_PORT &=~(1<<SS_PIN);
	__builtin_avr_delay_cycles(139-1);
	
	
	for(uint8_t x=8;x>0;x--) {//4 instructions i think
		//each branch 6 instructions
		if (c&1) {
			SS_PORT |=1<<SS_PIN;  //sbi = 1 cycle
		}
		else {
			SS_PORT &=~(1<<SS_PIN); //cbi = 1 cycle
			//its shaky which branch the nops go.  
			// it depends on if the compiler uses SBIC or SBIS
			// inline asm would be smarter... but im lazy.
			_NOP();
			_NOP(); 
			_NOP();
		}
		c = c>>1; // 1 inst
		//not going to lie,
		// I tweaked the delays with a scope until it was right.
		__builtin_avr_delay_cycles(139-6-4);
	}
	//stop bit
	SS_PORT |= 1<<SS_PIN;
	//don't bother with delay here since I'll call other code.
	
}