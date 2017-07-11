#ifndef _NVIC_H_
#define _NVIC_H_

#include <_stdint.h>

/**
 * System Control Space base address
 */
#define SCS_BASE_ADDR 0xE000E000

/**
 * System Control Block base address
 */
#define SCB_BASE_ADDR 0xE000ED00

/**
 * NVIC base address
 */
#define NVIC_BASE_ADDR 0xE000E100

/* SCB registers offset for ARMv6-M */
#define CPUID		0x00
#define ICSR		0x04
#define VTOR		0x08
#define AIRCR		0x0C
#define SCR			0x10
#define CCR			0x14
#define SHPR2		0x1C
#define SHPR3		0x20
#define SHCSR		0x24
#define DFSR		0x30

/* NVIC registers offset */
#define NVIC_ISER	0x000
#define NVIC_ICER	0x080
#define NVIC_ISPR	0x100
#define NVIC_ICPR	0x180
#define NVIC_IPR	0x200

uint32_t nvic_get_cpuid(void);

void nvic_irq_set_enable(uint8_t irq_num);
void nvic_irq_clear_enable(uint8_t irq_num);

void nvic_irq_set_pending(uint8_t irq_num);
void nvic_irq_clear_pending(uint8_t irq_num);

int nvic_irq_set_priority(uint8_t irq);
int nvic_irq_get_priority(uint8_t irq);

#endif /* _NVIC_H_ */
