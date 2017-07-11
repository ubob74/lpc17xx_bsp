#include <atomic.h>

inline void atomic_init(atomic_t *v)
{
	v->counter = 0;
}

inline void atomic_set(int i, atomic_t *v)
{
	v->counter = i;
}

inline int atomic_read(atomic_t *v)
{
	return v->counter;
}

inline void atomic_inc(atomic_t *v)
{
	atomic_add(1, v);
}

inline void atomic_dec(atomic_t *v)
{
	atomic_sub(1, v);
}

inline void atomic_add(int i, atomic_t *v)
{
	v->counter += i;
}

inline void atomic_sub(int i, atomic_t *v)
{
	v->counter -= i;
}
