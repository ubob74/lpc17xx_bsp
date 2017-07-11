#include <io.h>
#include <irq.h>
#include <lpc17xx_prcm.h>
#include <lpc17xx_sys_ctrl.h>
#include <lpc17xx_irq.h>

static inline void lpc17xx_prcm_pll0_set_feed(void)
{
	writeb(PLL0FEED, 0xAA);
	writeb(PLL0FEED, 0x55);
}

int lpc17xx_prcm_pll0_is_connected(void)
{
	return (test_bit(PLL0STAT, PLLC0_STAT))
		? PLL0_CONNECT : PLL0_DISCONNECT;
}

int lpc17xx_prcm_pll0_is_enabled(void)
{
	return (test_bit(PLL0STAT, PLLE0_STAT))
		? PLL0_ENABLE : PLL0_DISABLE;
}

int lpc17xx_prcm_pll0_is_locked(void)
{
	return (test_bit(PLL0STAT, PLOCK0))
		? PLL0_LOCKED : PLL0_UNLOCKED;
}

int lpc17xx_prcm_pll0_set_clk_src(uint8_t osc)
{
	uint32_t val;

	/* Check value */
	if (osc > RTC_CLK)
		return -1;

	val = readl(CLKSRCSEL);
	val &= ~0x03;
	val |= osc;
	writel(CLKSRCSEL, val);

	return 0;
}

void lpc17xx_prcm_pll0_enable(void)
{
	uint32_t val;

	val = readl(PLL0CON);
	val |= BIT(PLLE0);
	writel(PLL0CON, val);

	lpc17xx_prcm_pll0_set_feed();
}

void lpc17xx_prcm_pll0_disable(void)
{
	uint32_t val;

	val = readl(PLL0CON);
	val &= ~BIT(PLLE0);
	writel(PLL0CON, val);

	lpc17xx_prcm_pll0_set_feed();
}

void lpc17xx_prcm_pll0_connect(void)
{
	uint32_t val;

	val = readl(PLL0CON);
	val |= BIT(PLLC0);
	writel(PLL0CON, val);

	lpc17xx_prcm_pll0_set_feed();
}

void lpc17xx_prcm_pll0_disconnect(void)
{
	uint32_t val;

	val = readl(PLL0CON);
	val &= ~BIT(PLLC0);
	writel(PLL0CON, val);

	lpc17xx_prcm_pll0_set_feed();
}

int lpc17xx_prcm_pll0_set_mul_div_values(uint16_t m, uint8_t n)
{
	uint32_t val;

	if (m < MSEL0_MIN || m > MSEL0_MAX)
		return -1;

	if (n < NSEL0_MIN || n > NSEL0_MAX)
		return -1;

	val = ((m - 1) & MSEL0_MASK) | (((n - 1) & NSEL0_MASK) << NSEL0_SHIFT);
	writel(PLL0CFG, val);

	lpc17xx_prcm_pll0_set_feed();

	return 0;
}

uint16_t lpc17xx_prcm_pll0_read_mul_value(void)
{
	uint32_t val;

	val = readl(PLL0STAT);
	return (uint16_t)((val + 1) & MSEL0_MASK);
}

uint8_t lpc17xx_prcm_pll0_read_div_value(void)
{
	uint32_t val = readl(PLL0STAT);
	return (uint8_t)(((val >> NSEL0_SHIFT) + 1) & NSEL0_MASK);
}

int lpc17xx_prcm_set_cpu_clk_div(uint8_t div)
{
	uint32_t val;

	if (div < 1)
		return -1;

	val = (div - 1) & 0xFF;
	writel(CCLKCFG, val);

	return 0;
}

int lpc17xx_prcm_set_usb_clk_div(uint8_t div)
{
	uint32_t val;

	if (div != 6 && div != 8 && div != 10)
		return -1;

	val = (div - 1) & 0x0F;
	writel(USBCLKCFG, val);

	return 0;
}

int lpc17xx_prcm_set_clkout_src(uint8_t clkout_src, uint8_t clkout_div)
{
	uint32_t val;

	if ((clkout_src > 4) || (clkout_div > 16))
		return -1;

	val = clkout_src | ((clkout_div - 1) << 4);
	writel(CLKOUTCFG, val);

	return 0;
}

static int lpc17xx_prcm_pll0_irq_handler(void *arg)
{
	/* Connect PLL0 */
	lpc17xx_prcm_pll0_connect();

	/* Disable IRQ */
	irq_disable(PLL0);

	return IRQ_HANDLED;
}

static int lpc17xx_prcm_main_ocs_connect()
{
	int ret = -1;

	ret = lpc17xx_sys_ctrl_main_osc_set_range(MAIN_OSC_FREQ_RANGE_LOW);
	ret = lpc17xx_sys_ctrl_main_osc_get_range();
	if (ret != MAIN_OSC_FREQ_RANGE_LOW)
		goto exit;

	lpc17xx_sys_ctrl_main_osc_enable();
	while (lpc17xx_sys_ctrl_main_osc_is_enabled() != MAIN_OSC_ENABLE);
	ret = lpc17xx_sys_ctrl_main_osc_get_status();
	if (ret != MAIN_OSC_READY)
		goto exit;

	ret = 0;

exit:
	return ret;
}

int lpc17xx_prcm_pll0_init(uint8_t cclk_div, uint8_t usb_div)
{
	int ret = -1;
	uint16_t m;
	uint8_t n;

	/* Connect main oscillator */
	if (lpc17xx_prcm_main_ocs_connect() < 0)
		return -1;

	/* Disconnect PLL0 */
	if (lpc17xx_prcm_pll0_is_connected()) {
		lpc17xx_prcm_pll0_disconnect();
		while (lpc17xx_prcm_pll0_is_connected() != PLL0_DISCONNECT);
	}

	/* Disable PLL0 */
	if (lpc17xx_prcm_pll0_is_enabled()) {
		lpc17xx_prcm_pll0_disable();
		while (lpc17xx_prcm_pll0_is_enabled() != PLL0_DISABLE);
	}

	/* Select main oscillator */
	lpc17xx_prcm_pll0_set_clk_src(OSC_CLK);

	/* Setup CPU clock divider before connecting PLL0:
	FCCO == 288 MHz, CCLK = FCCO/CCLK_DIV = 288/4 = 72 MHz */
	if (lpc17xx_prcm_set_cpu_clk_div(cclk_div) < 0)
		goto err;

	/* Setup USB clock divider */
	if (lpc17xx_prcm_set_usb_clk_div(usb_div) < 0)
		goto err;

	/* Set PLL0CFG */
	m = 12;
	n = 1;
	lpc17xx_prcm_pll0_set_mul_div_values(m, n);

	irq_request(PLL0, lpc17xx_prcm_pll0_irq_handler, NULL, 0);
	irq_enable(PLL0);

	/* Enable PLL0 */
	lpc17xx_prcm_pll0_enable();

	/* Waiting for PLL0 connecting */
	while (lpc17xx_prcm_pll0_is_connected() != PLL0_CONNECT);

	ret = 0;

err:
	return ret;
}
