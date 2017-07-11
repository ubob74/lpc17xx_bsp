#include <_stdlib.h>
#include <_stdint.h>
#include <io.h>
#include <irq.h>
#include <resources.h>
#include <lpc17xx_eth.h>
#include <lpc17xx_eth_resources.h>
#include <lpc17xx_prcm.h>
#include <lpc17xx_irq.h>
#include <lpc17xx_phy.h>

#include <eth.h>
#include <inet.h>

static struct mii_bus mii_bus;
static struct eth_device *eth_dev = &lpc17xx_eth_device;

static uint8_t hw_addr[ETH_ALEN] = {MAC_1, MAC_2, MAC_3, MAC_4, MAC_5, MAC_6};

static struct rx_desc *rx_desc_array = (struct rx_desc *)RX_DESC_BASE;
static struct rx_stat *rx_stat_array = (struct rx_stat *)RX_STAT_BASE;

static struct tx_desc *tx_desc_array = (struct tx_desc *)TX_DESC_BASE;
static struct tx_stat *tx_stat_array = (struct tx_stat *)TX_STAT_BASE;

static inline void *eth_dev_get_regs(struct eth_device *eth_dev)
{
	return dev_get_regs(&eth_dev->dev);
}

static inline void lpc17xx_eth_mux_pins()
{
	dev_mux_pins(&eth_dev->dev);
}

static inline struct rx_desc *lpc17xx_eth_get_rx_desc(uint32_t idx)
{
	return rx_desc_array + idx;
}

static inline struct rx_stat *lpc17xx_eth_get_rx_stat(uint32_t idx)
{
	return rx_stat_array + idx;
}

static inline struct tx_desc *lpc17xx_eth_get_tx_desc(uint32_t idx)
{
	return tx_desc_array + idx;
}

#if 0
static inline struct tx_stat *lpc17xx_eth_get_tx_stat(uint32_t idx)
{
	return tx_stat_array + idx;
}
#endif

/* Set PHY operation mode - 100 Mbit, Half-Duplex */
static int lpc17xx_mii_set_op_mode(void)
{
	int ret;
	int phy_reg;
	struct mii_bus *mii = eth_dev->mii;

	/* Set PHY 100 MBit */
	phy_reg = mii->read(mii, MII_BMCR);
	phy_reg = mii->read(mii, MII_BMSR);
	printf("%s: BMSR: %X\r\n", __func__, phy_reg);

	/* Speed Select 100 Mb/s and Half Duplex operation */
	phy_reg = 0x2000;
	mii->write(mii, MII_BMCR, phy_reg);

	phy_reg = mii->read(mii, MII_BMCR);
	if (phy_reg != 0x2000) {
		printf("%s: RMII init error, phy_reg=0x%X\r\n", __func__, phy_reg);
		ret = -1;
	}

	return ret;
}

static int lpc17xx_check_link_status(void)
{
	int ret = -1;
	int phy_reg;
	struct mii_bus *mii = eth_dev->mii;

	/* Check Link status */
	/* TODO set timer */
	for (;;) {
		phy_reg = mii->read(mii, MII_STS);
		if (phy_reg & BIT(0)) {
			ret = 0;
			break;
		}
	}
	printf("link is up\r\n");
	return ret;
}

static void lpc17xx_mac_core_init()
{
	uint32_t regval;
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);

	/* Pass all received frames, bit 1 in MAC1 */
	regval = readl(regs->MAC1);
	regval |= MAC1_PASS_ALL;
	/*writel(regs->MAC1, regval);*/

	/* Append CRC to every Frame */
	regval = readl(regs->MAC2);
	/*regval |= MAC2_CRC_EN; - don't set*/
	regval |= MAC2_PAD_EN;
	writel(regs->MAC2, regval);
}

static void lpc17xx_eth_set_rmii_mode(void)
{
	uint32_t regval;
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);

	/* Enable RMII */
	regval = readl(regs->CMD);
	regval |= CMD_RMII;
	writel(regs->CMD, regval);
}

uint8_t *lpc17xx_eth_get_mac_addr(void)
{
	return hw_addr;
}

static void lpc17xx_eth_set_mac_addr(struct eth_regs *regs)
{
	writel(regs->SA0, ((MAC_1 << 8) | MAC_2));
	writel(regs->SA1, ((MAC_3 << 8) | MAC_4));
	writel(regs->SA2, ((MAC_5 << 8) | MAC_6));
}

