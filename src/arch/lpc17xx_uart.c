#include <_stdlib.h>
#include <_stdint.h>
#include <io.h>
#include <irq.h>
#include <uart.h>
#include <lpc17xx_irq.h>
#include <lpc17xx_uart.h>
#include <lpc17xx_uart_resources.h>
#include <lpc17xx_prcm.h>

static inline struct uart_device *uart_get_dev(int id)
{
	return lpc17xx_uart_device_array.uart_dev + id;
}

static inline void *uart_get_regs(struct uart_device *uart_dev)
{
	return dev_get_regs(&uart_dev->dev);
}

static void lpc17xx_uart_mux_pins(struct uart_device *uart_dev)
{
	dev_mux_pins(&uart_dev->dev);
}

static void send_byte(uint32_t lsr_reg, uint32_t thr_reg, uint8_t byte)
{
	uint32_t lsr_val;

	while (1) {
		lsr_val = readl(lsr_reg);
		if (lsr_val & BIT(LSR_THRE))
			break;
	}

	writel(thr_reg, byte);
}

static void tx_enable(uint32_t reg)
{
	uint32_t ter_val;

	/* Enable transmission */
	ter_val = readl(reg);
	ter_val |= BIT(TXEN);
	writel(reg, ter_val);
}

static void tx_disable(uint32_t reg)
{
	uint32_t ter_val;

	/* Disable transmission */
	ter_val = readl(reg);
	ter_val &= ~BIT(TXEN);
	writel(reg, ter_val);
}

static int lpc17xx_uart_irq_handler(void *arg)
{
	uint32_t lsr_val, iir_val;
	uint8_t val;
	struct uart_device *uart_dev = (struct uart_device *)arg;
	struct uart_regs *uart_regs = uart_get_regs(uart_dev);
	struct uart_data *uart_data = uart_dev->uart_data;

	iir_val = readl(uart_regs->IIR);
	if (iir_val & 0x1)
		goto out;

	iir_val >>= 1;
	if (!(iir_val & 0x3))
		goto out;

	lsr_val = readl(uart_regs->LSR);

	if (lsr_val & BIT(LSR_THRE)) {
		if (uart_data->cur_pos < uart_data->size) {
			val = uart_data->data[uart_data->cur_pos++];
			writel(uart_regs->THR, val);
		} else {
			uart_data->complete = 1;
		}
	}
out:
	return IRQ_HANDLED;
}

static int lpc17xx_uart_enable(int id)
{
	struct uart_device *uart_device;

	if (id < UART_0 || id > UART_3)
		return -1;

	uart_device = uart_get_dev(id);
	set_bit(PCONP, uart_device->enable_bit);
	return 0;
}

static int lpc17xx_uart_disable(int id)
{
	struct uart_device *uart_device;

	if (id < UART_0 || id > UART_3)
		return -1;

	uart_device = uart_get_dev(id);
	reset_bit(PCONP, uart_device->enable_bit);
	return 0;
}

static int lpc17xx_uart_set_word_length(int id, uint8_t word_length)
{
	struct uart_device *uart_device;
	struct uart_regs *uart_regs;

	if (id < 0 || id > UART_3)
		return -1;

	uart_device = uart_get_dev(id);
	uart_regs = uart_get_regs(uart_device);

	if (word_length < 5 || word_length > 8)
		return -1;

	return set_value(uart_regs->LCR, word_length - 5,
		LCR_WL_SHIFT, LCR_WL_WIDTH);
}

static int lpc17xx_uart_set_baud_rate(int id, uint32_t baud_rate)
{
	uint16_t us_fdiv;
	uint8_t dlm_val, dll_val;
	struct uart_device *uart_dev;
	struct uart_regs *uart_regs;

	if (id < UART_0 || id > UART_3)
		return -1;

	uart_dev = uart_get_dev(id);
	uart_regs = uart_get_regs(uart_dev);

	/* Latch the divisor */
	set_bit(uart_regs->LCR, LCR_DLAB);

	/* Set baud rate */
	us_fdiv = FPCLK / (16 * baud_rate);
	dlm_val = us_fdiv / 256;
	dll_val = us_fdiv % 256;
	writeb(uart_regs->DLM, dlm_val);
	writeb(uart_regs->DLL, dll_val);

	/* Drop Divisor Latch bit */
	reset_bit(uart_regs->LCR, LCR_DLAB);

	return 0;
}

static int lpc17xx_uart_set_stop_bit(int id, uint8_t stop_bit)
{
	struct uart_device *uart_dev;
	struct uart_regs *uart_regs;

	if (id < UART_0 || id > UART_3)
		return -1;

	uart_dev = uart_get_dev(id);
	uart_regs = uart_get_regs(uart_dev);

	switch (stop_bit) {
		case 2:
			set_bit(uart_regs->LCR, BIT(LCR_STOP));
			break;
		case 1:
			reset_bit(uart_regs->LCR, BIT(LCR_STOP));
			break;
		default:
			return -1;
	}

	return 0;
}

