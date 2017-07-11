#ifndef _UART_H_
#define _UART_H_

#include <_stdint.h>
#include <atomic.h>

/*
 * The UART data
 */
struct uart_data {
	uint8_t *data;
	uint32_t size;
	int complete;
	int cur_pos;
};

/*
 * UART operations
 */
struct uart_ops {
	int (*uart_enable)(int id);
	int (*uart_disable)(int id);
	int (*set_word_length)(int id, uint8_t word_length);
	int (*set_baud_rate)(int id, uint32_t baud_rate);
	int (*set_stop_bit)(int id, uint8_t stop_bit);
	int (*set_parity)(int id);
	int (*start_tx)(int id, struct uart_data *uart_data);
	int (*stop_tx)(int id);
	int (*start_rx)(int id, struct uart_data *uart_data);
	int (*stop_rx)(int id);
	int (*send_byte)(int id, uint8_t byte);
	int (*send_buffer)(int id, struct uart_data *uart_data);
};

int uart_init(struct uart_ops *);
int uart_enable(int id);
int uart_disable(int id);
int uart_set_word_length(int id, uint8_t word_length);
int uart_set_baud_rate(int id, uint32_t baud_rate);
int uart_set_stop_bit(int id, uint8_t stop_bit);
int uart_set_parity(int id);
int uart_start_tx(int id, struct uart_data *uart_data);
int uart_stop_tx(int id);
int uart_start_rx(int id, struct uart_data *uart_data);
int uart_stop_rx(int id);
int uart_send_byte(int id, uint8_t byte);
int uart_send_buffer(int id, struct uart_data *uart_data);

#endif /* _UART_H_ */
