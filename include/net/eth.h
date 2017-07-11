#ifndef _ETH_H_
#define _ETH_H_

#include <_stdint.h>

#define ETH_ALEN		6		/* octets in one ethernet addr */
#define ETH_HLEN		14		/* total octets in header */
#define ETH_ZLEN		60		/* min. octets in frame sans FCS */
#define ETH_DATA_LEN	1500	/* max. octets in payload */
#define ETH_MAX_FLEN	1536	/* Max. Ethernet Frame Size */

#pragma pack(1)

struct ethhdr {
	uint8_t h_dest[ETH_ALEN];
	uint8_t h_source[ETH_ALEN];
	uint16_t h_proto;
};

#define ETHERTYPE_ARP	0x0806
#define ETHERTYPE_IP	0x0800

void eth_packet_process(void);
int eth_packet_create(uint8_t *snd_pkt, uint8_t *rvc_pkt, int rvc_sz);
int eth_send(uint16_t h_proto, uint8_t *tx_data, int tx_data_sz);

#endif
