#include <_stdint.h>
#include <_stdlib.h>
#include <eth.h>
#include <inet.h>
#include <arp.h>
#include <ip.h>
#include <protos.h>

#include <lpc17xx_eth.h>

void eth_packet_dump(struct ethhdr *ethhdr)
{
#if 0
	int i;

	printf("Dest. hw address: ");
	for (i = 0; i < ETH_ALEN; i++)
		printf("%x ", ethhdr->h_dest[i]);
	printf("\r\n");

	printf("Source hw address: ");
	for (i = 0; i < ETH_ALEN; i++)
		printf("%x ", ethhdr->h_source[i]);
	printf("\r\n");

	printf("hw proto=%X\r\n", ntohs(ethhdr->h_proto));
#endif
}

static int eth_check_hw_addr(struct ethhdr *ethhdr)
{
	/* Check HW address. It should be either broadcast or local MAC */
	if (*((uint32_t *)&ethhdr->h_dest[0]) != ~0x0) {
		if (*((uint32_t *)&ethhdr->h_dest[0]) != 0xa22c701e)
			return -1;
	}

	return 0;
}

void eth_packet_process(void)
{
	int ret;
	struct ethhdr *ethhdr;
	struct proto *p;

	for (;;) {
		/* waiting for incoming packet */
		while (!net_buf.rx_size);

		lpc17xx_eth_rx_disable();

		ethhdr = (struct ethhdr *)net_buf.rx_buf;
		ret = eth_check_hw_addr(ethhdr);
		if (ret < 0)
			goto next;

		/*printf("%s: received %d bytes\r\n", __func__, net_buf.rx_size);*/

		p = lookup_proto(ntohs(ethhdr->h_proto));
		if (!p)
			goto next;

		p->proto_cb->process(net_buf.rx_buf + ETH_HLEN,
				net_buf.rx_size - ETH_HLEN);
next:
		net_buf.rx_size = 0;
		lpc17xx_eth_rx_enable();
	}
}

int eth_send(uint16_t h_proto, uint8_t *tx_data, int tx_data_sz)
{
	int ret;
	uint8_t *hw_addr = lpc17xx_eth_get_mac_addr();
	struct proto *p = NULL;
	struct ethhdr *ethhdr = (struct ethhdr *)tx_data;

	p = lookup_proto(h_proto);
	if (!p)
		return -1;

	/* Put pointer at the beginnig of the packet to set target MAC */
	if (p->proto_cb->hw_resolve != NULL) {
		ret = p->proto_cb->hw_resolve(tx_data);
		if (ret < 0)
			return -1;
	}

	/* Set source MAC */
	memcpy(ethhdr->h_source, hw_addr, ETH_ALEN);
	ethhdr->h_proto = htons(h_proto);

	net_buf.tx_size = tx_data_sz + ETH_HLEN;

	eth_packet_dump((struct ethhdr *)tx_data);

	lpc17xx_eth_transmit_packet();

	/* Waiting for TX_DONE interrupt */
	while (!net_buf.tx_complete);

	/* Drop TX_complete flag */
	net_buf.tx_complete = 0;

	return 0;
}
