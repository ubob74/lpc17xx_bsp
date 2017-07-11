#ifndef _IO_H_
#define _IO_H_

#include <_stdlib.h>
#include <_stdint.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define BIT_NUM_MAX 32
#define BIT(x) (1UL << (x))

static inline uint32_t readl(uint32_t addr)
{
	return *(volatile uint32_t *)addr;
}

static inline uint16_t readw(uint32_t addr)
{
	return *(volatile uint16_t *)addr;
}

static inline uint8_t readb(uint32_t addr)
{
	return *(volatile uint8_t *)addr;
}

static inline void writel(uint32_t addr, uint32_t val)
{
	*(volatile uint32_t *)addr = val;
}

static inline void writew(uint32_t addr, uint16_t val)
{
	*(volatile uint16_t *)addr = val;
}

static inline void writeb(uint32_t addr, uint8_t val)
{
	*(volatile uint32_t *)addr = val;
}

int set_bit(uint32_t addr, uint8_t pos);
int reset_bit(uint32_t addr, uint8_t pos);
int test_bit(uint32_t addr, uint8_t pos);

int set_value(uint32_t addr, int value, uint8_t pos, int width);
uint32_t get_value(uint32_t addr, uint8_t pos, int width);

#endif /* _IO_H_ */
