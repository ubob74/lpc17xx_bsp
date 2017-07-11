#include <resources.h>
#include <lpc17xx_eth.h>
#include <lpc17xx_eth_resources.h>

struct eth_regs eth_regs = {
	.MAC1 =			0x50000000,
	.MAC2 =			0x50000004,
	.IPGT =			0x50000008,
	.IPGR =			0x5000000C,
	.CLRT =			0x50000010,
	.MAXF =			0x50000014,
	.SUPP =			0x50000018,
	.SA0 =			0x50000040,
	.SA1 =			0x50000044,
	.SA2 =			0x50000048,
	.CMD =			0x50000100,
	.STAT =			0x50000104,
	.RX_DESC =		0x50000108,
	.RX_STAT =		0x5000010C,
	.RX_DESC_NUM =	0x50000110,
	.RX_PI =		0x50000114,
	.RX_CI =		0x50000118,
	.TX_DESC =		0x5000011C,
	.TX_STAT =		0x50000120,
	.TX_DESC_NUM =	0x50000124,
	.TX_PI =		0x50000128,
	.TX_CI =		0x5000012C,
	.TSV0 =			0x50000158,
	.TSV1 =			0x5000015C,
	.RSV =			0x50000160,
	.FCC =			0x50000170,
	.FCS =			0x50000174,
	.INT_STAT =		0x50000FE0,
	.INT_EN =		0x50000FE4,
	.INT_CLR =		0x50000FE8,
	.INT_SET =		0x50000FEC,
	.PWR_DOWN =		0x50000FF4,
};

struct pin eth_pins[] = {
	[0] = {
		/* P1.0 -> ENET_TXD0 (mode 1) */
		.pinsel_addr = PINSEL2,
		.num= 0,
		.mode = 1,
	},
	[1] = {
		/* P1.1 -> ENET_TXD1 (mode 1) */
		.pinsel_addr = PINSEL2,
		.num = 1,
		.mode = 1,
	},
	[2] = {
		/* P1.4 -> ENET_TX_EN (mode 1) */
		.pinsel_addr = PINSEL2,
		.num = 4,
		.mode = 1,
	},
	[3] = {
		/* P1.8 -> ENET_CRS (mode 1) */
		.pinsel_addr = PINSEL2,
		.num = 8,
		.mode = 1,
	},
	[4] = {
		/* P1.9 -> ENET_RXD0 (mode 1) */
		.pinsel_addr = PINSEL2,
		.num = 9,
		.mode = 1,
	},
	[5] = {
		/* P1.10 -> ENET_RXD1 (mode 1) */
		.pinsel_addr = PINSEL2,
		.num = 10,
		.mode = 1,
	},
	[6] = {
		/* P1.14 -> ENET_RX_ER (mode 1) */
		.pinsel_addr = PINSEL2,
		.num = 14,
		.mode = 1,
	},
	[7] = {
		/* P1.15 -> ENET_REF_CLK (mode 1) */
		.pinsel_addr = PINSEL2,
		.num = 15,
		.mode = 1,
	},
	[8] = {
		/* P1.16 -> ENET_MDC (mode 1) */
		.pinsel_addr = PINSEL3,
		.num = 0,
		.mode = 1,
	},
	[9] = {
		/* P1.17 -> ENET_MDIO (mode 1) */
		.pinsel_addr = PINSEL3,
		.num = 1,
		.mode = 1,
	},
};

struct eth_device lpc17xx_eth_device = {
	.enable_bit = 30,
	.dev = {
		.id = 0,
		.state = 1,
		.res = {
			.regs = {
				.regs = (void *)&eth_regs,
				.nr_regs = 0, /* not used now */
			},
			.pins = {
				.pins = eth_pins,
				.nr_pins = ARRAY_SIZE(eth_pins),
			},
		}, /* .res */
	},
};
