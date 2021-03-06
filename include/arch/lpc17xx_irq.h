#ifndef _LPC17XX_IRQ_HANDLER_H_
#define _LPC17XX_IRQ_HANDLER_H_

#define LPC17XX_IRQ_NUM 35

int lpc17xx_irq_table_init(void);

/* TODO: Interrupt vectors */
#define TIMER0		1
#define TIMER1		2
#define TIMER2		3
#define TIMER3		4
#define UART0		5
#define UART1		6
#define UART2		7
#define UART3		8
#define PLL0		16
#define ENET		28

#endif /* _LPC17XX_IRQ_HANDLER_H_ */
