#include <io.h>

int set_bit(uint32_t addr, uint8_t pos)
{
	volatile uint32_t *val = (volatile uint32_t *)addr;

	if (pos > BIT_NUM_MAX)
		return -1;

	*val |= BIT(pos);
	return 0;
}

int reset_bit(uint32_t addr, uint8_t pos)
{
	volatile uint32_t *val = (volatile uint32_t *)addr;

	if (pos > BIT_NUM_MAX)
		return -1;

	*val &= ~(BIT(pos));
	return 0;
}

int test_bit(uint32_t addr, uint8_t pos)
{
	uint32_t *val = 0;

	if (pos > BIT_NUM_MAX)
		return -1;

	val = (uint32_t *)addr;
	return !!(*val & (1UL << pos));
}

int set_value(uint32_t addr, int value, uint8_t pos, int width)
{
	uint32_t regval = 0;
	int mask;

	if (pos + width > BIT_NUM_MAX)
		return -1;

	/* Get current value */
	regval = readl(addr);

	/* Drop out an old value */
	mask = (1 << width) - 1;
	regval &= ~(mask << pos);

	/* Setup a new value */
	regval |= (value << pos);
	writel(addr, regval);

	return 0;
}

uint32_t get_value(uint32_t addr, uint8_t pos, int width)
{
	uint32_t regval = 0;
	uint32_t mask;

	if (pos + width > BIT_NUM_MAX)
		return 0;

	regval = readl(addr);
	mask = (1 << width) - 1;
	return (regval >> pos) & mask;
}
