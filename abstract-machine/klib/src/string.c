#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len] != '\0') len += 1;
  return len;
}

char *strcpy(char *dst, const char *src) {
  size_t index = 0;
  while (src[index] != '\0') {
	  dst[index] = src[index];
	  index++;
  }

  dst[index] = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t index = 0;
  for (; index < n && src[index] != '\0'; index += 1) 
    dst[index] = src[index];
  for (; index < n; index += 1) 
    dst[index] = '\0';
  return dst;
  
}

char *strcat(char *dst, const char *src) {
  size_t len = strlen(dst);
  size_t index = 0;
  while (src[index] != '\0') dst[len + index] = src[index];
  dst[len + index] = '\0';
  return dst;
}


int strcmp(const char *s1, const char *s2) {
  size_t i;
  for (i = 0; (s1[i] || s2[i]); i += 1) {
    if (s1[i] < s2[i]) return -1;
    else if (s1[i] > s2[i]) return 1;
  }
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i;
  for (i = 0; i < n && (s1[i] || s2[i]); i += 1) {
    if (s1[i] < s2[i]) return -1;
    else if (s1[i] > s2[i]) return 1;
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
  for (size_t i = 0; i < n; i += 1) ((unsigned char*)s)[i] = (unsigned char)c;
  return s;

}

void *memmove(void *dst, const void *src, size_t n) {
  void * temp = malloc(n);
  assert(temp);
  memcpy(temp, src, n);
  memcpy(dst, temp, n);
  free(temp);
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  for (size_t i = 0; i < n; i += 1) 
    ((unsigned char *)out)[i] = ((unsigned char *)in)[i];
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  for (size_t i = 0; i < n; i += 1) {
    if (((unsigned char*)s1)[i] < ((unsigned char*)s2)[i]) return -1;
    else if (((unsigned char*)s1)[i] > ((unsigned char*)s2)[i]) return 1;
  }
  return 0;
}

#endif
