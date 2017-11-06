#include <lpc17xx_gpio.h>
#include <io.h>

static struct lpc17xx_gpio lpc17xx_gpio_array[] = {
	[0] = {
		.FIODIR = 0x2009C000,
		.FIOMASK = 0x2009C010,
		.FIOPIN = 0x2009C014,
		.FIOSET = 0x2009C018,
		.FIOCLR = 0x2009C01C,
	},
	[1] = {
		.FIODIR = 0x2009C020,
		.FIOMASK = 0x2009C030,
		.FIOPIN = 0x2009C034,
		.FIOSET = 0x2009C038,
		.FIOCLR = 0x2009C03C,
	},
	[2] = {
		.FIODIR = 0x2009C040,
		.FIOMASK = 0x2009C050,
		.FIOPIN = 0x2009C054,
		.FIOSET = 0x2009C058,
		.FIOCLR = 0x2009C05C,
	},
	[3] = {
		.FIODIR = 0x2009C060,
		.FIOMASK = 0x2009C070,
		.FIOPIN = 0x2009C074,
		.FIOSET = 0x2009C078,
		.FIOCLR = 0x2009C07C,
	},
	[4] = {
		.FIODIR = 0x2009C080,
		.FIOMASK = 0x2009C090,
		.FIOPIN = 0x2009C094,
		.FIOSET = 0x2009C098,
		.FIOCLR = 0x2009C09C,
	},
};

void gpio_setup(struct gpio *gpio)
{
	struct lpc17xx_gpio *lpc17xx_gpio = &lpc17xx_gpio_array[gpio->bank];

	if (gpio->direction == GPIO_OUT) {
		writel(lpc17xx_gpio->FIODIR, BIT(gpio->pin_num));
		writel(lpc17xx_gpio->FIOSET, BIT(gpio->pin_num));
	} else {
		writel(lpc17xx_gpio->FIOCLR, BIT(gpio->pin_num));
	}
}

void gpio_set_value(struct gpio *gpio, unsigned int val)
{
	struct lpc17xx_gpio *lpc17xx_gpio = &lpc17xx_gpio_array[gpio->bank];
	uint32_t addr;

	if (gpio->direction != GPIO_OUT || val > 1)
		return;

	addr = (val == 1) ? lpc17xx_gpio->FIOSET : lpc17xx_gpio->FIOCLR;
	writel(addr, BIT(gpio->pin_num));
}

int gpio_get_value(struct gpio *gpio)
{
	struct lpc17xx_gpio *lpc17xx_gpio = &lpc17xx_gpio_array[gpio->bank];
	return test_bit(lpc17xx_gpio->FIOPIN, gpio->pin_num);
}
