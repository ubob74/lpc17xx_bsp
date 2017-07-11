#ifndef _LPC17XX_ETH_H_
#define _LPC17XX_ETH_H_

#include <_stdint.h>
#include <atomic.h>
#include <io.h>
#include <resources.h>

/**
 * Ethernet pins:
 * - ENET_TX_EN - transmit data enable
 * - ENET_TXD - transmit data, 2 bits
 * - ENET_RXD - receive data, 2 bits
 * - ENET_RX_ER - receive error
 * - ENET_CRS - carrier sense/data valid
 * - ENET_REF_CLK/ENET_RX_CLK - reference clock
 * - ENET_MDC - MIIM clock
 * - ENET_MDIO - MI data input/output
 */

/**
 * Ethernet registers
 * - MAC1 - MAC config. register 1
 * - MAC2 - MAC config. register 2
 * - IPGT - Back-to-back inter packet gap register
 * - IPGR - non Back-to-back inter packet gap register
 * - MAXF - Maximum frame register
 * - SUPP - PHY support register
 * - SA - station address
 * - CMD - command register
 * - STAT - status
 * - RX_DESC - receive descriptor base address
 * - RX_STAT - receive status base address
 * - RX_DESC_NUM - receive number of decriptors register
 * - RX_PI - receive produce index
 * - RX_CI - receive consume index
 * - TX_DESC - transmit descriptor base address register
 * - TX_STAT - transmit status base address reg.
 * - TX_DESC_NUM - transmit number of descriptors reg.
 * - TX_PI - transmit produce index reg.
 * - TX_CI - transmit consume index reg.
 * - TSV - transmit status vector reg.
 * - RSV - receive status vector reg.
 * - FCC - flow control counter reg.
 * - FCS - flow control status reg.
 * - INT_STAT - interrupt status reg.
 * - INT_EN - interrupt enable reg.
 * - INT_CLR - interrupt clear reg.
 * - INT_SET - interrupt set reg.
 * - PWR_DOWN - power-down reg.
 */
struct eth_regs {
	uint32_t MAC1;
	uint32_t MAC2;
	uint32_t IPGT;
	uint32_t IPGR;
	uint32_t CLRT;
	uint32_t MAXF;
	uint32_t SUPP;
	uint32_t TEST;
	uint32_t SA0;
	uint32_t SA1;
	uint32_t SA2;
	uint32_t CMD;
	uint32_t STAT;
	uint32_t RX_DESC;
	uint32_t RX_STAT;
	uint32_t RX_DESC_NUM;
	uint32_t RX_PI;
	uint32_t RX_CI;
	uint32_t TX_DESC;
	uint32_t TX_STAT;
	uint32_t TX_DESC_NUM;
	uint32_t TX_PI;
	uint32_t TX_CI;
	uint32_t TSV0;
	uint32_t TSV1;
	uint32_t RSV;
	uint32_t FCC;
	uint32_t FCS;
	uint32_t INT_STAT;
	uint32_t INT_EN;
	uint32_t INT_CLR;
	uint32_t INT_SET;
	uint32_t PWR_DOWN;
};

/* MAC1 register bits */
#define MAC1_RX_EN			BIT(0)	/* receive enable */
#define MAC1_PASS_ALL		BIT(1)	/* pass all received frame */
#define MAC1_RX_FLOW_CTRL	BIT(2)	/* PAUSE Flow Control Frame */
#define MAC1_TX_FLOW_CTRL	BIT(3)	/* PAUSE Flow Control Frame */
#define MAC1_LOOPBACK		BIT(4)
#define MAC1_TX_RST			BIT(8)	/* put the Transmit function logic to reset */
#define MAC1_MSC_TX_RST		BIT(9)	/* resets the MAC Control Sublayer / Transmit logic */
#define MAC1_RX_RST			BIT(10)	/* put Ethernet receive logic in reset */
#define MAC1_MSC_RX_RST		BIT(11)	/* resets MAC Control Sublayer / Receive logic */
#define MAC1_SIM_RST		BIT(14)	/* reset to the random number generator within the Transmit Function */
#define MAC1_SOFT_RST		BIT(15)	/* soft reset - put all modules within the MAC in reset */

/* MAC Configuration Register 2 */
#define MAC2_FULL_DUP		BIT(0)	/* Full Duplex Mode */
#define MAC2_FRM_LEN_CHK	BIT(1)	/* Frame Length Checking */
#define MAC2_HUGE_FRM_EN	BIT(2)	/* Huge Frame Enable */
#define MAC2_DLY_CRC		BIT(3)	/* Delayed CRC Mode */
#define MAC2_CRC_EN			BIT(4)	/* Append CRC to every Frame */
#define MAC2_PAD_EN			BIT(5)	/* Pad all Short Frames */
#define MAC2_VLAN_PAD_EN	BIT(6)	/* VLAN Pad Enable */
#define MAC2_ADET_PAD_EN	BIT(7)	/* Auto Detect Pad Enable */
#define MAC2_PPREAM_ENF		BIT(8)	/* Pure Preamble Enforcement */
#define MAC2_LPREAM_ENF		BIT(9)	/* Long Preamble Enforcement */
#define MAC2_NO_BACKOFF		BIT(12)	/* No Backoff Algorithm */
#define MAC2_BACK_PRESSURE	BIT(13)	/* Backoff Presurre / No Backoff */
#define MAC2_EXCESS_DEF		BIT(14)	/* Excess Defer */

