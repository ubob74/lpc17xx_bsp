#include <_stdlib.h>
#include <_stdint.h>
#include <icmp.h>
#include <ip.h>
#include <eth.h>
#include <protos.h>

static uint16_t seq = 1;

static void icmp_dump(struct icmphdr *icmphdr)
{
#if 0
	printf("ICMP packet dump:\r\n");
	printf("\ttype=%d\r\n", icmphdr->type);
	printf("\tcode=%d\r\n", icmphdr->code);
	printf("\tid=%d\r\n", icmphdr->un.echo.id);
	printf("\tseq=%d\r\n", icmphdr->un.echo.seq);
	printf("\ticmphdr->csum=%X\r\n", icmphdr->csum);
#endif
}

static int icmp_packet_create(uint8_t type, struct icmphdr *icmphdr_tx,
		struct icmphdr *icmphdr_rx, int rx_data_sz)
{
	uint16_t csum;
	int tx_data_sz = rx_data_sz;
	uint8_t *rx_data;
	uint8_t *tx_data;

	icmphdr_tx->type = type;
	icmphdr_tx->code = 0;
	icmphdr_tx->csum = 0;
	icmphdr_tx->un.echo.id = ICMP_ECHOID;
	icmphdr_tx->un.echo.seq = htons(seq);
	seq++;

	icmp_dump(icmphdr_rx);
	icmp_dump(icmphdr_tx);

	rx_data = (uint8_t *)icmphdr_rx;
	tx_data = (uint8_t *)icmphdr_tx;

	memcpy((void *)(tx_data + ICMP_HLEN),
		(void *)(rx_data + ICMP_HLEN),
			rx_data_sz - ICMP_HLEN);

	csum = ip_cksum((uint16_t *)icmphdr_tx, tx_data_sz);
	icmphdr_tx->csum = csum;

	return tx_data_sz;
}

static int icmp_echo_reply(struct icmphdr *icmphdr_rx, int rx_data_sz)
{
	int tx_data_sz;
	struct icmphdr *icmphdr_tx;
	uint8_t *tx_data = get_tx_buf();

	/* Point to ICMP header */
	icmphdr_tx = (struct icmphdr *)(tx_data + ETH_HLEN + IPV4_HDR_LEN);
	tx_data_sz = icmp_packet_create(ICMP_ECHOREPLY, icmphdr_tx,
			icmphdr_rx, rx_data_sz);

	ip_send(IPPROTO_ICMP, tx_data_sz);

	return 0;
}

static int icmp_rcv(uint8_t *rx_data, int rx_data_sz)
{
	struct icmphdr *icmphdr_rx = (struct icmphdr *)rx_data;

	switch (icmphdr_rx->type) {
	case ICMP_ECHO:
		return icmp_echo_reply(icmphdr_rx, rx_data_sz);
	default:
		break;
	}

	return 0;
}

static struct proto_cb proto_cb = {
	.process = icmp_rcv,
};

int icmp_init(void)
{
	return register_proto(IPPROTO_ICMP, &proto_cb);
}
