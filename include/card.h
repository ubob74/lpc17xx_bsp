#ifndef _CARD_H_
#define _CARD_H_

#include <_stdint.h>

#pragma pack(1)

#define BLOCK_SIZE	512

/* Card states */
#define IDLE	0x1

/* Commands */
#define CMD0	0x00
#define CMD1	0x01
#define CMD8	0x08
#define CMD9	0x09
#define CMD10	0x0A
#define CMD12	0x0C
#define CMD16	0x10
#define CMD17	0x11
#define CMD18	18
#define CMD23	23
#define CMD55	55
#define CMD59	0x3B

#define ACMD41	41

struct csd {
	uint8_t data[41];
};

struct cid {
	uint8_t data[18];
};

struct card_info {
	struct csd csd;
	struct cid cid;
	uint32_t capacity;
	uint32_t block_size;
	uint16_t rca;
	uint8_t card_type;
	uint32_t space_total;
	uint32_t space_free;
};

#endif
