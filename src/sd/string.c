#include <stddef.h>
#include <stdint.h>

void* memcpy(void* dest, const void* src, size_t n) {
	unsigned char* d = dest;
	const unsigned char* s = src;
	while (n--)
		*d++ = *s++;
	return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
	const unsigned char* p1 = (const unsigned char*)s1;
	const unsigned char* p2 = (const unsigned char*)s2;

	while (n--) {
		if (*p1 != *p2) {
			return *p1 - *p2;
		}
		p1++;
		p2++;
	}
	return 0;
}

void* memset(void* s, int c, size_t n) {
	unsigned char* p = (unsigned char*)s;

	while (n--) {
		*p++ = (unsigned char)c;
	}

	return s;
}

char* strchr(const char* s, int c) {
	while (*s) {
		if (*s == (char)c) {
			return (char*)s; // Zwraca wskaźnik do znalezionego znaku
		}
		s++;
	}

	return c == '\0' ? (char*)s : NULL; // Jeśli szukamy '\0', zwróć wskaźnik do końca
}

size_t strlen(const char* str) {
	size_t length = 0;
	while (str[length] != '\0') {
		length++;
	}
	return length;
}
