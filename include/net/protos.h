#ifndef _PROTOS_H_
#define _PROTOS_H_

#include <_stdint.h>

/**
 * How many protocols we support:
 * - ARP
 * - IP
 * - ICMP
 * - UDP (TODO)
 */
#define NR_PROTOS	4

#pragma pack(1)

struct proto_cb {
	int (*process)(uint8_t *data, int data_sz);
	int (*hw_resolve)(uint8_t *data);
};

struct proto {
	uint16_t id;
	struct proto_cb *proto_cb;
};

struct protos {
	struct proto proto[NR_PROTOS];
	int nr_proto;
};

void protos_init(void);
int register_proto(int id, struct proto_cb *);
struct proto *lookup_proto(uint16_t id);

#endif
