#include <_stdlib.h>
#include <_stdint.h>
#include <inet.h>
#include <arp.h>
#include <ip.h>
#include <icmp.h>
#include <eth.h>
#include <protos.h>

struct net_buf net_buf;
static char addr[INET_ADDRSTRLEN];

static in_addr_t local_ip;

uint32_t swab32(uint32_t val)
{
	uint32_t _val;

	if (val == 0)
		return val;

	*((uint8_t *)&_val) = *((uint8_t *)&val + 3);
	*((uint8_t *)&_val + 1) = *((uint8_t *)&val + 2);
	*((uint8_t *)&_val + 2) = *((uint8_t *)&val + 1);
	*((uint8_t *)&_val + 3) = *(uint8_t *)&val;

	return _val;
}

uint16_t swab16(uint16_t val)
{
	uint16_t _val;

	if (val == 0)
		return val;

	*((uint8_t *)&_val) = *((uint8_t *)&val + 1);
	*((uint8_t *)&_val + 1) = *(uint8_t *)&val;

	return _val;
}

in_addr_t inet_addr(const char *str_addr)
{
	int i = 0;
	in_addr_t s_addr = 0;
	uint8_t tmp;
	char *token, *o = (char *)str_addr;

	if (!str_addr)
		goto out;

	while ((token = strsep(&o, ".")) != NULL) {
		tmp = (uint8_t)atoi(token);
		*((uint8_t *)&s_addr + i++) = tmp;
	}

out:
	return s_addr;
}

char *inet_ntoa(struct in_addr in)
{
	bzero(addr, sizeof(addr));
	sprintf(&addr[0], "%d.%d.%d.%d",
		*(uint8_t *)&in.s_addr, *((uint8_t *)&in.s_addr + 1),
		*((uint8_t *)&in.s_addr + 2), *((uint8_t *)&in.s_addr + 3));

	return addr;
}

in_addr_t get_local_ip(void)
{
	return local_ip;
}

static void net_buf_init(void)
{
	net_buf.rx_buf = NULL;
	net_buf.rx_size = 0;
	net_buf.tx_buf = NULL;
	net_buf.tx_size = 0;
}

void set_local_ip(const char *ip_addr)
{
	char _local_ip[INET_ADDRSTRLEN];

	bzero(_local_ip, sizeof(_local_ip));
	memcpy(_local_ip, ip_addr, sizeof(_local_ip));
	local_ip = inet_addr(_local_ip);
}

/**
 * Return pointer to transmit buffer
 */
uint8_t *get_tx_buf(void)
{
	return net_buf.tx_buf;
}

/**
 * Return pointer to receive buffer
 */
uint8_t *get_rx_buf(void)
{
	return net_buf.rx_buf;
}

int net_init(void)
{
	set_local_ip("192.168.1.222");

	net_buf_init();

	protos_init();

	arp_init();
	ip_init();
	icmp_init();

	return 0;
}
