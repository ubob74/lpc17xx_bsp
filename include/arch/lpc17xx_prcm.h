#ifndef _LPC17XX_PRCM_H_
#define _LPC17XX_PRCM_H_

#define MAIN_OSC 12000000UL
#define CCLK 72000000UL /* CPU clock = 72 MHz */
#define FPCLK 18000000UL /* 72 MHz CPU / 4 (default scale value) */

/* Clock Source Select register */
#define CLKSRCSEL	0x400FC10C

/* - 0x00 - Internal RC oscillator as a PLL0 clock source (default source)
 * - 0x01 - main oscillator as a PLL0 clock source
 * - 0x02 - RTC oscillator  as a PLL0 clock source
 */
#define IRC_OSC	0x00
#define OSC_CLK	0x01
#define RTC_CLK 0x02

/*****************
 * PLL0, Main PLL
 *****************/

/* PLL0 Control Register */
#define PLL0CON		0x400FC080

/* PLL0 enable */
#define PLLE0	0

/* PLL0 connect */
#define PLLC0	1

/* PLL0 Config Register */
#define PLL0CFG		0x400FC084

/* MSEL0 min and max */
#define MSEL0_MIN	6
#define MSEL0_MAX	512

/* MSEL0 bitmask (bits 14..0) */
#define MSEL0_MASK 0x4FFF

/* NSEL0 min and max */
#define NSEL0_MIN	1
#define NSEL0_MAX	32

/* NSEL0 shift */
#define NSEL0_SHIFT	16

/* NSEL0 bitmask (23..16) */
#define NSEL0_MASK	0xFF

/* PLL0 Status Register */
#define PLL0STAT	0x400FC088

#define PLLE0_STAT		24
#define PLLC0_STAT		25
#define PLOCK0			26

/* PLL0 enable/disable status */
#define PLL0_ENABLE		1
#define PLL0_DISABLE	0

/* PLL0 connected/disconnected status */
#define PLL0_CONNECT	1
#define PLL0_DISCONNECT	0

/* PLL0 locked/unlocked status */
#define PLL0_LOCKED		1
#define PLL0_UNLOCKED	0

/* PLL0 Feed Register */
#define PLL0FEED	0x400FC08C

/*****************
 * PLL1, USB PLL
 *****************/

/* PLL1 Control Register */
#define PLL1CON		0x400FC0A0

/* PLL1 Config Register */
#define PLL1CFG		0x400FC0A4

/* PLL1 Status Register */
#define PLL1STAT	0x400FC00A8

/* PLL1 Feed Register */
#define PLL1FEED	0x400FC0AC

/********************
 * Clock Dividers
 ********************/

/* CPU Clock Config Register */
#define CCLKCFG		0x400FC104

/* USB Clock Config Register */
#define USBCLKCFG	0x400FC108

/* Peripheral Clock Selection Register 0 */
#define PCLKSEL0	0x400FC1A8

/* Peripheral Clock Selection Register 1 */
#define PCLKSEL1	0x400FC1AC

/****************
 * Power control
 ****************/

/* Power Control Register */
#define PCON		0x400FC0C0

/* Power Control for Peripherals Register */
#define PCONP		0x400FC0C4

/***********
 * Utility
 ***********/

/* Clock Output Config Register */
#define CLKOUTCFG	0x400FC1C8

int lpc17xx_prcm_pll0_set_clk_src(uint8_t osc);
void lpc17xx_prcm_pll0_enable(void);
void lpc17xx_prcm_pll0_connect(void);
void lpc17xx_prcm_pll0_disable(void);
void lpc17xx_prcm_pll0_disconnect(void);

/* Set PLL0 Multiplier and Pre-Divider values */
int lpc17xx_prcm_pll0_set_mul_div_values(uint16_t m, uint8_t n);

/* Read PLL0 status register */
uint16_t lpc17xx_prcm_pll0_read_mul_value(void);
uint8_t lpc17xx_prcm_pll0_read_div_value(void);
int lpc17xx_prcm_pll0_is_enabled(void);
int lpc17xx_prcm_pll0_is_connected(void);
int lpc17xx_prcm_pll0_is_locked(void);

/* CPU/USB clock dividers setting */
int lpc17xx_prcm_set_cpu_clk_div(uint8_t div);
int lpc17xx_prcm_set_usb_clk_div(uint8_t div);

/* CLK output selector */
int lpc17xx_prcm_set_clkout_src(uint8_t clkout_src, uint8_t clkout_div);

/* Init PLL0 */
int lpc17xx_prcm_pll0_init(uint8_t cclk_div, uint8_t usb_div);

#endif /* _LPC17XX_PRCM_H_ */
