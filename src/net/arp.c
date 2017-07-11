#include <_stdlib.h>
#include <_stdint.h>
#include <inet.h>
#include <eth.h>
#include <arp.h>
#include <protos.h>

#include <lpc17xx_eth.h>

static struct arp_cache arp_cache;

static void arp_packet_dump(uint8_t *data, int data_sz)
{
#if 0
	struct in_addr in;
	struct arphdr *arphdr;
	char *ip;

	arphdr = (struct arphdr *)data;

	printf("\r\n%s: ARP packet:\r\ndata=%X, size=%d\r\n", __func__, data, data_sz);
	printf("hw_size=%d proto_size=%d\r\n",
		__func__, arphdr->hw_size, arphdr->proto_size);

	in.s_addr = arphdr->sender_ip;
	ip = inet_ntoa(in);
	printf("Sender IP: %s\r\n", ip);

	in.s_addr = arphdr->target_ip;
	ip = inet_ntoa(in);
	printf("Target IP: %s\r\n", ip);

	printf("%s: ARP opcode: %d\r\n",
		__func__, ntohs(arphdr->opcode));

	printf("%s: ARP proto type: %X\r\n",
		__func__, ntohs(arphdr->proto_type));

	printf("%s: ARP proto size: %d\r\n",
		__func__, arphdr->proto_size);

	printf("%s: Sender HW: %x:%x:%x:%x:%x:%x\r\n",
		__func__, arphdr->sender_mac[0], arphdr->sender_mac[1],
			arphdr->sender_mac[2],
			arphdr->sender_mac[3], arphdr->sender_mac[4],
			arphdr->sender_mac[5]);
#endif
}

static int arp_reply_create(uint8_t *tx_data, uint8_t *rx_data)
{
	int tx_size;
	uint32_t local_ip = get_local_ip();
	struct arphdr *arphdr_tx; /* ARP packet to be sent */
	struct arphdr *arphdr_rx; /* received ARP packet */
	uint8_t *hw_addr = lpc17xx_eth_get_mac_addr();

	arphdr_tx = (struct arphdr *)tx_data;
	arphdr_rx = (struct arphdr *)rx_data;

	arphdr_tx->hw_type = arphdr_rx->hw_type;
	arphdr_tx->opcode = htons(ARPOP_REPLY);
	arphdr_tx->hw_size = ETH_ALEN;
	arphdr_tx->proto_type = htons(ETHERTYPE_IP);
	arphdr_tx->proto_size = sizeof(in_addr_t);
	memcpy(arphdr_tx->sender_mac, hw_addr, ETH_ALEN); /* local MAC */
	memcpy(arphdr_tx->target_mac, arphdr_rx->sender_mac, ETH_ALEN);
	arphdr_tx->sender_ip = local_ip;
	arphdr_tx->target_ip = arphdr_rx->sender_ip;

	tx_size = sizeof(*arphdr_tx);

	arp_packet_dump((uint8_t *)arphdr_tx, tx_size);

	return tx_size;
}

void arp_cache_dump(void)
{
#if 0
	int i;
	struct in_addr in;
	struct arp_cache_entry *entry;

	printf("\r\nARP cache:\r\n");
	for (i = 0; i < arp_cache.nr_entry; i++) {
		entry = arp_cache.entry + i;
		in.s_addr = entry->ip;
		printf("%d: ip=%s mac=%x:%x:%x:%x:%x:%x\r\n",
			i+1, inet_ntoa(in), entry->mac[0], entry->mac[1], entry->mac[2],
				entry->mac[3], entry->mac[4], entry->mac[5]);
	}
#endif
}

int arp_cache_add(in_addr_t ip, uint8_t *mac)
{
	struct arp_cache_entry *entry;

	if (arp_cache.nr_entry == NR_ARP_CACHE_ENTRY)
		return -1;

	entry = arp_cache.entry + arp_cache.nr_entry++;
	entry->ip = ip;
	memcpy(entry->mac, mac, ETH_ALEN);

	return 0;
}

struct arp_cache_entry *arp_cache_find(in_addr_t ip)
{
	int i;
	struct arp_cache_entry *entry;

	for (i = 0; i < arp_cache.nr_entry; i++) {
		entry = (struct arp_cache_entry *)arp_cache.entry + i;
		if (ip == entry->ip)
			return entry;
	}

	return NULL;
}

int arp_cache_del(in_addr_t ip)
{
	/* TODO */
	return 0;
}

void arp_cache_drop(void)
{
	/* TODO */
	return;
}

static void arp_cache_init(void)
{
	arp_cache.entry = (struct arp_cache_entry *)ARP_CACHE_START_ADDR;
	memset(arp_cache.entry, 0, NR_ARP_CACHE_ENTRY * sizeof(struct arp_cache_entry));
	arp_cache.nr_entry = 0;
}

static int arp_reply(uint8_t *rx_data, int rx_data_sz)
{
	uint8_t *tx_data = get_tx_buf();
	int tx_data_sz;

	tx_data_sz = arp_reply_create(tx_data + ETH_HLEN, rx_data);
	if (!tx_data_sz)
		return 0;

	return eth_send(ETHERTYPE_ARP, tx_data, tx_data_sz);
}

static int arp_rcv(uint8_t *rx_data, int rx_data_sz)
{
	int ret;
	uint32_t local_ip = get_local_ip();
	struct arphdr *arphdr = (struct arphdr *)rx_data;
	struct arp_cache_entry *entry;

	arp_packet_dump(rx_data, rx_data_sz);

	if (local_ip != arphdr->target_ip)
		return 0;

	entry = arp_cache_find(arphdr->sender_ip);
	if (!entry) {
		ret = arp_cache_add(arphdr->sender_ip, arphdr->sender_mac);
		if (ret < 0)
			return -1;
	}

	arp_cache_dump();

	if (arphdr->opcode == htons(ARPOP_REQUEST))
		arp_reply(rx_data, rx_data_sz);

	return 0;
}

/**
 * Lookup target MAC in ARP cache
 */
static int arp_hw_resolve(uint8_t *pkt_data)
{
	struct arp_cache_entry *entry;
	struct ethhdr *ethhdr = (struct ethhdr *)pkt_data;
	struct arphdr *arphdr = (struct arphdr *)(pkt_data + ETH_HLEN);

	entry = arp_cache_find(arphdr->target_ip);
	if (!entry)
		return -1;

	memcpy(ethhdr->h_dest, entry->mac, ETH_ALEN);
	return 0;
}

static struct proto_cb arp_proto_cb = {
	.process = arp_rcv,
	.hw_resolve = arp_hw_resolve,
};

int arp_init(void)
{
	arp_cache_init();
	register_proto(ETHERTYPE_ARP, &arp_proto_cb);

	return 0;
}
