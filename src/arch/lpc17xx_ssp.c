#include <lpc17xx_ssp.h>
#include <lpc17xx_ssp_resources.h>
#include <lpc17xx_prcm.h>
#include <resources.h>
#include <io.h>
#include <card.h>

static struct ssp_device *ssp0_dev = &lpc17xx_ssp0_device;

static void lpc17xx_ssp0_power_on(void)
{
	set_bit(PCONP, ssp0_dev->enable_bit);
}

static inline void lpc17xx_ssp0_mux_pins(void)
{
	dev_mux_pins(&ssp0_dev->dev);
}

/* Get SSP0 PCLK divider */
static unsigned int lpc17xx_ssp0_get_div(void)
{
	unsigned int retval;

	retval = readl(PCLKSEL1);
	retval >>= 10;
	return (retval & 0x3UL);
}

/* Get SSP0 PCLK (peripheral clock) */
static unsigned int lpc17xx_ssp0_get_pclk(void)
{
	unsigned int pclk = 0;
	unsigned int div[4] = {4,1,2,8};
	unsigned int idx = lpc17xx_ssp0_get_div();

	pclk = CCLK / div[idx];

	return pclk;
}

static void lpc17xx_ssp0_set_clock(unsigned int target_clock)
{
	struct ssp_regs *ssp0_regs = dev_get_regs(&ssp0_dev->dev);
	unsigned int prescale, cr0_div, cmp_clk, ssp_clk;
	uint16_t scr = 0;

	ssp_clk = lpc17xx_ssp0_get_pclk();

	/* Find closest divider to get at or under the target freq.
	 Use smallest prescale possible and rely on the divider to get
	 the closest target freq */
	cr0_div = 0;
	cmp_clk = 0xFFFFFFFF;
	prescale = 2;
	while (cmp_clk > target_clock) {
		cmp_clk = ssp_clk / ((cr0_div + 1) * prescale);
		if (cmp_clk > target_clock) {
			cr0_div++;
			if (cr0_div > 0xFF) {
				cr0_div = 0;
				prescale += 2;
			}
		}
	}

	printf("cr0_div=%x prescale=%x\r\n", cr0_div, prescale);

	scr = readw(ssp0_regs->CR0);
	scr |= (uint8_t)cr0_div << 8;
	writew(ssp0_regs->CR0, scr);
	writeb(ssp0_regs->CPSR, (uint8_t)prescale);
}

/**
 * Setup basic config:
 * - first clock phase
 * - clock polarity - high level
 * - SSP master mode
 * - frame format - SSP frame SPI
 * - clock rate - 1000000 Hz
 * - data bit - 8 bit
 */
static void lpc17xx_ssp0_setup(void)
{
	struct ssp_regs *ssp0_regs = dev_get_regs(&ssp0_dev->dev);
	uint16_t val;

	val = 7;
	writew(ssp0_regs->CR0, val);

	val = 0;
	writew(ssp0_regs->CR1, val);
}

static void lpc17xx_ssp0_enable(void)
{
	struct ssp_regs *ssp0_regs = dev_get_regs(&ssp0_dev->dev);

	/* SSP enable */
	set_bit(ssp0_regs->CR1, 1);
}

static void lpc17xx_ssp0_disable(void)
{
	struct ssp_regs *ssp0_regs = dev_get_regs(&ssp0_dev->dev);

	/* SSP disable */
	reset_bit(ssp0_regs->CR1, 1);
}

static void lpc17xx_ssp0_card_enable(void)
{
	int idx = ssp0_dev->card_select;
	struct pin *pin = dev_get_pin(&ssp0_dev->dev, idx);
	gpio_set_value(&pin->gpio, 0);
}

static void lpc17xx_ssp0_card_disable(void)
{
	int idx = ssp0_dev->card_select;
	struct pin *pin = dev_get_pin(&ssp0_dev->dev, idx);
	gpio_set_value(&pin->gpio, 1);
}

static int lpc17xx_ssp0_card_insert(void)
{
	int idx = ssp0_dev->card_detect;
	struct pin *pin = dev_get_pin(&ssp0_dev->dev, idx);
	return gpio_get_value(&pin->gpio);
}

