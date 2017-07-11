#ifndef _LPC17XX_PHY_H_
#define _LPC17XX_PHY_H_

#include <_stdint.h>

#define PHY_MAX_ADDR 32

/* MII Mgmt Command reg. bits */
#define MCMD_READ			BIT(0)	/* to perform a single read cycle */
#define MCMD_SCAN			BIT(1)	/* to perform a read cycle continuously */

/* MII Mgmt Address reg. bits */
#define MADR_REG_ADDR_SHIFT		0		/* 5-bit Register Address field of Mgmt cycle */
#define MADR_REG_ADDR_WIDTH		5
#define MADR_PHY_ADDR_SHIFT		8		/* 5-bit PHY Address field of Mgmt cycle */
#define MADR_PHY_ADDR_WIDTH		5

/* MII Mgmt indicators register bits */
#define MIND_BSY			BIT(0)	/* '1' indicates MII Mgmt is currently performing an MII Mgmt Read or Write cycle */

/* DP83848C PHY Registers */
#define MII_BMCR		0x00	/* Basic Mode Control Register */
#define MII_BMSR		0x01	/* Basic Mode Status Register */
#define MII_PHYSID1		0x02	/* PHY Identifier 1 */
#define MII_PHYSID2		0x03	/* PHY Identifier 2 */
#define MII_ANAR		0x04	/* Auto-Negotiation Advertisement */
#define MII_ANLPAR		0x05	/* Auto-Neg. Link Partner Abitily */
#define MII_ANER		0x06	/* Auto-Neg. Expansion Register */
#define MII_ANNPTR		0x07	/* Auto-Neg. Next Page TX */

/* PHY Extended Registers */
#define MII_STS			0x10	/* Status Register */
#define MII_MICR		0x11	/* MII Interrupt Control Register */
#define MII_MISR		0x12	/* MII Interrupt Status Register */
#define MII_FCSCR		0x14	/* False Carrier Sense Counter */
#define MII_RECR		0x15	/* Receive Error Counter */
#define MII_PCSR		0x16	/* PCS Sublayer Config. and Status */
#define MII_RBR			0x17	/* RMII and Bypass Register */
#define MII_LEDCR		0x18	/* LED Direct Control Register */
#define MII_PHYCR		0x19	/* PHY Control Register */
#define MII_10BTSCR		0x1A	/* 10Base-T Status/Control Register */
#define MII_CDCTRL1		0x1B	/* CD Test Control and BIST Extens. */
#define MII_EDCR		0x1D	/* Energy Detect Control Register */

/* PHY registers
 * - MCFG - MII Mgmt config. register
 * - MCMD - MII Mgmt command register
 * - MADR - MII Mgmt address register
 * - MWTD - MII Mgmt write data register
 * - MRDD - MII Mgmt read data register
 * - MIND - MII Mgmt indicators register
 */
struct phy_regs {
	uint32_t MCFG;
	uint32_t MCMD;
	uint32_t MADR;
	uint32_t MWTD;
	uint32_t MRDD;
	uint32_t MIND;
};

struct mii_bus {
	const char *name;
	int phy_id;
	int phy_addr;
	int (*read)(struct mii_bus *bus, uint32_t regnum);
	int (*write)(struct mii_bus *bus, uint32_t regnum, uint16_t val);
	int (*reset)(struct mii_bus *bus);
};

int lpc17xx_mii_init(struct mii_bus *mii);

#endif
