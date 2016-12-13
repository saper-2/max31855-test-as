/*
 * max31855.c
 *
 * Created: 2016-12-12 20:42:46
 * Author : saper
 */ 

#include <avr/io.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "lib/delay.h"
#include "lib/usart.h"
#include "lib/max31855.h"


#define led_toggle PORTB ^= 0x01
#define led_off PORTB &= ~0x01
#define led_on PORTB |= 0x01

int main(void)
{
	uint8_t r;
	int16_t tr, tl;//, rh,rl;
	uint16_t t16, t162;
	//int32_t t32;
	//uint8_t i,j;
	DDRB |= 1<<0;
	
	sei();
	// 38400 8N1, Tx enabld only
	usart_config(25,USART_TX_ENABLE,USART_MODE_8N1); //usart_config(25,USART_RX_ENABLE|USART_TX_ENABLE|USART_RX_INT_COMPLET,USART_MODE_8N1);
	
	usart_send_strP(PSTR("\r\n\r\n********** MAX31855 **********\r\n"));

	max_init();

	delay1ms(1000); // max power-up init ( ~200ms )

	while(1) {
		max_get_raw(&tr,&tl);
		t16 = tr;
		t162 = tl;
		r = max_process_raw(&tr,&tl);

		// fault
		usart_send_strP(PSTR("\r\nF="));
		if (r&0x08) { usart_send_char('1'); } else { usart_send_char('0'); }

		// SCV
		usart_send_strP(PSTR(" SCV="));
		if (r&0x04) { usart_send_char('1'); } else { usart_send_char('0'); }
		// SCG
		usart_send_strP(PSTR(" SCG="));
		if (r&0x02) { usart_send_char('1'); } else { usart_send_char('0'); }
		// SCV
		usart_send_strP(PSTR(" OC="));
		if (r&0x01) { usart_send_char('1'); } else { usart_send_char('0'); }

		usart_send_strP(PSTR(" RAW=0x"));
		usart_send_hex_byte(t16>>8); usart_send_hex_byte(t16);
		usart_send_hex_byte(t162>>8); usart_send_hex_byte(t162);

		usart_send_char(' '); usart_send_bin_byte(t16>>8); usart_send_char(' '); usart_send_bin_byte(t16);
		usart_send_char(' '); usart_send_bin_byte(t162>>8); usart_send_char(' '); usart_send_bin_byte(t162);

		usart_send_strP(PSTR(" Tr="));
		usart_send_int(tr);
		usart_send_strP(PSTR(" ("));
		r = tr & 0x0003;
		r *= 25;
		tr >>= 2;
		usart_send_int(tr);
		usart_send_char('.');
		if (r == 0) {
			usart_send_char('0'); usart_send_char('0');
		} else {
			usart_send_uint(r);
		}

		usart_send_strP(PSTR(" Tl="));
		usart_send_int(tl);
		usart_send_strP(PSTR(" ("));
		t16 = tl & 0x000f;
		t16 *= 625;
		tl >>= 4;
		usart_send_int(tl);
		usart_send_char('.');
		if (t16 < 1000) usart_send_char('0');
		if (t16 < 100) usart_send_char('0');
		if (t16 < 10) usart_send_char('0');
		usart_send_uint(t16);
		
		usart_send_char(')');

		delay1ms(1000); // 1sec delay
		led_toggle;

	}
 
}

