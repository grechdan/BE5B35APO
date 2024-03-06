#define _POSIX_C_SOURCE 200112L

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

#define LCD_WIDTH 480
#define LCD_HEIGHT 320

union pixel buffer[LCD_WIDTH][LCD_HEIGHT];
ship main_ship;
font_descriptor_t* font14x16 = &font_winFreeSystem14x16;
struct timespec delay_amount = {
	.tv_nsec = 300000000,
	.tv_sec = 0
};

int main(int argc, char *argv[])
{

	unsigned char *parlcd_reg_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	if (parlcd_reg_base == NULL) exit(1);
	parlcd_write_cmd(parlcd_reg_base, 0x2c);

	unsigned char* led_mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	if (led_mem_base == NULL) exit(1);

	unsigned char *knobs_mem_base = init_mem_base_knobs();

	fill_display(buffer,0, 0, 0); // Clear display
	update_display(buffer, parlcd_reg_base);
	
	uint32_t loading = 0xf0000000;
	
	for (int i = 0; i < 8; i++){
		loading = (loading >> i) | 0xff000000;
		ledstrip_value(led_mem_base, loading);
		led2_value(led_mem_base, loading);
		clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_amount, NULL); // Loading animation with LEDs
	}

	menu(buffer, parlcd_reg_base, led_mem_base, knobs_mem_base); // Goes to MENU

  return 0;
}
