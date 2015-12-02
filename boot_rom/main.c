// Here is the tranditional main function
#include "soc_mmap.h"
#include "dma_ctrl.h"
#include "partitions.h"

#define UBOOT_LOAD_BASE 0x10000
#define UBOOT_SIZE 0x70000
#define UBOOT_TEXT_BASE 0x60800000

int main(void) {
  DMAtrans(UBOOT_LOAD_BASE, UBOOT_TEXT_BASE, UBOOT_SIZE/4, TR_W_32, TR_W_32);
  void (*e)() = (void(*)())0x60800000;
  e();
  return 0;
}