static void lpc17xx_eth_select_speed(struct eth_regs *regs)
{
	uint32_t regval;

	/* Set IPGT value for Half-duplex mode */
	writel(regs->IPGT, IPGT_HALF_DUP);

	/* Set IPGR value for Half-duplex mode */
	writel(regs->IPGR, IPGR_DEF);

	/* Select PHY 100 MBit */
	regval = readl(regs->SUPP);
	regval |= SUPP_SPEED;
	writel(regs->SUPP, regval);
}

static void lpc17xx_eth_soft_reset(void)
{
	int tout;
	uint32_t regval;
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);

	/* Do full soft reset:
	 * - set the 'Soft Reset' bit in the MAC1 reg. to 1
	 * - set the RegReset bit in the Command reg. to 1
	 */
	writel(regs->MAC1, MAC1_SOFT_RST);
	writel(regs->CMD, CMD_REG_RST);
	for (tout = 300; tout; tout--);

	/* reset the 'Soft Reset' bit in the MAC1 reg. to 0 */
	regval = readl(regs->MAC1);
	regval &= ~MAC1_SOFT_RST;
	writel(regs->MAC1, regval);
}

/**
 * Update RX customer index
 */
static uint16_t lpc17xx_eth_rx_ci_update(struct eth_regs *regs)
{
	uint32_t c_idx, curr;

	c_idx = readl(regs->RX_CI);
	curr = c_idx &= 0xFFFF;
	if (++c_idx == NR_RX_DESC)
		c_idx = 0;
	writel(regs->RX_CI, c_idx);

	return curr;
}

/**
 * Update TX producer index
 */
static uint16_t lpc17xx_eth_tx_pi_update(struct eth_regs *regs)
{
	uint32_t p_idx, curr;

	p_idx = readl(regs->TX_PI);
	curr = p_idx &= 0xFFFF;
	if (++p_idx == NR_TX_DESC) {
		/*printf("%s: rotate TX producer idx: %d\r\n", __func__, p_idx);*/
		p_idx = 0;
	}
	writel(regs->TX_PI, p_idx);

	return curr;
}

#if 0
static void lpc17xx_eth_dump_rx_stat(struct rx_stat *rx_stat)
{
	printf("%s: rx stat _info: size=%d (0x%x)\r\n", __func__,
		rx_stat->info._info.size + 1, rx_stat->info._info.size + 1);
	if (rx_stat->info._info.error) {
		printf("%s: crc=%d symb=%d len=%d range=%d align=%d over=%d\r\n",
			__func__, rx_stat->info._info.error, rx_stat->info._info.symbol_err,
			rx_stat->info._info.length_err, rx_stat->info._info.range_err,
			rx_stat->info._info.align_err, rx_stat->info._info.overrun);
	}
}
#endif

static void lpc17xx_eth_process_rx_packet(struct eth_device *eth_dev)
{
	int c_idx, p_idx;
	struct rx_desc *rx_desc;
	struct tx_desc *tx_desc;
	struct rx_stat *rx_stat;
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);

	/* Read producer and customer indexes */
	p_idx = readl(regs->TX_PI);
	c_idx = readl(regs->RX_CI);

	rx_desc = lpc17xx_eth_get_rx_desc(c_idx);
	tx_desc = lpc17xx_eth_get_tx_desc(p_idx);

	rx_stat = lpc17xx_eth_get_rx_stat(c_idx);

	net_buf.tx_buf = (uint8_t *)(tx_desc->addr);
	net_buf.tx_size = 0;
	net_buf.rx_buf = (uint8_t *)(rx_desc->addr);
	net_buf.rx_size = rx_stat->info._info.size + 1; /* how many bytes are actualy received */
	net_buf.tx_complete = 0;

	lpc17xx_eth_rx_ci_update(regs);
}

/**
 * Transmit the packet. We have to set pkt_size in tx_desc->ctrl._ctrl.size
 * to pass the packet to wire
 */
int lpc17xx_eth_transmit_packet(void)
{
	int p_idx;
	struct tx_desc *tx_desc;
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);

	p_idx = readl(regs->TX_PI);
	tx_desc = lpc17xx_eth_get_tx_desc(p_idx);
	tx_desc->ctrl._ctrl.size = net_buf.tx_size - 1;
	net_buf.tx_size = 0;
	lpc17xx_eth_tx_pi_update(regs);

	return 0;
}

