#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <_stdint.h>

#define EXCEPTIONS_NUM 15

#define NMI_EXCEPTION			0
#define HARD_FAULT_EXCEPTION		1
#define MEM_MANAGE_EXCEPTION		2
#define BUS_FAULT_EXCEPTION		3
#define USAGE_FAULT_EXCEPTION		4
#define SVC_EXCEPTION			5
#define DEBUG_MON_EXCEPTION		6
#define PEND_SV_EXCEPTION		7
#define SYSTICK_EXCEPTION		8

struct exception {
	int (*handler)(void *);
	void *arg;
};

struct exception_table {
	struct exception *exception;
	int nr_exception;
};

void nmi_handler(void);
void hard_fault_handler(void);
void mem_manage_handler(void);
void bus_fault_handler(void);
void usage_fault_handler(void);
void svc_handler(void);
void debug_mon_handler(void);
void pend_sv_handler(void);
void systick_handler(void);

int exception_table_init(struct exception_table *);
int set_exception(int nr, int (*handler)(void *), void *arg, int flags);
struct exception * get_exception(int nr);

#endif /* _EXCEPTIONS_H_ */