static int lpc17xx_uart_set_parity(int id)
{
	return 0;
}

static int lpc17xx_uart_start_rx(int id, struct uart_data *uart_data)
{
	return 0;
}

static int lpc17xx_uart_stop_rx(int id)
{
	return 0;
}

static int lpc17xx_uart_send_byte(int id, uint8_t byte)
{
	struct uart_device *uart_dev;
	struct uart_regs *uart_regs;

	if (id < UART_0 || id > UART_3)
		return -1;

	uart_dev = uart_get_dev(id);
	uart_regs = uart_get_regs(uart_dev);

	send_byte(uart_regs->LSR, uart_regs->THR, byte);

	return 0;
}

static int lpc17xx_uart_send_buffer(int id, struct uart_data *uart_data)
{
	/* TODO: DMA support */
	int i;
	struct uart_device *uart_dev;
	struct uart_regs *uart_regs;

	if (id < UART_0 || id > UART_3)
		return -1;

	uart_dev = uart_get_dev(id);
	uart_regs = uart_get_regs(uart_dev);

	if (!uart_data->data)
		return -1;

	for (i = 0; i < uart_data->size; i++)
		send_byte(uart_regs->LSR, uart_regs->THR,
				uart_data->data[i]);

	return 0;
}

static int lpc17xx_uart_start_tx(int id, struct uart_data *uart_data)
{
	uint32_t ier_val;
	struct uart_device *uart_dev;
	struct uart_regs *uart_regs;

	if (id < UART_0 || id > UART_3)
		return -1;

	uart_dev = uart_get_dev(id);
	uart_regs = uart_get_regs(uart_dev);

	writel(uart_regs->FCR, BIT(FCR_ENABLE) |
		BIT(FCR_RX_FIFO) | BIT(FCR_TX_FIFO));

	/* Enable THRE interrupt */
	ier_val = readl(uart_regs->IER);
	ier_val |= BIT(IER_THRE);
	writel(uart_regs->IER, ier_val);

	/* Enable transmission */
	tx_enable(uart_regs->TER);

	/* Enable interrupt */
	/*irq_enable(uart_dev->dev.irq);*/

	return 0;
}

static int lpc17xx_uart_stop_tx(int id)
{
	uint32_t ier_val;
	struct uart_device *uart_dev;
	struct uart_regs *uart_regs;

	if (id < UART_0 || id > UART_3)
		return -1;

	uart_dev = uart_get_dev(id);
	uart_regs = uart_get_regs(uart_dev);

	/* Disable interrupt */
	irq_disable(uart_dev->dev.irq);

	writel(uart_regs->FCR,
		BIT(FCR_RX_FIFO) | BIT(FCR_TX_FIFO));

	/* Disable transmission */
	tx_disable(uart_regs->TER);

	/* Disable THRE interrupt */
	ier_val = readl(uart_regs->IER);
	ier_val &= ~BIT(IER_THRE);
	writel(uart_regs->IER, ier_val);

	return 0;
}

static int lpc17xx_uart_irq_request(struct uart_device *uart_dev,
			int (*handler)(void *), void *arg, int flags)
{
	return irq_request(uart_dev->dev.irq, handler, arg, flags);
}

struct uart_ops lpc17xx_uart_ops = {
	.uart_enable =		lpc17xx_uart_enable,
	.uart_disable =		lpc17xx_uart_disable,
	.set_word_length =	lpc17xx_uart_set_word_length,
	.set_baud_rate =	lpc17xx_uart_set_baud_rate,
	.set_stop_bit =		lpc17xx_uart_set_stop_bit,
	.set_parity =		lpc17xx_uart_set_parity,
	.start_rx =			lpc17xx_uart_start_rx,
	.stop_rx =			lpc17xx_uart_stop_rx,
	.start_tx =			lpc17xx_uart_start_tx,
	.stop_tx =			lpc17xx_uart_stop_tx,
	.send_byte =		lpc17xx_uart_send_byte,
	.send_buffer =		lpc17xx_uart_send_buffer,
};

int lpc17xx_uart_init()
{
	int id;
	int nr_uart_devs;
	struct uart_device *uart_dev;

	nr_uart_devs = lpc17xx_uart_device_array.nr_uart_devs;
	for (id = 0; id < nr_uart_devs; id++) {
		uart_dev = uart_get_dev(id);
		uart_dev->id = id;
		atomic_init(&uart_dev->lock);
		if (uart_dev->dev.state == 1) {
			/* Mux UART pins */
			lpc17xx_uart_mux_pins(uart_dev);
			/* Request IRQ */
			lpc17xx_uart_irq_request(uart_dev, lpc17xx_uart_irq_handler,
					(void *)uart_dev, 0);
		}
	}

	return uart_init(&lpc17xx_uart_ops);
}
