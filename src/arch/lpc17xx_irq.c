#include <io.h>
#include <irq.h>
#include <nvic.h>
#include <lpc17xx_irq.h>

static void lpc17xx_irq_enable(int nr);
static void lpc17xx_irq_disable(int nr);
static struct irq lpc17xx_irq[LPC17XX_IRQ_NUM];

static struct irq_table lpc17xx_irq_table = {
	.irq = lpc17xx_irq,
	.nr_irq = LPC17XX_IRQ_NUM,
	.irq_enable = lpc17xx_irq_enable,
	.irq_disable = lpc17xx_irq_disable,
};

static void lpc17xx_irq_enable(int nr)
{
	nvic_irq_set_enable(nr);
}

static void lpc17xx_irq_disable(int nr)
{
	nvic_irq_clear_enable(nr);
}

int lpc17xx_irq_table_init(void)
{
	int i;
	struct irq *irq = NULL;

	for (i = 0; i < lpc17xx_irq_table.nr_irq; i++) {
		irq = lpc17xx_irq_table.irq + i;
		irq->handler = NULL;
		irq->arg = NULL;
		irq->flags = 0;
	}

	return irq_table_init(&lpc17xx_irq_table);
}

/* IRQ handlers */
IRQ_HANDLER(pll0_irq_handler, PLL0);
IRQ_HANDLER(uart0_irq_handler, UART0);
IRQ_HANDLER(uart1_irq_handler, UART1);
IRQ_HANDLER(uart2_irq_handler, UART2);
IRQ_HANDLER(uart3_irq_handler, UART3);
IRQ_HANDLER(enet_irq_handler, ENET);
