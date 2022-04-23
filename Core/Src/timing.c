
#include "stm32f1xx_hal.h"

void uSecDelay(unsigned int i)
{
  static int start;
  start = (int)SysTick->VAL;
  int end = start -  (72 * i);
  if (end < 0)
    end +=72000;

  if (end > start)
  { // wait for it to wrap:
    while (SysTick->VAL < start)
      ;
  }
  while (end < SysTick->VAL)
    __no_operation();
}
