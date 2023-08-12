#include <kernel.h>
#include <klib.h>

int sum = 0;
int main() {
  os->init();
  mpe_init(os->run);
  return 1;
}
