#ifndef _LPC17XX_SSP_H_
#define _LPC17XX_SSP_H_

#include <_stdint.h>
#include <resources.h>

#define DUMMY_BYTE	0xFF

#pragma pack(1)

/**
 * SSP registers:
 */
struct ssp_regs {
	uint32_t CR0;
	uint32_t CR1;
	uint32_t DR;
	uint32_t SR;
	uint32_t CPSR;
	uint32_t IMSC;
	uint32_t RIS;
	uint32_t MIS;
	uint32_t ICR;
	uint32_t DMACR;
};

/*
CR0 - Control register 0
	- dss - data size select
	- frf - frame format
	- cpol - clock out polarity
	- cpha - clock out phase
	- scr - serial clock rate
*/
struct CR0 {
	uint16_t val;
};

/*
CR1 - Control register 1
	- LBM - loop back mode
	- SSE - SSP enable
	- MS - master/slave mode
	- SOD - slave output disable
*/

#define LBM		0
#define SSE		1
#define MS		2
#define SOD		3

struct CR1 {
	uint8_t val;
};

/* DR - Data register */
struct DR {
	uint16_t val;
};

/*
SR - Status register
	- tfe - transmit FIFO Empty
	- tnf - transmit FIFO not Full
	- rne - receive FIFO not Empty
	- rff - receive FIFO Full
	- bsy - busy
*/

#define BSY 4
#define RNE 2

struct SR {
	int tfe :1;
	int tnf :1;
	int rne :1;
	int rff :1;
	int bsy :1;
	int rez :3;
};

/* CPSR - Clock Prescale register */
struct CPSR {
	uint8_t val;
};

/* IMSC - Iterrupt Set/Clear register */
struct IMSC {
	uint32_t val;
};

/* RIS - Raw Interrupt Status register */
struct RIS {
	uint32_t val;
};

/* MIS - Masked Interrupt Status register */
struct MIS {
	uint32_t val;
};

/* ICR - Interrupt Clear register */
struct ICR {
	uint32_t ICR;
};

/* DMACR - DMA control register */
struct DMACR {
	uint32_t val;
};

struct ssp_device {
	uint8_t enable_bit;
	uint8_t card_detect;
	uint8_t card_select;
	uint8_t card_power;
	struct device dev;
};

#define CLOCK_RATE	1000000

int lpc17xx_ssp0_init(void);

#endif
