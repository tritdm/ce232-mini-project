#ifndef __OLED_TASK__
#define __OLED_TASK__
#include "i2c_init.h"

void ssd1306_init(void);

void task_ssd1306_display_text(void *arg_text);

void task_ssd1306_display_clear(void *ignore);

#endif