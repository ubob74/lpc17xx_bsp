#include <_stdlib.h>
#include <_stdint.h>
#include <io.h>
#include <resources.h>
#include <lpc17xx_eth.h>
#include <lpc17xx_phy.h>

static struct phy_regs phy_regs = {
	.MCFG =		0x50000020,
	.MCMD =		0x50000024,
	.MADR =		0x50000028,
	.MWTD =		0x5000002C,
	.MRDD =		0x50000030,
	.MIND =		0x50000034,
};

static int lpc17xx_mii_read(struct mii_bus *mii, uint32_t regnum)
{
	uint32_t regval;
	int phy_addr = mii->phy_addr;

	regval = (phy_addr << MADR_PHY_ADDR_SHIFT) | regnum;
	/*printf("DEBUG: %s: regval=0x%X\r\n", __func__, regval);*/
	writel(phy_regs.MADR, regval);
	writel(phy_regs.MCMD, MCMD_READ);

	for (;;) {
		regval = readl(phy_regs.MIND);
		if (!(regval & MIND_BSY))
			break;
	}

	writel(phy_regs.MCMD, 0);
	regval = readl(phy_regs.MRDD);
	/*printf("DEBUG: regval=0x%X\r\n", regval);*/
	return (int)regval;
}

static int lpc17xx_mii_write(struct mii_bus *mii,
		uint32_t regnum, uint16_t val)
{
	uint32_t regval;
	int phy_addr = mii->phy_addr;

	writel(phy_regs.MCMD, 0);
	writel(phy_regs.MADR, (phy_addr << MADR_PHY_ADDR_SHIFT) | regnum);
	writel(phy_regs.MWTD, (uint32_t)val);

	for (;;) {
		regval = readl(phy_regs.MIND);
		if (!(regval & MIND_BSY))
			break;
	}

	return 0;
}

static int lpc17xx_mii_reset(struct mii_bus *mii)
{
	/* TODO */
	return 0;
}

static int mdiobus_scan(struct mii_bus *mii)
{
	int phy_reg, phy_id;

	phy_reg = mii->read(mii, MII_PHYSID1);
	if (phy_reg < 0)
		return -1;

	phy_id = (phy_reg & 0xFFFF) << 16;

	phy_reg = mii->read(mii, MII_PHYSID2);
	if (phy_reg < 0)
		return -1;

	phy_id |= (phy_reg & 0xFFFF);

	return phy_id;
}

/* RMII initialization */
int lpc17xx_mii_init(struct mii_bus *mii)
{
	int i, phy_id;

	mii->read = lpc17xx_mii_read;
	mii->write = lpc17xx_mii_write;
	mii->reset = lpc17xx_mii_reset;

	for (i = 0, phy_id = 0; i < PHY_MAX_ADDR; i++) {
		mii->phy_addr = i;
		phy_id = mdiobus_scan(mii);
		if ((phy_id & 0x1FFFFFFF) != 0x1FFFFFFF) {
			printf("DEBUG: %s: PHY id=0x%X, addr=0x%X\r\n",
				__func__, phy_id, i);
			mii->phy_id = phy_id;
			return phy_id;
		}
	}

	return 0;
}
