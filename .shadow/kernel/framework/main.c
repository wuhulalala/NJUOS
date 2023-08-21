#include <kernel.h>
#include <klib.h>

int i;
int sum = 0;
int main() {
  ioe_init();
  cte_init(os->trap);
  os->init();
  i = 0;
  printf("the i is %d\n", i);
  mpe_init(os->run);
  return 1;
}