static void lpc17xx_ssp0_card_power_on(void)
{
	int idx = ssp0_dev->card_power;
	struct pin *pin = dev_get_pin(&ssp0_dev->dev, idx);
	gpio_set_value(&pin->gpio, 0);
}

#if 0
static void insert_remove_test(void)
{
	int ci, n = 0, m = 0;

	ci = lpc17xx_ssp0_card_insert();
	if (ci) {
		printf("card removed\r\n");
		n = 1;
	} else {
		printf("card inserted\r\n");
		m = 1;
	}

	for (;;) {
		ci = lpc17xx_ssp0_card_insert();
		if (ci && !n) {
			printf("card removed\r\n");
			n = 1;
			m = 0;
		} else if (!ci && !m) {
			printf("card inserted\r\n");
			m = 1;
			n = 0;
		}
	}
}
#endif

static int card_init(void);
static int card_get_info(void);
static int card_read_block(uint32_t sector, uint8_t *block);

int lpc17xx_ssp0_init(void)
{
	int ret;
	int i;
	uint32_t sector;
	uint8_t block[BLOCK_SIZE];

	lpc17xx_ssp0_power_on();
	lpc17xx_ssp0_mux_pins();
	lpc17xx_ssp0_setup();
	lpc17xx_ssp0_set_clock(CLOCK_RATE);

	lpc17xx_ssp0_card_disable();

	lpc17xx_ssp0_enable();

//	insert_remove_test();
	ret = card_init();
	if (ret)
		goto out;

	card_get_info();

	sector = 0;
	memset(block, 0x00, sizeof(block));
	ret = card_read_block(sector, block);
	if (!ret) {
		for (i = 0; i < BLOCK_SIZE; i++)
			printf("0x%x ", block[i]);
		printf("\r\n");
	} else {
		printf("%s: can't read sector #%d, ret=%d\r\n",
			__func__, sector, ret);
	}

out:
	return 0;
}

////////////////////////////////////////////////////////////////

static uint8_t spi_txrx(uint8_t data)
{
	struct ssp_regs *ssp0_regs = dev_get_regs(&ssp0_dev->dev);

	while (test_bit(ssp0_regs->SR, BSY));
	writeb(ssp0_regs->DR, data);
	while (!test_bit(ssp0_regs->SR, RNE));
	return readb(ssp0_regs->DR);
}

static int send_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t r1;
	uint8_t retry;

	lpc17xx_ssp0_card_enable();

	spi_txrx(DUMMY_BYTE);
	spi_txrx(cmd | 0x40);
	spi_txrx(arg >> 24);
	spi_txrx(arg >> 16);
	spi_txrx(arg >> 8);
	spi_txrx(arg);
	spi_txrx(crc);

	/* Wait for response */
	for (retry = 0; retry < 200; retry++) {
		r1 = spi_txrx(DUMMY_BYTE);
		if (r1 != DUMMY_BYTE)
			break;
	}

	spi_txrx(DUMMY_BYTE);
	lpc17xx_ssp0_card_disable();

	return r1;
}

static int read_buffer(uint8_t *data, uint16_t data_sz, uint8_t release)
{
	int n;
	uint8_t r1;
	uint16_t retry;

	lpc17xx_ssp0_card_enable();

	for (;;) {
		r1 = spi_txrx(DUMMY_BYTE);
		if (r1 == 0xFE)
			break;
	}

	for (retry = 0; retry < data_sz; retry++)
		data[retry] = spi_txrx(DUMMY_BYTE);

	/* 2 bytes dummy CRC */
	spi_txrx(DUMMY_BYTE);
	spi_txrx(DUMMY_BYTE);

	if (release) {
		lpc17xx_ssp0_card_disable();
		spi_txrx(DUMMY_BYTE);
	}

	return 0;
}

