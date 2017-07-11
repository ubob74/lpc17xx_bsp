#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <_stdint.h>
#include <stdarg.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

void memset(void *dst, int pattern, size_t size);
size_t memcpy(void *dst, const void *src, size_t size);
int strcmp(const char *src, const char *dst);
int strlen(const char *src);
void printf(const char *fmt, ...);
void sprintf(char *s, const char *fmt, ...);

char *strsep(char **stringp, const char *delim);
int atoi(const char *s);
char *itoa(int n, char *s);
void bzero(char *s, unsigned int sz);

#endif /* _STDLIB_H_ */
