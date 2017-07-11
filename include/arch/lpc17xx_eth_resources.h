#ifndef _LPC17XX_ETH_RESOURCES_H_
#define _LPC17XX_ETH_RESOURCES_H_

#include <eth.h>

extern struct eth_device lpc17xx_eth_device;

#define TX		0
#define RX		1

#define DESC_SZ				8

#define NR_RX_STAT			4
#define RX_STAT_BASE		0x20080000

#define NR_RX_DESC			NR_RX_STAT
#define RX_DESC_BASE		(RX_STAT_BASE + NR_RX_STAT * DESC_SZ)

#define NR_RX_BUF 			NR_RX_DESC
#define RX_BUF_START_ADDR	(RX_DESC_BASE + NR_RX_DESC * DESC_SZ)
#define RX_BUF_SIZE			ETH_MAX_FLEN

#define NR_TX_STAT			NR_RX_STAT
#define TX_STAT_BASE		(RX_BUF_START_ADDR + NR_RX_BUF * RX_BUF_SIZE)

#define NR_TX_DESC			NR_TX_STAT
#define TX_DESC_BASE		(TX_STAT_BASE + NR_TX_STAT * 4)

#define NR_TX_BUF			NR_TX_DESC
#define TX_BUF_START_ADDR	(TX_DESC_BASE + NR_TX_DESC * DESC_SZ)
#define TX_BUF_SIZE			ETH_MAX_FLEN

#endif /* _LPC17XX_ETH_RESOURCES_H_ */
