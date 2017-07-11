#ifndef _ARP_H_
#define _ARP_H_

#include <_stdint.h>
#include <eth.h>
#include <inet.h>

#define ARPOP_REQUEST	1
#define ARPOP_REPLY		2

#pragma pack(1)

struct arphdr {
	uint16_t hw_type;
	uint16_t proto_type;
	uint8_t hw_size;
	uint8_t proto_size;
	uint16_t opcode;
	uint8_t sender_mac[ETH_ALEN];
	in_addr_t sender_ip;
	uint8_t target_mac[ETH_ALEN];
	in_addr_t target_ip;
};

#define ARP_CACHE_START_ADDR	0x20083100
#define NR_ARP_CACHE_ENTRY		5

struct arp_cache_entry {
	in_addr_t ip;
	uint8_t mac[ETH_ALEN];
};

struct arp_cache {
	struct arp_cache_entry *entry;
	int nr_entry;
};

/* ARP cache API */
int arp_init(void);
int arp_cache_add(in_addr_t ip, uint8_t *mac);
struct arp_cache_entry *arp_cache_find(in_addr_t ip);
void arp_cache_dump(void);
int arp_cache_del(in_addr_t ip);
void arp_cache_drop(void);

#endif