static int check_card_type(void)
{
	uint8_t r1;
	uint16_t retry;

	/* Card operation conditions */
	r1 = send_command(CMD8, 0x1AA, 0x87);

	printf("%s: r1=0x%x\r\n", __func__, r1);

	/* r1 == 0x5 - Illegal command, i.e. we have MMC or SD v1.0 */
	if (r1 == 0x5) {
		/* MMC/SDv1.0 start init */
		for (retry = 0; retry < 0xFFF; retry++) {
			r1 = send_command(ACMD41, 0, 0);
			if (r1 == 0x00) {
				printf("SD card\r\n");
				return -1;
			}
		}

		/* MMC card init start */
		if (retry == 0xFFF) {
			for (retry = 0; retry < 0xFFFF; retry++) {
				r1 = send_command(CMD1, 0, 0);
				if (r1 == 0x00) {
					retry = 0;
					printf("MMC card\r\n");
					return 0;
				}
			}

			if (retry == 0xFFFF) {
				printf("CMD1 error, r1=0x%x\r\n", r1);
				return -1;
			}
		}
	}

	printf("Unsupported card type\r\n");
	return -1;
}

static int card_init(void)
{
	int ret;
	uint8_t r1;
	uint16_t retry;

	/* Start send 74 clocks at least */
	for (retry = 0; retry < 10; retry++)
		spi_txrx(DUMMY_BYTE);

	/* Set CS to low */
	//lpc17xx_ssp0_card_enable();

	/* Go to idle state */
	for (retry = 0; retry < 0xFFF; retry++) {
		r1 = send_command(CMD0, 0, 0x95);
		if (r1 == IDLE) {
			retry = 0;
			break;
		}
	}

	/* Timeout */
	if (retry == 0xFFF) {
		printf("%s: reset card failed\r\n", __func__);
		return -1;
	}

	printf("%s: reset card OK\r\n", __func__);

	ret = check_card_type();
	if (ret < 0)
		return ret;

	/* Set valid block size for block oriented data */
	r1 = send_command(CMD16, BLOCK_SIZE, 0);
	if (r1) {
		printf("%s: can't set block data size, r1=%d\r\n", __func__, r1);
		return -1;
	}

	/* CRC disable */
	r1 = send_command(CMD59, 0, 0);
	if (r1) {
		printf("%s: can't disable CRC, r1=%d\r\n", __func__, r1);
		return -1;
	}

	return 0;
}

static int card_get_info(void)
{
	int i;
	uint8_t r1;
	uint8_t csd[20];
	uint8_t cid[20];
	uint32_t read_blen; /* read block length */
	uint32_t v, m, nr_blocks;
	uint32_t capacity;
	struct card_info card_info;

	memset(csd, 0, sizeof(csd));
	memset(cid, 0, sizeof(cid));

	/* Read CSD */
	r1 = send_command(CMD9, 0, 0xFF);
	if (r1) {
		printf("CMD9 error, r1=0x%x\r\n", r1);
		return -1;
	}

	read_buffer(csd, sizeof(csd), 1);

	/* Read CID */
	r1 = send_command(CMD10, 0, 0xFF);
	if (r1) {
		printf("CMD10 error, r1=0x%x\r\n", r1);
		return -1;
	}

	read_buffer(cid, sizeof(cid), 1);

	read_blen = 1 << (csd[5] & 0x0F);

	v = ((csd[6] << 16 | csd[7] << 8 | csd[8]) >> 6) & 0x0FFF;
	m = ((csd[9] << 8 | csd[10]) >> 7) & 0x07;
	nr_blocks = (v + 1) << (m + 2);

	printf("CSD: ");
	for (i = 0; i < 16; i++)
		printf("%x ", csd[i]);
	printf("\r\n");

	printf("CID: ");
	for (i = 0; i < 16; i++)
		printf("%x ", cid[i]);
	printf("\r\n");

	printf("%s: read block length: %d bytes\r\n", __func__, read_blen);
	printf("%s: block number: %d\r\n", __func__, nr_blocks);
	printf("%s: %c%c%c%c%c%c\r\n", __func__,
		cid[3], cid[4], cid[5], cid[6], cid[7], cid[8]);

	return 0;
}

static int card_read_block(uint32_t arg, uint8_t *block)
{
	uint8_t r1;

	arg <<= 9;
	printf("%s: arg=%d (%X)\r\n", __func__, arg, arg);

	/* Read single block - start data transmit*/
	r1 = send_command(CMD17, arg, 0);
	if (r1) {
		printf("%s: CMD17 error, r1=%d\r\n", __func__, r1);
		return r1;
	}

	read_buffer(block, BLOCK_SIZE, 1);

	/* Stop data transmit*/
	send_command(CMD12, 0, 0);

	return 0;
}
