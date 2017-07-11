#include <_stdlib.h>
#include <stdarg.h>
#include <uart.h>

char *putchar(char *s, char c);
char *putdec(char *s, uint32_t byte);
char *puthexi(char *s, uint32_t dword);
char *puthex(char *s, uint8_t byte);
char *puthexd(char *s, uint8_t digit);
char *puts(char *s, const char *str);
void printf(const char *fmt, ...);
void dvprintf(char *s, const char *fmt, va_list args);

/* TODO */

void memset(void *dst, int pattern, size_t size)
{
	int i;

	if (!dst)
		return;

	for (i = 0; i < size; i++)
		*((uint8_t *)dst + i) = (uint8_t)pattern;
}

size_t memcpy(void *dst, const void *src, size_t size)
{
	int i;

	if (!dst || !src)
		return 0;

	for (i = 0; i < size; i++)
		*((uint8_t *)dst + i) = *((uint8_t *)src + i);

	return size;
}

int strlen(const char *src)
{
	int i = 0;

	if (src)
		for (;src[i] != 0x00; i++);

	return i;
}

int strcmp(const char *src, const char *dst)
{
	int i, len;

	if (!src || !dst)
		return -1;

	len = strlen(src);

	for (i = 0; i < len; i++) {
		if (src[i] != dst[i])
			return -1;
	}

	if (dst[i] != 0x00)
		return -1;

	return 0;
}

char *strsep(char **stringp, const char *delim)
{
	char *token;

	if (*stringp == NULL || (*stringp)[0] == 0)
		return NULL;

	for (token = *stringp; *(*stringp); (*stringp)++) {
		if (*(*stringp) == *delim) {
			*(*stringp)++ = 0;
			break;
		}
	}

	return token;
}

int atoi(const char *s)
{
	int ret = 0;

	if (*s) {
		while (*s && *s >= '0' && *s <= '9') {
			ret = (ret * 10) + (*s - '0');
			s++;
		}
	}

	return ret;
}

char *itoa(int n, char *s)
{
	int ret, i = 0;
	char *p;

	if (n < 0)
		goto out;

	if (n == 0) {
		*s = '0';
		goto out;
	}

	for (ret = n; ret > 0; ret /= 10, i++);

	p = s;
	while (n) {
		ret = n % 10;
		p[--i] = ret + '0';
		n /= 10;
	}

out:
	return s;
}

void bzero(char *s, unsigned int sz)
{
	memset(s, 0, sz);
}

char *putchar(char *s, char c)
{
	*s = c;
	return (s + 1);
}

char *putdec(char *s, uint32_t byte)
{
	unsigned char b1;
	int b[30];
	signed int nb;
	int i = 0;

	while (1) {
		b1 = byte % 10;
		b[i] = b1;
		nb = byte / 10;
		if (nb <= 0)
			break;
		i++;
		byte = nb;
	}

	for (nb = i + 1; nb > 0; nb--)
		s = puthexd(s, b[nb - 1]);

	return s;
}

char *puthexi(char *s, uint32_t dword)
{
	s = puthex(s, (dword & 0xFF000000) >> 24);
	s = puthex(s, (dword & 0x00FF0000) >> 16);
	s = puthex(s, (dword & 0x0000FF00) >> 8);
	s = puthex(s, (dword & 0x000000FF));

	return s;
}

char *puthex(char *s, uint8_t byte)
{
	unsigned char lb, rb;

	lb = byte >> 4;
	rb = byte & 0x0F;

	s = puthexd(s, lb);
	s = puthexd(s, rb);

	return s;
}

char *puthexd(char *s, unsigned char digit)
{
	char table[]="0123456789ABCDEF";
	return putchar(s, table[digit]);
}

char *puts(char *s, const char *str)
{
	while (*str) {
		s = putchar(s, *str);
		str++;
	}

	return s;
}

void dvprintf(char *s, const char *fmt, va_list args)
{
	char *tmp = s;
	while (*fmt) {
		switch (*fmt) {
		case '%':
			fmt++;
			switch (*fmt) {
			case 's':
				s = puts(s, va_arg(args, char *));
				break;
			case 'c':
				s = putchar(s, va_arg(args, unsigned int));
				break;
			case 'd':
				s = putdec(s, va_arg(args, unsigned int));
				break;
			case 'x':
				s = puthex(s, va_arg(args, unsigned int));
				break;
			case 'X':
				s = puthexi(s, va_arg(args, unsigned int));
				break;
			}
			break;
		default:
			s = putchar(s, *fmt);
			break;
		}
		fmt++;
	}

	s = tmp;
}

void printf(const char *fmt, ...)
{
	char s[80], *p;
	va_list args;

	memset(s, 0, sizeof(s));

	va_start(args, fmt);
	dvprintf(&s[0], fmt, args);
	va_end(args);

	p = s;
	do {
		uart_send_byte(2, *p);
	} while (*p++);
}

void sprintf(char *s, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	dvprintf(s, fmt, args);
	va_end(args);
}
