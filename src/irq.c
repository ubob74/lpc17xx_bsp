#include <irq.h>
#include <io.h>

static struct irq_table *irq_table = NULL;

static struct irq *irq_get_entry(int nr)
{
	if (!irq_table)
		return NULL;

	if (nr >= irq_table->nr_irq)
		return NULL;

	return irq_table->irq + nr;
}

int irq_table_init(struct irq_table *arch_irq_table)
{
	if (!arch_irq_table)
		return -1;

	if (irq_table)
		return -1;

	irq_table = arch_irq_table;
	return 0;
}

int irq_request(int nr, int (*handler)(void *), void *arg, int flags)
{
	struct irq *irq = NULL;

	if (!irq_table)
		return -1;

	if (nr >= irq_table->nr_irq)
		return -1;

	irq = irq_table->irq + nr;
	irq->handler = handler;
	irq->arg = arg;
	irq->flags = flags;

	return nr;
}

void irq_enable(int nr)
{
	if (irq_table->irq_enable)
		irq_table->irq_enable(nr);
}

void irq_disable(int nr)
{
	if (irq_table->irq_disable)
		irq_table->irq_disable(nr);
}

int irq_generic_handler(int nr)
{
	struct irq *irq = irq_get_entry(nr);
	if (irq && irq->handler)
		return irq->handler(irq->arg);

	return IRQ_NONE;
}
