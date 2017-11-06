#include <lpc17xx_ssp.h>
#include <lpc17xx_ssp_resources.h>
#include <io.h>

static struct ssp_regs ssp0_regs = {
	.CR0 = 0x40088000,
	.CR1 = 0x40088004,
	.DR = 0x40088008,
	.SR = 0x4008800C,
	.CPSR = 0x40088010,
	.IMSC = 0x40088014,
	.RIS = 0x40088018,
	.MIS = 0x4008801C,
	.ICR = 0x40088020,
	.DMACR = 0x40088024,
};

static struct pin ssp0_pins[] = {
	[0] = {
		/* P1.20 - SCK0 (mode 3) */
		.pinsel_addr = PINSEL3,
		.num = 4,
		.mode = 3,
	},
	[1] = {
		/* P1.23 - MISO0 (mode 3) */
		.pinsel_addr = PINSEL3,
		.num = 7,
		.mode = 3,
	},
	[2] = {
		/* P1.24 - MOSI0 (mode 3) */
		.pinsel_addr = PINSEL3,
		.num = 8,
		.mode = 3,
	},
	[3] = {
		/* P1.21 - GPIO port 1.21 output SD_CS (chip select) */
		.pinsel_addr = PINSEL3,
		.num = 5,
		.mode = 0,
		.gpio = {
			.bank = 1,
			.pin_num = 21,
			.direction = GPIO_OUT,
		},
	},
	[4] = {
		/* P3.25 - GPIO port 3.25 input (SD_CD, card detect) */
		.pinsel_addr = PINSEL7,
		.num = 9,
		.mode = 0,
		.gpio = {
			.bank = 3,
			.pin_num = 25,
			.direction = GPIO_IN,
		},
	},
	[5] = {
		/* P3.26 - GPIO port 3.26 output (SD_PWR) */
		.pinsel_addr = PINSEL7,
		.num = 10,
		.mode = 0,
		.gpio = {
			.bank = 3,
			.pin_num = 26,
			.direction = GPIO_OUT,
		},
	},
};

struct ssp_device lpc17xx_ssp0_device = {
	.enable_bit = 21,
	.card_detect = 4,
	.card_select = 3,
	.card_power = 5,
	.dev = {
		.id = 0,
		.state = 1,
		.res = {
			.regs = (void *)&ssp0_regs,
			.pins = {
				.pins = ssp0_pins,
				.nr_pins = ARRAY_SIZE(ssp0_pins),
			},
		},
	}, /* dev */
};
