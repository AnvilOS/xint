
#ifndef TIME_STAMP_H
#define TIME_STAMP_H

#if !defined __arm__
#define __disable_irq()
#define __enable_irq()
#define STAMP_VARS() clock_t start, end;
#define STAMP_BEFORE() start = clock();
#define STAMP_AFTER() end = clock();
#define STAMP_DIFF() ((unsigned)((end - start) * 1000 / CLOCKS_PER_SEC))
#else
#include "stm32u5xx_hal.h"
#define STAMP_VARS() unsigned long before; unsigned long after;
#define STAMP_BEFORE() before = DWT->CYCCNT
#define STAMP_AFTER()  after = DWT->CYCCNT
#define STAMP_DIFF() (after-before)
#endif

#endif /* TIME_STAMP_H */
