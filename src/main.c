#include <io.h>
#include <atomic.h>
#include <irq.h>

#include <_stdlib.h>
#include <inet.h>
#include <lpc17xx_prcm.h>
#include <lpc17xx_irq.h>
#include <lpc17xx_uart.h>
#include <lpc17xx_eth.h>
#include <lpc17xx_ssp.h>

#include <eth.h>
#include <uart.h>

#define UART2_BPS 57600

static int lpc17xx_prcm_pll0_test()
{
	int ret = -1;
	uint16_t m = 0;
	uint8_t n = 0;

	/* Read parameters */
	m = lpc17xx_prcm_pll0_read_mul_value();
	if (m != 12)
		goto err;

	n = lpc17xx_prcm_pll0_read_div_value();
	if (n != 1)
		goto err;

	ret = 0;

err:
	return ret;
}

static int lpc17xx_uart2_test(void)
{
	uint8_t data[] = "DEBUG: UART2 test for LPC17xx\r\n";
	struct uart_data uart_data;

	uart_data.data = &data[0];
	uart_data.size = ARRAY_SIZE(data);

	if (uart_enable(UART_2) < 0)
		return -1;

	if (uart_set_word_length(UART_2, 7) < 0)
		return -1;

	if (uart_set_stop_bit(UART_2, 1) < 0)
		return -1;

	if (uart_set_baud_rate(UART_2, UART2_BPS) < 0)
		return -1;

	if (uart_start_tx(UART_2, NULL) < 0)
		return -1;

	uart_send_buffer(UART_2, &uart_data);

	return 0;
}

static int string_test(void)
{
	int ret;
	int i = 10;
	char test_str[] = "test string";
	char data[] = "test:test1:test2:test3";
	char s[16];
	char *token, *o = data;

	printf("%s %d | 0x%x | 0x%X | (end of %s)\r\n",
		test_str, i, i, i+1, test_str);

	while ((token = strsep(&o, ":")) != NULL) {
		if (!*token)
			break;
		printf("token=%s o=%s\r\n", token, o);
	}

	ret = atoi("192");
	printf("ret=%d\r\n", ret);
	ret = atoi("849");
	printf("ret=%d\r\n", ret);
	ret = atoi("8475387");
	printf("ret=%d\r\n", ret);
	ret = atoi("1");
	printf("ret=%d\r\n", ret);

	bzero(s, sizeof(s));
	token = itoa(192456, s);
	printf("s=%s\r\n", token);
	bzero(s, sizeof(s));
	token = itoa(23, s);
	printf("s=%s\r\n", token);

	return 0;
}

static int inet_test(void)
{
	in_addr_t s_addr;
	struct in_addr in_addr;
	char *addr;
	char str_addr[] = "192.168.1.1";

	in_addr.s_addr = 0xC0A8C884;
	addr = inet_ntoa(in_addr);
	printf("addr=%s (%X)\r\n", addr, in_addr.s_addr);

	in_addr.s_addr = 0x01020304;
	addr = inet_ntoa(in_addr);
	printf("addr=%s (%X)\r\n", addr, in_addr.s_addr);

	in_addr.s_addr = 0x10203040;
	addr = inet_ntoa(in_addr);
	printf("addr=%s (%X)\r\n", addr, in_addr.s_addr);

	s_addr = inet_addr(str_addr);
	printf("s_addr=%X\r\n", s_addr);
	in_addr.s_addr = s_addr;
	addr = inet_ntoa(in_addr);
	printf("addr=%s\r\n", addr);

	return 0;
}

int main()
{
	uint8_t cclk_div, usb_div;

	if (lpc17xx_irq_table_init() < 0)
		goto out;

	/* Setup CCLK = 72 MHz, PLL0CLK = 288 MHz */
	/* Set CPU and USB clock dividers */
	cclk_div = 4;
	usb_div = 6;
	if (lpc17xx_prcm_pll0_init(cclk_div, usb_div) < 0)
		goto out;

	if (lpc17xx_prcm_pll0_test() < 0)
		goto out;

	if (lpc17xx_uart_init() < 0)
		goto out;

	lpc17xx_uart2_test();

	net_init();

	string_test();
	inet_test();

//	lpc17xx_eth_init();

/*	eth_packet_process();*/
	lpc17xx_ssp0_init();
out:
	while(1);
}
