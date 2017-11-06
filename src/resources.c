#include <_stdint.h>
#include <resources.h>
#include <io.h>
#include <lpc17xx_gpio.h>

void dev_mux_pins(struct device *dev)
{
	int i;
	struct pin *pin;
	int nr_pins;

	pin = dev->res.pins.pins;
	nr_pins = dev->res.pins.nr_pins;

	for (i = 0; i < nr_pins; pin++, i++) {
		set_value(pin->pinsel_addr, pin->mode,
			(pin->num) << 1, 2);

		if (gpio_mode(pin->mode))
			gpio_setup(&pin->gpio);
	}
}
