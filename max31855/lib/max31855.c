/*
 * max31855.c
 *
 * Created: 2016-12-12 21:29:57
 *  Author: saper
 */ 

#include <avr/io.h>
#include <inttypes.h>
 
#include "max31855.h"
#include "delay.h"

// init
void max_init(void) {
	#if (!defined(MAX_CS_DRIVE_DISABLE))
		MAX_CS_PORT |= 1<<MAX_CS_PN;
		MAX_CS_DDR |= 1<<MAX_CS_PN;
	#endif

	MAX_CLK_PORT &= ~(1<<MAX_CLK_PN);
	MAX_CLK_DDR |= 1<<MAX_CLK_PN;

	MAX_DO_PORT |= 1<<MAX_DO_PN;
	MAX_DO_DDR &= ~(1<<MAX_DO_PN);

}

/*
uint32_t max_get_raw32(void) {
	uint32_t r=0;
	for(uint8_t i=0;i<32;i++) {
		r <<= 1;
		// clk: lo->hi
		max_clk_hi;
		delay1us(MAX_CLK_HDELAY);
		// probe DO & update r
		if (max_get_do_pin) r |= 0x00000001;
		max_clk_lo;
		delay1us(MAX_CLK_HDELAY);
	}
	return r;
}*/

// read frame from max31855 and return in two 16bit variables: hw - high word (bits 31 to 16), lw - low word (bits 15 to 0)
// if CS drive is disabled then before calling this you have to enable CS from your code, this apply after calling this function too.
void max_get_raw(int16_t *hw, int16_t *lw) {
	#if (!defined(MAX_CS_DRIVE_DISABLE))
		max_cs_lo;
	#endif
	delay1us(MAX_CLK_HDELAY);
	uint16_t r=0;

	for(uint8_t i=0;i<32;i++) {
		if (i == 0 || i==16) r=0;
		// shift r
		r <<= 1;
		// clk: lo->hi
		max_clk_hi;
		delay1us(MAX_CLK_HDELAY);
		// probe DO & update r
		if (max_get_do_pin) r |= 0x0001;
		max_clk_lo;
		delay1us(MAX_CLK_HDELAY);
		if (i==15) {
			(*hw)=r;
			r=0;
		}
	}

	(*lw) = r;

	#if (!defined(MAX_CS_DRIVE_DISABLE))
		max_cs_hi;
	#endif
	delay1us(MAX_CLK_HDELAY);

}



// return error bits: [3]-Fault (global error), [2]-SCV fault (short to Vcc), [1]-SCG fault (short to gnd), [0]-OC fault (open error)
// params out:
//    tr - remote sensor temperature (14bit resolution, LSB=0.25degC)
//    tl - local (internal) temperature (12bit resolution, LSB=0.0625degC)
// params in:
//    tr - MSB bits [31:16] of frame
//    tl - LSB bits [15:0]  of frame
uint8_t max_process_raw(int16_t *tr, int16_t *tl) {
	uint8_t re = 0;
	int16_t r=0, l=0;
	r=(*tr);
	l=(*tl);
	if (r&0x0001) re |= 0x08;
	re |= (l&0x07);

	// remote
	r >>= 2;
	// check sign bit, if set then set MSB bits [15:14] for negative value
	if (r & 0x2000) r |= 0xC000;

	// local
	l >>= 4;
	// check if bit[11] is set mthen copy it to bits [12:15] to make a proper negative int16
	if (l & 0x0800) l |= 0xF000;

	(*tr)=r;
	(*tl)=l;

	return re;
}