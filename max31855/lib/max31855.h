/*
 * max31855.h
 *
 * Created: 2016-12-12 20:44:27
 *  Author: saper
 */ 


#ifndef MAX31855_H_
#define MAX31855_H_

#define MAX_SPI_MODE_SW 0
#define MAX_SPI_MODE_HW 1

#define MAX_SPI_MODE MAX_SPI_MODE_SW


#if MAX_SPI_MODE == MAX_SPI_MODE_SW
	
	#define MAX_DO_PORT PORTC
	#define MAX_DO_DDR DDRC
	#define MAX_DO_PIN PINC
	#define MAX_DO_PN 2

	// uncomment this line to disable controlling of CS line, you have to control CS line from main code
	//#define MAX_CS_DRIVE_DISABLE

	#if  (!defined(MAX_CS_DRIVE_DISABLE))
		#define MAX_CS_PORT PORTC
		#define MAX_CS_DDR DDRC
		#define MAX_CS_PIN PINC
		#define MAX_CS_PN 1

		#define max_cs_lo MAX_CS_PORT &= ~(1<<MAX_CS_PN)
		#define max_cs_hi MAX_CS_PORT |=  (1<<MAX_CS_PN)
	#endif

	#define MAX_CLK_PORT PORTC
	#define MAX_CLK_DDR DDRC
	#define MAX_CLK_PN 0

	// half clock time, this is used also for CS-SCK timings
	#define MAX_CLK_HDELAY 5 // [us]
	
	#define max_clk_lo MAX_CLK_PORT &= ~(1<<MAX_CLK_PN)
	#define max_clk_hi MAX_CLK_PORT |=  (1<<MAX_CLK_PN)

	#define max_get_do_pin ((MAX_DO_PIN & (1<<MAX_DO_PN)) == (1<<MAX_DO_PN))
#else
	#error "Not implemented hardware SPI support fo MAX31855."
#endif

void max_init(void); // just initialize ports

// read frame from max31855 and return in two 16bit variables: hw - high word (bits 31 to 16), lw - low word (bits 15 to 0)
// if CS drive is disabled then before calling this you have to enable CS from your code, this apply after calling this function too.
void max_get_raw(int16_t *hw, int16_t *lw);

// return error bits: [3]-Fault (global error), [2]-SCV fault (short to Vcc), [1]-SCG fault (short to gnd), [0]-OC fault (open error)
// params out: 
//    tr - remote sensor temperature (14bit resolution, LSB=0.25degC)
//    tl - local (internal) temperature (12bit resolution, LSB=0.0625degC) 
// params in:
//    tr - MSB bits [31:16] of frame
//    tl - LSB bits [15:0]  of frame
uint8_t max_process_raw(int16_t *tr, int16_t *tl); 



#endif /* MAX31855_H_ */