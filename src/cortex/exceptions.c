#include <exceptions.h>
#include <io.h>

static struct exception exception[EXCEPTIONS_NUM];

static void exception_generic_handler(int exc_num);

static struct exception_table exception_table = {
	.exception = exception,
	.nr_exception = EXCEPTIONS_NUM,
};

static void exception_generic_handler(int exc_num)
{
	if (exc_num < exception_table.nr_exception) {
		struct exception *exception = exception_table.exception + exc_num;
		if (exception && exception->handler) {
			exception->handler(exception->arg);
			return;
		}
	}

	while(1);
}

#define EXCEPTION(name, index) \
	void name() { \
		exception_generic_handler(index); \
	}

EXCEPTION(nmi_handler, NMI_EXCEPTION);
EXCEPTION(hard_fault_handler, HARD_FAULT_EXCEPTION);
EXCEPTION(mem_manage_handler, MEM_MANAGE_EXCEPTION);
EXCEPTION(bus_fault_handler, BUS_FAULT_EXCEPTION);
EXCEPTION(usage_fault_handler, USAGE_FAULT_EXCEPTION);
EXCEPTION(svc_handler, SVC_EXCEPTION);
EXCEPTION(debug_mon_handler, DEBUG_MON_EXCEPTION);
EXCEPTION(pend_sv_handler, PEND_SV_EXCEPTION);
EXCEPTION(systick_handler, SYSTICK_EXCEPTION);

int set_exception(int nr, int (*handler)(void *), void *arg, int flags)
{
	struct exception *exception = NULL;

	if (nr >= exception_table.nr_exception)
		return -1;

	exception = exception_table.exception + nr;
	exception->handler = handler;
	exception->arg = arg;
	return 0;
}
