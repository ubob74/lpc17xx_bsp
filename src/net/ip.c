#include <_stdlib.h>
#include <_stdint.h>
#include <inet.h>
#include <eth.h>
#include <ip.h>
#include <icmp.h>
#include <protos.h>
#include <arp.h>

void ip_packet_dump(struct iphdr *iphdr, unsigned int sz)
{
#if 0
	struct in_addr in;
	char *ip;

	printf("%s: IP packet size=%d\r\n", __func__, sz);

	printf("\tihl=%d (%d)\r\n", iphdr->ihl << 2, sizeof(*iphdr));
	printf("\tver=%d\r\n", iphdr->ver);
	printf("\ttot_len=%d\r\n", ntohs(iphdr->tot_len));
	printf("\tid=%d\r\n", ntohs(iphdr->id));
	printf("\tprotocol=%d\r\n", iphdr->protocol);
	printf("\ttos=%d\r\n", iphdr->tos);
	printf("\tfrag_off=%d\r\n", iphdr->frag_off);

	in.s_addr = iphdr->saddr;
	ip = inet_ntoa(in);
	printf("\tsaddr=%s\r\n", ip);

	in.s_addr = iphdr->daddr;
	ip = inet_ntoa(in);
	printf("\tdaddr=%s\r\n", ip);
#endif
}

/* Taken from Linux kernel */
uint16_t ip_cksum(unsigned short *ptr, int nbytes)
{
	register unsigned int sum;
	unsigned short oddbyte;
	register unsigned short answer;

	sum = 0;
	while (nbytes > 1) {
		sum += *ptr++;
		nbytes -= 2;
	}

	if (nbytes == 1) {
		oddbyte = 0;
		*((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
		sum += oddbyte;
	}

	sum = (sum >> 16) + (sum & 0xffff); /* add high-16 to low-16 */
	sum += (sum >> 16); /* add carry */
	answer = ~sum;

	return answer;
}

static int iphdr_create(int protocol, struct iphdr *iphdr_tx,
		struct iphdr *iphdr_rx, int tx_data_sz)
{
	uint16_t csum;

	iphdr_tx->ihl = 5;
	iphdr_tx->ver = IPV4;
	iphdr_tx->frag_off = 0;
	iphdr_tx->tos = 0;
	iphdr_tx->ttl = iphdr_rx->ttl;
	iphdr_tx->saddr = get_local_ip();
	iphdr_tx->daddr = iphdr_rx->saddr;
	iphdr_tx->protocol = protocol;
	iphdr_tx->id = iphdr_rx->id;
	iphdr_tx->tot_len = htons((iphdr_tx->ihl << 2) + tx_data_sz);
	iphdr_tx->check = 0;
	csum = ip_cksum((uint16_t *)iphdr_tx, iphdr_tx->ihl << 2);
	iphdr_tx->check = csum;

	return ntohs(iphdr_tx->tot_len);
}

int ip_send(uint8_t protocol, int tx_data_sz)
{
	int ip_size;
	uint8_t *tx_data = get_tx_buf();
	uint8_t *rx_data = get_rx_buf();
	struct iphdr *iphdr_tx = (struct iphdr *)(tx_data + ETH_HLEN);
	struct iphdr *iphdr_rx = (struct iphdr *)(rx_data + ETH_HLEN);

	ip_size = iphdr_create(protocol, iphdr_tx, iphdr_rx, tx_data_sz);

	return eth_send(ETHERTYPE_IP, tx_data, ip_size);
}

static int iphdr_check(struct iphdr *iphdr)
{
	/* TODO */
	return 0;
}

static int ip_rcv(uint8_t *rx_data, int rx_data_sz)
{
	int ret = 0;
	in_addr_t local_ip = get_local_ip();
	struct iphdr *iphdr = (struct iphdr *)rx_data;
	struct proto *p;

	if (local_ip != iphdr->daddr)
		return 0;

	ip_packet_dump(iphdr, rx_data_sz);

	p = lookup_proto(iphdr->protocol);
	if (!p)
		return 0;

	ret = iphdr_check(iphdr);
	if (ret)
		goto out;

	ret = p->proto_cb->process(rx_data + (iphdr->ihl << 2),
			rx_data_sz - (iphdr->ihl << 2));
out:
	return ret;
}

static int ip_hw_resolve(uint8_t *pkt_data)
{
	struct arp_cache_entry *entry;
	struct ethhdr *ethhdr = (struct ethhdr *)pkt_data;
	struct iphdr *iphdr_tx = (struct iphdr *)(pkt_data + ETH_HLEN);

	entry = arp_cache_find(iphdr_tx->daddr);
	if (!entry)
		return -1;

	memcpy(ethhdr->h_dest, entry->mac, ETH_ALEN);
	return 0;
}

struct proto_cb ip_proto_cb = {
	.process = ip_rcv,
	.hw_resolve = ip_hw_resolve,
};

int ip_init(void)
{
	return register_proto(ETHERTYPE_IP, &ip_proto_cb);
}
