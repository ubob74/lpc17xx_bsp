#ifndef _IP_H_
#define _IP_H_

#include <_stdint.h>
#include <inet.h>

#define IPV4	4
#define IPV4_HDR_LEN	20

#pragma pack(1)

struct iphdr {
	unsigned int ihl :4;
	unsigned int ver :4;
	uint8_t tos; /* type of service */
	uint16_t tot_len; /* total lenght */
	uint16_t id;
	uint16_t frag_off;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t check;
	in_addr_t saddr;
	in_addr_t daddr;
};

uint16_t ip_cksum(unsigned short *ptr, int nbytes);
int ip_send(uint8_t protocol, int tx_data_sz);
int ip_init(void);
#endif
