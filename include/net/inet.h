#ifndef _INET_H_
#define _INET_H_

#include <_stdint.h>

/* XXX.XXX.XXX.XXX */
#define INET_ADDRSTRLEN 16

enum {
	IPPROTO_IP = 0,
#define IPPROTO_IP		IPPROTO_IP
	IPPROTO_ICMP = 1,
#define IPPROTO_ICMP	IPPROTO_ICMP
	IPPROTO_TCP = 6,
#define IPPROTO_TCP		IPPROTO_TCP
};

typedef uint32_t in_addr_t;

struct in_addr {
	uint32_t s_addr;
};

struct net_ops {
	int (*open)(void);
	int (*close)(void);
	int (*xmit)(void);
};

struct net_buf {
	uint8_t *rx_buf;
	uint32_t rx_size;
	uint8_t *tx_buf;
	uint32_t tx_size;
	int tx_complete;
};

extern struct net_buf net_buf;

in_addr_t inet_addr(const char *p);
char *inet_ntoa(struct in_addr in);

uint32_t swab32(uint32_t val);
uint16_t swab16(uint16_t val);

#define htonl(hostlong)		swab32(hostlong)
#define htons(hostshort)	swab16(hostshort)
#define ntohl(netlong)		swab32(netlong)
#define ntohs(netshort)		swab16(netshort)

/*
uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);*/

void set_local_ip(const char *addr);
in_addr_t get_local_ip(void);
int net_init(void);

uint8_t *get_tx_buf(void);
uint8_t *get_rx_buf(void);

#endif