static void lpc17xx_eth_process_tx_packet(void)
{
	int p_idx;
	struct tx_desc *tx_desc;
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);

	p_idx = readl(regs->TX_PI);
	tx_desc = lpc17xx_eth_get_tx_desc(p_idx);

	tx_desc->ctrl._ctrl.size = 0;
	net_buf.tx_complete = 1;
}

static int lpc17xx_eth_irq_handler(void *arg)
{
	uint32_t stat;
	struct eth_device *eth_dev = (struct eth_device *)arg;
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);

	/* Read interrupt status register */
	stat = readl(regs->INT_STAT);
	/*printf("%s: status=%X\r\n", __func__, stat);*/

	if (stat & RX_OVERRUN) {
		/*printf("%s: RxOverrun bit is set\r\n", __func__);*/
	}

	if (stat & RX_ERROR) {
		/*printf("%s: RxError bit is set\r\n", __func__);*/
	}

	if (stat & RX_FINISHED) {
		/*printf("%s: RxFinished bit is set\r\n", __func__);*/
	}

	if (stat & RX_DONE) {
		/*printf("%s: RxDone bit is set\r\n", __func__);*/
		lpc17xx_eth_process_rx_packet(eth_dev);
	}

	if (stat & TX_UNDERRUN) {
		/*printf("%s: TxUnderrun bit is set\r\n", __func__);*/
	}

	if (stat & TX_ERROR) {
		printf("%s: TxError bit is set\r\n", __func__);
		/* TODO: reset TX path and read status info */
	}

	if (stat & TX_FINISHED) {
		/*printf("%s: TxFinished bit is set\r\n", __func__);*/
	}

	if (stat & TX_DONE) {
		/*printf("%s: TxDone bit is set\r\n", __func__);*/
		lpc17xx_eth_process_tx_packet();
	}

	/* Clear all interrupts */
	writel(regs->INT_CLR, stat);
	return IRQ_HANDLED;
}

static inline void lpc17xx_eth_enable(void)
{
	set_bit(PCONP, eth_dev->enable_bit);
}

static inline void lpc17xx_eth_disable(void)
{
	reset_bit(PCONP, eth_dev->enable_bit);
}

/* Enable RX path */
void lpc17xx_eth_rx_enable(void)
{
	uint32_t regval;
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);

	/* Enable in CMD register */
	regval = readl(regs->CMD);
	regval |= CMD_RX_EN;
	writel(regs->CMD, regval);

	/* Enable in MAC1 register */
	regval = readl(regs->MAC1);
	regval |= MAC1_RX_EN;
	writel(regs->MAC1, regval);
}

/* Disable RX path */
void lpc17xx_eth_rx_disable(void)
{
	uint32_t regval;
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);

	/* Disable in CMD register */
	regval = readl(regs->CMD);
	regval &= ~CMD_RX_EN;
	writel(regs->CMD, regval);

	/* Disable in MAC1 register */
	regval = readl(regs->MAC1);
	regval &= ~MAC1_RX_EN;
	writel(regs->MAC1, regval);
}

/* Enable/disable TX path */
void lpc17xx_eth_tx_enable(void)
{
	int regval;
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);

	/* Enable in CMD register */
	regval = readl(regs->CMD);
	regval |= CMD_TX_EN;
	writel(regs->CMD, regval);
}

void lpc17xx_eth_tx_disable(void)
{
	int regval;
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);

	/* Disable in CMD register */
	regval = readl(regs->CMD);
	regval &= ~CMD_TX_EN;
	writel(regs->CMD, regval);
}

static void rx_desc_init(struct eth_regs *regs)
{
	int i;
	struct rx_desc *rx_desc = rx_desc_array;
	struct rx_stat *rx_stat = rx_stat_array;

	for (i = 0; i < NR_RX_DESC; i++, rx_desc++, rx_stat++) {
		rx_desc->addr = RX_BUF_START_ADDR + i * RX_BUF_SIZE;
		printf("%s: buffer #%d address: %X\r\n", __func__, i, rx_desc->addr);
		rx_desc->ctrl = (RX_BUF_SIZE - 1) | BIT(31);
		rx_stat->info.info = 0;
		rx_stat->crc = 0;
	}

	writel(regs->RX_DESC, (uint32_t)rx_desc_array);
	writel(regs->RX_STAT, (uint32_t)rx_stat_array);

	/* Init descriptors number and RX customer index */
	writel(regs->RX_DESC_NUM, NR_RX_DESC - 1);
	writel(regs->RX_CI, 0);
}

