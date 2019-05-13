

/*******************************************************************************

 /*
 midterm2.c
 Created: 5/12/2019  PM
 Author : Alfonso Contreras
 Instructor: Dr. Venkatesan Muthukumar
 class: CPE 301
 This program was created for Atmega328P and what this program does is to
 analyze the light in three basic colors (RGB) red, green, and blue and
 show the three diferent values using the UART and then transmitting the data
 using ESP01 wifi module to Thinkspeak.
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "i2c_master.h"
#include "uart.h"
#include "apds9960.h"
#define BAUD 9600
#define BRGVAL (F_CPU/16/BAUD) - 1


object str_uart = FDEV_SETUP_STREAM(uart_char, NULL , _FDEV_SETUP_WRITE);
void int_UART();
int uart_char( char c, object *stream);
char readings[500];


int main(void)
{
	uint16_t red = 0, green = 0, blue = 0;
	
	i2c_init();
	int_UART();
	stdout = &str_uart;
	apds_init();
	
	_delay_ms(1000);
	printf("AT\r\n");
	
	_delay_ms(3000);
	printf("AT+CWMODE=1\r\n");
	
	_delay_ms(3000);
	printf("AT+CWJAP=\"mynetworkname\",\"password\"\r\n");
	
	while (1)
	{
		_delay_ms(3000);
		printf("AT+CIPMUX=0\r\n");
		
		_delay_ms(3000);
		printf("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
		
		_delay_ms(3000);
		readingColor(&red, &green, &blue);
		printf("AT+CIPSEND=104\r\n");
		printf("GET https://api.thingspeak.com/update?api_key=mykey&field1=%05u&field2=%05u&field3=%05u\r\n", red, green, blue);
		_delay_ms(5000);
	}
}

void apds_init(){
	uint8_t setup;
	i2c_readReg(APDS_WRITE, APDS9960_ID, &setup,1);
	if(setup != APDS9960_ID_1) while(1);
	setup = 1 << 1 | 1<<0 | 1<<3 | 1<<4;
	i2c_writeReg(APDS_WRITE, APDS9960_ENABLE, &setup, 1);
	setup = DEFAULT_ATIME;
	i2c_writeReg(APDS_WRITE, APDS9960_ATIME, &setup, 1);
	setup = DEFAULT_WTIME;
	i2c_writeReg(APDS_WRITE, APDS9960_WTIME, &setup, 1);
	setup = DEFAULT_PROX_PPULSE;
	i2c_writeReg(APDS_WRITE, APDS9960_PPULSE, &setup, 1);
	setup = DEFAULT_POFFSET_UR;
	i2c_writeReg(APDS_WRITE, APDS9960_POFFSET_UR, &setup, 1);
	setup = DEFAULT_POFFSET_DL;
	i2c_writeReg(APDS_WRITE, APDS9960_POFFSET_DL, &setup, 1);
	setup = DEFAULT_CONFIG1;
	i2c_writeReg(APDS_WRITE, APDS9960_CONFIG1, &setup, 1);
	setup = DEFAULT_PERS;
	i2c_writeReg(APDS_WRITE, APDS9960_PERS, &setup, 1);
	setup = DEFAULT_CONFIG2;
	i2c_writeReg(APDS_WRITE, APDS9960_CONFIG2, &setup, 1);
	setup = DEFAULT_CONFIG3;
	i2c_writeReg(APDS_WRITE, APDS9960_CONFIG3, &setup, 1);
}



void int_UART(void){
	//Set baud rate
	uint16_t baud_rate = BRGVAL;
	UBRR0H = baud_rate >> 8;
	UBRR0L = baud_rate & 0xFF;
	
	//Enable receiver and transmitter
	UCSR0B = ( 1 <<RXEN0)|( 1 <<TXEN0);
	
	// Set frame format: 8data, 1stop bit
	UCSR0C = (3 <<UCSZ00);
}

int uart_char(char c, object *stream){
	//wait until buffer empty
	while ( !( UCSR0A & ( 1 <<UDRE0)) );
	
	//Put data into buffer
	UDR0 = c;
	return 0;
}

void readingColor(uint16_t* red, uint16_t* green, uint16_t* blue) {
	uint8_t redl, redh;
	uint8_t greenl, greenh;
	uint8_t bluel, blueh;
	i2c_readReg(APDS_WRITE, APDS9960_RDATAL, &redl, 1);
	i2c_readReg(APDS_WRITE, APDS9960_RDATAH, &redh, 1);
	i2c_readReg(APDS_WRITE, APDS9960_GDATAL, &greenl, 1);
	i2c_readReg(APDS_WRITE, APDS9960_GDATAH, &greenh, 1);
	i2c_readReg(APDS_WRITE, APDS9960_BDATAL, &bluel, 1);
	i2c_readReg(APDS_WRITE, APDS9960_BDATAH, &blueh, 1);
	*red = redh << 8 | redl;
	*green = greenh << 8 | greenl;
	*blue = blueh << 8 | bluel;
}