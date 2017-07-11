#include <nvic.h>
#include <io.h>

uint32_t nvic_get_cpuid(void)
{
	return readl(SCB_BASE_ADDR + CPUID);
}

void nvic_irq_set_enable(uint8_t irq_num)
{
	writel(NVIC_ISER + NVIC_BASE_ADDR, BIT(irq_num));
}

void nvic_irq_clear_enable(uint8_t irq_num)
{
	writel(NVIC_ICER + NVIC_BASE_ADDR, BIT(irq_num));
}

void nvic_irq_set_pending(uint8_t irq_num)
{
	writel(NVIC_ISPR + NVIC_BASE_ADDR, BIT(irq_num));
}

void nvic_irq_clear_pending(uint8_t irq_num)
{
	writel(NVIC_ICPR + NVIC_BASE_ADDR, BIT(irq_num));
}

int nvic_irq_set_priority(uint8_t irq_num)
{
	/* TODO */
	return 0;
}

int nvic_irq_get_priority(uint8_t irq_num)
{
	/* TODO */
	return 0;
}
