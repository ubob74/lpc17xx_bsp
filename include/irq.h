#ifndef _IRQ_HANDLER_H_
#define _IRQ_HANDLER_H_

#include <_stdint.h>

#define IRQ_NONE	0
#define IRQ_HANDLED	1

struct irq {
	int (*handler)(void *);
	void *arg;
	int flags;
};

struct irq_table {
	struct irq *irq;
	int nr_irq;
	void (*irq_enable)(int);
	void (*irq_disable)(int);
};

int irq_table_init(struct irq_table *);
int irq_request(int nr, int (*handler)(void *), void *arg, int flags);
void irq_enable(int nr);
void irq_disable(int nr);
int irq_generic_handler(int nr);

#define IRQ_HANDLER(handler, irq_num) \
	void handler(void) { \
		irq_generic_handler(irq_num); \
	}

#endif /* _IRQ_HANDLER_H_ */
