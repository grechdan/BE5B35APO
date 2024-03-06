#ifndef LED_CONTROL_H
#define LED_CONTROL_H
#define LCD_WIDTH 480
#define LCD_HEIGHT 320
#include <stdint.h>
void turn_ledstrip_on(unsigned char* led_mem_base);

void ledstrip_value(unsigned char* led_mem_base, uint32_t value);

void led1_value(unsigned char* led_mem_base, uint32_t value);

void led2_value(unsigned char* led_mem_base, uint32_t value);
#endif

