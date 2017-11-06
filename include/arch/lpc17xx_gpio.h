#ifndef _LPC17XX_GPIO_H_
#define _LPC17XX_GPIO_H_

#include <_stdint.h>

#define GPIO_OUT	1
#define GPIO_IN		0

struct lpc17xx_gpio {
	uint32_t FIODIR;
	uint32_t FIOMASK;
	uint32_t FIOPIN;
	uint32_t FIOSET;
	uint32_t FIOCLR;
};

struct gpio {
	int bank;
	int pin_num;
	int direction;
};

static inline int gpio_mode(int mode)
{
	return (mode == 0);
}

void gpio_setup(struct gpio *gpio);
void gpio_set_value(struct gpio *gpio, unsigned int val);
int gpio_get_value(struct gpio *gpio);

#endif
