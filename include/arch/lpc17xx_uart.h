#ifndef _LPC17XX_UART_H_
#define _LPC17XX_UART_H_

#include <_stdint.h>
#include <atomic.h>
#include <resources.h>

/* UART Id */
#define UART_0	0
#define UART_1	1
#define UART_2	2
#define UART_3	3

/*
 LPC17XX UART register addresses:
  - LCR - Line Control register
  - DLM - Divisor latch MSB
  - DLL - Divisor latch LSB
  - IER - Interrupt Enable register
  - FCR - FIFO Control register, WO
  - IIR - Interrupt ID register
  - LSR - Line Status Register
  - THR - Transmitter Holding Register
  - RBR - Receiver Buffer Register
  - TER - Transmit Enable Register
*/
struct uart_regs {
	uint32_t LCR;
	uint32_t DLM;
	uint32_t DLL;
	uint32_t IER;
	uint32_t FCR;
	uint32_t IIR;
	uint32_t LSR;
	uint32_t THR;
	uint32_t RBR;
	uint32_t TER;
};

/* LCR bits: */
#define LCR_WL_SHIFT	0 /* word length shift */
#define LCR_WL_WIDTH	2 /* word length width */
#define LCR_STOP		2 /* stop bit */
#define LCR_PE			3 /* parity enable */
#define LCR_PSEL_SHIFT	4 /* parity select shift */
#define LCR_PSEL_WIDTH	2 /* parity select width */
#define LCR_DLAB		7 /* divisor latch access */

/* FIFO Control register bits: */
#define FCR_ENABLE		0 /* FIFO enable */
#define FCR_RX_FIFO		1 /* RX FIFO reset */
#define FCR_TX_FIFO		2 /* TX FIFO reset */

/* LSR bits: */
#define LSR_THRE		5 /* Transmitter holding register empty */

/* IER bits: */
#define IER_RBR			0 /* Enables the Receive Data Available interrupt for UARTn */
#define IER_THRE		1 /* Enables the THRE interrupt for UARTn */

/* TER bits: */
#define TXEN			7 /* Enable/Disable transmission */

struct uart_device {
	uint8_t enable_bit;
	atomic_t lock;
	int id;
	struct device dev;
	struct uart_data *uart_data;
};

struct uart_device_array {
	struct uart_device *uart_dev;
	int nr_uart_devs;
};

int lpc17xx_uart_init(void);

#endif /* _LPC17XX_UART_H_ */
