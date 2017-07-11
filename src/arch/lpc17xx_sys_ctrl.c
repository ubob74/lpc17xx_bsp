#include <io.h>
#include <lpc17xx_sys_ctrl.h>

int lpc17xx_sys_ctrl_main_osc_set_range(int range)
{
	uint32_t val = readl(SCS);

	switch (range) {
		case MAIN_OSC_FREQ_RANGE_LOW:
			val &= ~(BIT(OSCRANGE));
			break;
		case MAIN_OSC_FREQ_RANGE_HIGH:
			val |= BIT(OSCRANGE);
			break;
		default:
			return -1;
	}

	writel(SCS, val);
	return range;
}

int lpc17xx_sys_ctrl_main_osc_get_range(void)
{
	return (test_bit(SCS, OSCRANGE))
		? MAIN_OSC_FREQ_RANGE_HIGH : MAIN_OSC_FREQ_RANGE_LOW;
}

void lpc17xx_sys_ctrl_main_osc_enable(void)
{
	uint32_t val = readl(SCS);
	val |= BIT(OSCEN);
	writel(SCS, val);
}

void lpc17xx_sys_ctrl_main_osc_disable(void)
{
	uint32_t val = readl(SCS);
	val &= ~(BIT(OSCEN));
	writel(SCS, val);
}

int lpc17xx_sys_ctrl_main_osc_is_enabled(void)
{
	return (test_bit(SCS, OSCSTAT))
		? MAIN_OSC_ENABLE : MAIN_OSC_DISABLE;
}

int lpc17xx_sys_ctrl_main_osc_get_status(void)
{
	return (test_bit(SCS, OSCSTAT))
		? MAIN_OSC_READY : MAIN_OSC_NOT_READY;
}
