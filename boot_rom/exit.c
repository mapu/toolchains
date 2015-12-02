/*
 * exit.c
 *
 *  Created on: Jan 18, 2014
 *      Author: wangl
 */
#include "soc_mmap.h"
void exit_sim(int no) {
  volatile int *gpio_addr = (int *)GPIO_START;
  if (no == 0) {
    gpio_addr[0x18/4] = 0x5a9b;//passed
  } else {
    gpio_addr[0x18/4] = 0x5a9a;//error occurs
  }
}
