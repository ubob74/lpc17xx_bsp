#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include <_stdint.h>

typedef struct {
	int counter;
} atomic_t;

void atomic_init(atomic_t *);
void atomic_set(int, atomic_t *);
int atomic_read(atomic_t *);
void atomic_inc(atomic_t *);
void atomic_dec(atomic_t *);
void atomic_add(int, atomic_t *);
void atomic_sub(int, atomic_t *);

#endif /* _ATOMIC_H_ */
