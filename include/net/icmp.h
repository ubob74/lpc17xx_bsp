#ifndef _ICMP_H_
#define _ICMP_H_

#include <_stdint.h>

#define ICMP_HLEN	4

struct icmphdr {
	uint8_t type;
	uint8_t code;
	uint16_t csum;
	union {
		struct {
			uint16_t id;
			uint16_t seq;
		} echo; /* echo datagram */
		uint32_t gw_addr; /* gateway address */
		struct {
			uint16_t res;
			uint16_t mtu;
		} frag; /* path mtu discovery */
	} un;
};

/* Supported types */
#define ICMP_ECHOREPLY	0 /* Echo Reply */
#define ICMP_ECHO		8 /* Echo Request */
#define ICMP_ECHOID		0x4433

unsigned int icmp_send(uint8_t *in, unsigned int in_sz, uint8_t *out);
int icmp_init(void);

#endif
