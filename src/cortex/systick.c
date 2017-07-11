#include <systick.h>
#include <io.h>
#include <nvic.h>
#include <exceptions.h>

/* Set Control and Status Register */
void systick_set_csr(uint32_t val)
{
	writel(SYSTICK_BASE_ADDR + CSR, val);
}

/* Set Reload Value Register */
void systick_set_rvr(uint32_t val)
{
	writel(SYSTICK_BASE_ADDR + RVR, val);
}

/* Set Current Value Register */
void systick_set_cvr(uint32_t val)
{
	writel(SYSTICK_BASE_ADDR + CVR, val);
}

int systick_set_exception_handler(int (*handler)(void *), void *arg, int flags)
{
	return set_exception(SYSTICK_EXCEPTION, handler, arg, flags);
}
