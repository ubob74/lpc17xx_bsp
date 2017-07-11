#ifndef _RESOURCES_H_
#define _RESOURCES_H_

#include <_stdint.h>
#include <atomic.h>

#define PINSEL0 0x4002C000
#define PINSEL1 0x4002C004
#define PINSEL2 0x4002C008
#define PINSEL3 0x4002C00C
#define PINSEL4 0x4002C010
#define PINSEL7 0x4002C01C
#define PINSEL8 0x4002C020
#define PINSEL9 0x4002C024
#define PINSEL10 0x4002C028

#define PINMODE0 0x4002C040
#define PINMODE1 0x4002C044
#define PINMODE2 0x4002C048
#define PINMODE3 0x4002C04C
#define PINMODE4 0x4002C050
#define PINMODE5 0x4002C054
#define PINMODE6 0x4002C058
#define PINMODE7 0x4002C05C
#define PINMODE9 0x4002C064

#define PINMODE_OD0 0x4002C068
#define PINMODE_OD1 0x4002C06C
#define PINMODE_OD2 0x4002C070
#define PINMODE_OD3 0x4002C074
#define PINMODE_OD4 0x4002C078

struct pin {
	uint32_t pinsel_addr;
	uint8_t num;
	int mode;
};

struct registers {
	void *regs;
	int nr_regs;
};

struct pins {
	struct pin *pins;
	int nr_pins;
};

struct resources {
	struct registers regs;
	struct pins pins;
};

struct device {
	int id;
	int state;
	int irq;
	int (*irq_handler)(void *);
	struct resources res;
	atomic_t ref_cnt;
};

void dev_mux_pins(struct device *dev);
void *dev_get_regs(struct device *dev);

#endif
