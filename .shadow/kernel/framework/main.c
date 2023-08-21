#include <kernel.h>
#include <klib.h>

int sum = 0;
int main() {
  ioe_init();
  cte_init(os->trap);
  printf("Hello World\n");
  os->init();
  mpe_init(os->run);
  return 1;
}
