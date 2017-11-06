#include <io.h>
#include <resources.h>
#include <lpc17xx_irq.h>
#include <lpc17xx_uart.h>
#include <lpc17xx_uart_resources.h>

struct pin uart2_pins[] = {
	[0] = {
		 /* Pad configuration: P0.10 -> TXD2 (mode 1) */
		.pinsel_addr = 0x4002C000,
		.num = 10,
		.mode = 1,
	},
};

struct uart_regs uart_regs = {
	.LCR = 0x4009800C,
	.DLM = 0x40098004,
	.DLL = 0x40098000,
	.IER = 0x40098004,
	.FCR = 0x40098008,
	.IIR = 0x40098008,
	.LSR = 0x40098014,
	.THR = 0x40098000,
	.RBR = 0x40098000,
	.TER = 0x40098030,
};

struct uart_device lpc17xx_uart_devices[] = {
	[0] = {
		/* TODO */
		.dev = {
			.id = UART_0,
			.state = 0,
		},
	},
	[1] = {
		/* TODO */
		.dev = {
			.id = UART_1,
			.state = 0,
		},
	},
	[2] = {
		.enable_bit = 24, /* Connect UART2 - bit 24 in PCONP */
		.dev = {
			.id = UART_2,
			.state = 1,
			.irq = UART2,
			.res = {
				.regs = (void *)&uart_regs,
				.pins = {
					.pins = uart2_pins,
					.nr_pins = ARRAY_SIZE(uart2_pins),
				},
			},
		},
	},
	[3] = {
		/* TODO */
		.dev = {
			.id = UART_3,
			.state = 0,
		},
	},
};

struct uart_device_array lpc17xx_uart_device_array = {
	.uart_dev = lpc17xx_uart_devices,
	.nr_uart_devs = ARRAY_SIZE(lpc17xx_uart_devices),
};
