#include <kernel.h>
#include <klib.h>

int sum = 0;
int main() {
  //ioe_init();
  //cte_init(os->trap);
  os->init();
  mpe_init(os->run);
  return 1;
}