/* Back-to-Back Inter-Packet-Gap Register */
#define IPGT_FULL_DUP		0x00000015	/* Recommended value for Full Duplex */
#define IPGT_HALF_DUP		0x00000012	/* Recommended value for Half Duplex */

/* Non Back-to-Back Inter-Packet-Gap Register */
#define IPGR_DEF			0x00000012	/* Recommended value */

/* Collision Window/Retry Register */
#define CLRT_DEF			0x0000370F	/* Default value */

/* PHY Support Register */
#define SUPP_SPEED			BIT(8)	/* Reduced MII Logic Current Speed */

/* Command register bits */
#define CMD_RX_EN			BIT(0)	/* enable receive */
#define CMD_TX_EN			BIT(1)	/* enable transmit */
#define CMD_REG_RST			BIT(3)	/* all datapaths and the host registers are reset.The MAC needs to be reset separetly */
#define CMD_TX_RST			BIT(4)	/* the transmit datapath is reset */
#define CMD_RX_RST			BIT(5)	/* the receive datapath is reset */
#define CMD_PASS_RUNT_FRAME	BIT(6)	/* pass runt frames */
#define CMD_PASS_RX_FILTER	BIT(7)	/* enable/disable receive filtering */
#define CMD_TX_FLOW_CTRL	BIT(8)	/* enable sending pause frame in FD and continuous preamble in half duplex */
#define CMD_RMII			BIT(9)	/* RMII mode is selected */
#define CMD_FULL_DUPLEX		BIT(10)	/* full duplex operation */

/* Back-to-Back Inter-Packet-Gap Register */
#define IPGT_FULL_DUP	0x00000015	/* Recommended value for Full Duplex */
#define IPGT_HALF_DUP	0x00000012	/* Recommended value for Half Duplex */

/* Non Back-to-Back Inter-Packet-Gap Register recommended value */
#define IPGR_DEF		0x00000012

/* Collision Window/Retry Register default value */
#define CLRT_DEF		0x0000370F

#define MAC_1			0x1E		/* Ethernet (MAC) address */
#define MAC_2			0x70		/* (MUST be unique in LAN!) */
#define MAC_3			0x2C
#define MAC_4			0xA2
#define MAC_5			0x45
#define MAC_6			0x5D

/* Interrupt status register bits */
#define RX_OVERRUN		BIT(0) /* fatal overrun error in receive queue */
#define RX_ERROR		BIT(1) /* receive error */
#define RX_FINISHED		BIT(2) /* all receive descriptors have been processed, */
								/* i.e. produce index == consume index */
#define RX_DONE			BIT(3) /* receive descriptor has been processed */
#define TX_UNDERRUN		BIT(4) /**/
#define TX_ERROR		BIT(5) /**/
#define TX_FINISHED		BIT(6) /**/
#define TX_DONE			BIT(7) /* transmit descriptor has been processed */

struct mii_bus;
struct eth_dev_ops;

struct eth_device {
	uint8_t enable_bit;
	struct mii_bus *mii;
	struct eth_dev_ops *ops;
	struct device dev;
};

struct eth_device_array {
	struct eth_device *eth_dev;
	int nr_eth_dev;
};

#pragma pack(1)

/* RX/TX descriptor format */
struct rx_desc {
	uint32_t addr;
	uint32_t ctrl;
};

struct tx_desc {
	uint32_t addr;
	union {
		uint32_t ctrl;
		struct {
			int size :11;
			int unused :15;
			int override :1;
			int huge :1;
			int pad :1;
			int crc :1;
			int last :1;
			int interrupt :1;
		} _ctrl;
	} ctrl;
};

/* RX status descriptor */
struct rx_stat {
	union {
		uint32_t info;
		struct {
			int size :11;
			int unused :7;
			int ctrl_frame :1;
			int vlan :1;
			int multicast :1;
			int broadcast :1;
			int crc_error :1;
			int symbol_err :1;
			int length_err :1;
			int range_err :1;
			int align_err :1;
			int overrun :1;
			int no_desc :1;
			int last_flag :1;
			int error :1;
		} _info;
	} info;
	uint32_t crc;
};

/* TX status descriptor */
struct tx_stat {
	uint32_t info;
};

int lpc17xx_eth_init(void);
uint8_t *lpc17xx_eth_get_mac_addr(void);
void lpc17xx_eth_rx_enable(void);
void lpc17xx_eth_rx_disable(void);
void lpc17xx_eth_tx_enable(void);
void lpc17xx_eth_tx_disable(void);
int lpc17xx_eth_transmit_packet(void);

#endif
