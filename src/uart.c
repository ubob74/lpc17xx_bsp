#include <_stdint.h>
#include <uart.h>
#include <io.h>

static struct uart_ops *uart_ops = NULL;

int uart_init(struct uart_ops *arch_uart_ops)
{
	if (!arch_uart_ops)
		return -1;

	uart_ops = arch_uart_ops;

	return 0;
}

int uart_enable(int id)
{
	return (uart_ops && uart_ops->uart_enable)
		? uart_ops->uart_enable(id) : -1;
}

int uart_disable(int id)
{
	return (uart_ops && uart_ops->uart_disable)
		? uart_ops->uart_disable(id) : -1;
}

int uart_set_word_length(int id, uint8_t word_length)
{
	return (uart_ops && uart_ops->set_word_length)
		? uart_ops->set_word_length(id, word_length) : -1;
}

int uart_set_baud_rate(int id, uint32_t baud_rate)
{
	return (uart_ops && uart_ops->set_word_length)
		? uart_ops->set_baud_rate(id, baud_rate) : -1;
}

int uart_set_stop_bit(int id, uint8_t stop_bit)
{
	return (uart_ops && uart_ops->set_stop_bit)
		? uart_ops->set_stop_bit(id, stop_bit) : -1;
}

int uart_set_parity(int id)
{
	return (uart_ops && uart_ops->set_parity)
		? uart_ops->set_parity(id) : -1;
}

int uart_start_tx(int id, struct uart_data *uart_data)
{
	return (uart_ops && uart_ops->start_tx)
		? uart_ops->start_tx(id, uart_data) : -1;
}

int uart_stop_tx(int id)
{
	return (uart_ops && uart_ops->stop_tx)
		? uart_ops->stop_tx(id) : -1;
}

int uart_start_rx(int id, struct uart_data *uart_data)
{
	return (uart_ops && uart_ops->start_rx)
		? uart_ops->start_rx(id, uart_data) : -1;
}

int uart_stop_rx(int id)
{
	return (uart_ops && uart_ops->stop_rx)
		? uart_ops->stop_rx(id) : -1;
}

int uart_send_byte(int id, uint8_t byte)
{
	return (uart_ops && uart_ops->send_byte)
		? uart_ops->send_byte(id, byte) : -1;
}

int uart_send_buffer(int id, struct uart_data *uart_data)
{
	return (uart_ops && uart_ops->send_buffer)
		? uart_ops->send_buffer(id, uart_data) : -1;
}
