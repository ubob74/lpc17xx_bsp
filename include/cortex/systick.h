#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include <_stdint.h>

#define SYSTICK_BASE_ADDR 0xE000E010

/* SysTick registers offset:
 * - CSR - Control and Status Register
 * - RVR - Reload Value Register
 * - CVR - Current Value Register
 * - CALIB - Calibration Register
 */
#define CSR 0x00
#define RVR 0x04
#define CVR 0x08
#define CALIB 0x0C

/**
 * CSR bits:
 * - ENABLE - enable/disable counter
 * - TICKINT - exception control
 * - CLKSRC - SysTick clock source
 * - COUNTFLAG - counter control
 */
#define CSR_ENABLE			0
#define CSR_TICKINT			1
#define CSR_CLKSRC			2
#define CSR_COUNTFLAG		16

void systick_set_csr(uint32_t);
void systick_set_rvr(uint32_t);
void systick_set_cvr(uint32_t);

int systick_init(void);

#endif /* _SYSTICK_H_ */
