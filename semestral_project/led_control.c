#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "font_types.h"
#include "graphics.h"
#include "game_state.h"
#include "led_control.h"
#include "knobs.h"

void turn_ledstrip_on(unsigned char* led_mem_base){
	*(volatile uint32_t*)(led_mem_base + SPILED_REG_LED_LINE_o ) = 0xFFFFFFFF;
}

void ledstrip_value(unsigned char* led_mem_base, uint32_t value){
	*(volatile uint32_t*)(led_mem_base + SPILED_REG_LED_LINE_o ) = value;
}

void led1_value(unsigned char* led_mem_base, uint32_t value){
	*(volatile uint32_t*)(led_mem_base+ SPILED_REG_LED_RGB1_o) = value;
}

void led2_value(unsigned char* led_mem_base, uint32_t value){
	*(volatile uint32_t*)(led_mem_base + SPILED_REG_LED_RGB2_o) = value;
}