static void tx_desc_init(struct eth_regs *regs)
{
	int i;
	struct tx_desc *tx_desc = tx_desc_array;
	struct tx_stat *tx_stat = tx_stat_array;

	for (i = 0; i < NR_TX_DESC; i++, tx_desc++, tx_stat++) {
		tx_desc->addr = TX_BUF_START_ADDR + i * TX_BUF_SIZE;
		printf("%s: buffer #%d address: %X\r\n",
			__func__, i, tx_desc->addr);
		tx_desc->ctrl._ctrl.crc = 1;
		tx_desc->ctrl._ctrl.last = 1;
		tx_desc->ctrl._ctrl.pad = 1;
		tx_desc->ctrl._ctrl.interrupt = 1;
		tx_stat->info = 0;
	}

	writel(regs->TX_DESC, (uint32_t)tx_desc_array);
	writel(regs->TX_STAT, (uint32_t)tx_stat_array);

	/* Init descriptors number and TX producer index */
	writel(regs->TX_DESC_NUM, NR_TX_DESC - 1);
	writel(regs->TX_PI, 0);
}

static void lpc17xx_eth_desc_init(void)
{
	rx_desc_init(eth_dev_get_regs(eth_dev));
	tx_desc_init(eth_dev_get_regs(eth_dev));
}

static void lpc17xx_eth_rx_filter(struct eth_regs *regs)
{
	uint32_t regval;

	/* Disable receive filtering */
	regval = readl(regs->CMD);
	regval |= CMD_PASS_RX_FILTER;
	writel(regs->CMD, regval);
}

static void lpc17xx_eth_irq_init(void)
{
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);
	/* Setup IRQ handler */
	irq_request(ENET, lpc17xx_eth_irq_handler, (void *)eth_dev, 0);
	/* Drop all interrupts */
	writel(regs->INT_CLR, 0xFFFF);
	/* Enable interrupt on receiving and transmitting frames */
	writel(regs->INT_EN, 0xFF);
	/* Enable interrupt in NVIC */
	irq_enable(ENET);
}

int lpc17xx_eth_init(void)
{
	int ret = 0;
	struct eth_regs *regs = eth_dev_get_regs(eth_dev);

	printf("%s: EMAC init\r\n", __func__);

	/* Power up Ethernet */
	lpc17xx_eth_enable();

	/* Mux Eth pins */
	lpc17xx_eth_mux_pins();

	/* Do full soft reset */
	lpc17xx_eth_soft_reset();

	/* Initialize the MAC registers */
	lpc17xx_mac_core_init();

	/* Set PHY RMII mode */
	lpc17xx_eth_set_rmii_mode();

	/* Select PHY speed - 100 Mb/s, half duplex*/
	/* TODO - auto-negotiation */
	lpc17xx_eth_select_speed(regs);

	/* MII initialization */
	eth_dev->mii = &mii_bus;
	ret = lpc17xx_mii_init(eth_dev->mii);
	if (!ret) {
		printf("%s: MDIO bus init failure, ret=%d\r\n", __func__, ret);
		goto err;
	}

	ret = lpc17xx_mii_set_op_mode();
	if (ret) {
		printf("%s: RMII init failure\r\n", __func__);
		goto err;
	}

	ret = lpc17xx_check_link_status();
	if (ret) {
		printf("%s: link is not ready\r\n", __func__);
		goto err;
	}

	/* Set MAC address */
	lpc17xx_eth_set_mac_addr(regs);

	/* Init RX/TX descriptors */
	lpc17xx_eth_desc_init();

	/* Setup receive filter */
	lpc17xx_eth_rx_filter(regs);

	lpc17xx_eth_irq_init();
	lpc17xx_eth_rx_enable();
	lpc17xx_eth_tx_enable();

	return 0;

err:
	lpc17xx_eth_disable();
	return ret;
}
