#ifndef _LPC17XX_SYS_CTRL_H_
#define _LPC17XX_SYS_CTRL_H_

/* External Interrupt Flag Register */
#define EXTINT		0x400FC140

/* External Interrupt Mode Register */
#define EXTMODE		0x400FC148

/* External Interrupt Polarity Register */
#define EXTPOLAR	0x400FC14C

/* Reset Source Identification Register */
#define RSID		0x400FC180

/* System Control and Status */
#define SCS			0x400FC1A0

/* SCS control bits:
 * - OCSRANGE - main oscillator range select:
 *	 -- 0 - 1-20 Mhz
 *	 -- 1 - 15-25 Mhz
 * - OSCEN - main oscillator enable (1 - enable, 0 - disable)
 * - OSCSTAT - main oscillator status (0 - not ready, 1 - ready)
 */
#define OSCRANGE	4
#define OSCEN		5
#define OSCSTAT		6

/* Main OSC Frequency Ranges*/
#define MAIN_OSC_FREQ_RANGE_LOW		0
#define MAIN_OSC_FREQ_RANGE_HIGH	1

/* Main OSC enable/disable state */
#define MAIN_OSC_ENABLE		1
#define MAIN_OSC_DISABLE	0

/* Main OSC Status */
#define MAIN_OSC_READY		1
#define MAIN_OSC_NOT_READY	0

int lpc17xx_sys_ctrl_main_osc_set_range(int range);
int lpc17xx_sys_ctrl_main_osc_get_range(void);

void lpc17xx_sys_ctrl_main_osc_enable(void);
void lpc17xx_sys_ctrl_main_osc_disable(void);
int lpc17xx_sys_ctrl_main_osc_is_enabled(void);

int lpc17xx_sys_ctrl_main_osc_get_status(void);

#endif /* _LPC17XX_SYS_CTRL_H_ */
