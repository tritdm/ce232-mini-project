#ifndef __OLED_TASK__
#define __OLED_TASK__
#include "esp_log.h"
#include "i2c_init.h"
#include "ssd1306.h"
#include "font8x8_basic.h"

void ssd1306_init(void);

void task_ssd1306_display_text(const char *text);

void task_ssd1306_display_clear(void *ignore);

#endif