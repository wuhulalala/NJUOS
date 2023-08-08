#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#define MAX_INT 100
#define MASK(add, digit) (((add) >> (digit) * 4) & 0xf)
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list ap;
	va_start(ap, fmt);
	char buffer[512] = {0};
	char outBuf[512] = {0};
	strcpy(buffer, fmt);
	char *pb = buffer, *po = outBuf;
	int tempInt = 0;
	int tempChar = 0;
	char *tempPointer = NULL;
	unsigned long tempAddress = 0;
	int hex = 0;
	int lastIsNotFormater = 1;
	while (*pb != '\0') {
		if (*pb != '%' && lastIsNotFormater) {
			*po++ = *pb++;
			continue;
		} else if (*pb == '%'){
			pb++;
			lastIsNotFormater = 0;
		} else {
			switch (*pb) {
				case 'd':
					tempInt = va_arg(ap, int);
					if (tempInt < 0) *po++ = '-';
					if (tempInt == 0) *po++ = '0';
					tempInt = abs(tempInt);
					char num[MAX_INT] = {0};
					int digit = 0;
					for (int t = tempInt % 10; tempInt != 0; tempInt = tempInt / 10, t = tempInt % 10) {
						num[digit++] = t + '0';
					}
					while (digit - 1 >= 0) {
						*po++ = num[digit - 1];
						digit--;
					}
					break;
				case 'c':
					tempChar = va_arg(ap, int);
					*po++ = tempChar;
					break;
				case 's':
					tempPointer = (char*)va_arg(ap, char*);
					while (*tempPointer) {
						*po++ = *tempPointer++;
					}
					break;
				case 'p':
					*po++ = '0';
					*po++ = 'x';
					tempAddress = va_arg(ap, unsigned long);
					if (tempAddress == 0) {*po++ = '0'; break;}
					int startFlag = 1;
					for (int i = 15; i >= 0; i--) {
						__uint64_t temp = tempAddress;
						hex = MASK(temp, i);
						if (hex == 0 && startFlag == 1) continue;
						else if (hex < 10) {
							startFlag = 0;
							*po++ = hex + '0';
						} else {
							startFlag = 0;
							*po++ = hex - 10 + 'a';
						}
					}
					break;

					
					
			}
			pb++;
			lastIsNotFormater = 1;
			continue;
		}
	}
	va_end(ap);
	*po = '\0';
	int count = 0;
	for (po = outBuf; *po; po++, count++) putch(*po);
	return count;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
