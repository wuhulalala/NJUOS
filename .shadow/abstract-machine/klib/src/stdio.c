#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  char buffer[256];//Be tolerant with my_printf, never try to jump ovet the band of the buffer -- buffer overflow
 
  int temp = 0;
  va_list arg;

  char* p_string = NULL;
  char* p_buffer = buffer;
  char* p_temp   = NULL;

  int counter = 0;
  int number  = 0;
  int foo     = 0;
  
  va_start(arg,fmt);
  char fmt_copy[256];
  strcpy(fmt_copy, fmt);
  for(counter = 0,p_string = fmt_copy;*(p_string) != '\0';)
  {
          switch(*p_string)
          {
                  case '%':
                          p_string++;

                          switch(*p_string)
                          {
                                  case 'd':

                                          temp = va_arg(arg,int);

                                          foo = temp;

                                          while(foo)
                                          {
                                                  number++;
                                                  counter++;
                                                  foo /= 10;
                                          }

                                          foo = temp;

                                          while(number)
                                          {
                                                  *(p_buffer+number-1) = (foo%10);
                                                  foo /= 10;
                                                  number--;
                                          }

                                          p_buffer += number;
                                  break;

                                  case 'c':
                                          temp = va_arg(arg,int);
                                          *(p_buffer++) = temp;
                                  break;

                                  case 's':
                                          p_temp = va_arg(arg,char*);

                                          while(p_temp != NULL)
                                          {
                                                  *(p_buffer++) = *(p_temp++);
                                                  counter++;
                                          }
                                  break;

                                  default:
                                          break;

                          }
                  break;

                  default:
                          *(p_buffer++) = *(p_string++);
                          counter++;
          }
  }

  va_end(arg);

  p_buffer = NULL;
  for (char *p = buffer; *p; p++) putch(*p);
  return counter;

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
