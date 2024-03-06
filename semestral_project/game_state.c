#define _POSIX_C_SOURCE 202012L
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
#include "game.h"

#define LCD_WIDTH 480
#define LCD_HEIGHT 320
#define MAX_INVADER_BULLETS 10
#define RED 1
#define GREEN 2
#define BLUE 3

#define SMALL 0
#define LARGE 1

#define NORMAL_D 0
#define EASY 1
#define DIFFICULT 2
#define PEACEFUL 3

#define NORMAL_S 0
#define FAST 1
#define SLOW 2

int fsize = 0; // 0, 1
int game_diff = 0; // 0, 1, 2, 3
int game_speed = 0; // 0, 1, 2
int first_load = 1;
int knob_checker = 1;
int knob_state = 0;

void menu(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base, unsigned char *led_mem_base, unsigned char *knobs_mem_base){
	led2_value(led_mem_base, 0x00000000);
	led1_value(led_mem_base, 0x00000000);
	ledstrip_value(led_mem_base, 0x00000000);
	font_descriptor_t* font14x16 = &font_winFreeSystem14x16;
	font_descriptor_t* font8x16 = &font_rom8x16;
	const unsigned char invaders_map[] = {0x5C,0xB6,0x3F,0xB6,0x5C};

	struct timespec delay_amount = {
		.tv_nsec = 500000000,
		.tv_sec = 0
	};
	struct timespec slower_delay_amount = {
		.tv_nsec = 100000000,
		.tv_sec = 0
	};

	unsigned char title1[] = "SPACE";
	unsigned char title2[] = "REMAINDERS";
	unsigned char str1[] = "PLAY:";
	unsigned char str2[] = "SETTINGS:";
	unsigned char str3[] = "How to play?:";
	unsigned char str4[] = "By: JB & DG";

	while(1){
		if (knob_state == 0){
			knob_checker = 1;
		}
		fill_display(buffer, 0, 0, 0);
		if (first_load == 1){
			first_load = 0;
			draw_text(buffer, 173, 10, 3, 3, 255, 255, 255, font14x16, title1); // Creating a loading animation
			update_display(buffer, parlcd_reg_base);
			clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_amount, NULL);
			draw_text(buffer, 100, 50, 3, 3, 255, 255, 255, font14x16, title2);
			update_display(buffer, parlcd_reg_base);
			clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_amount, NULL);
			draw_line(buffer, 125, 100, 2, 160, 220, 255);
			update_display(buffer, parlcd_reg_base);
			clock_nanosleep(CLOCK_MONOTONIC, 0, &slower_delay_amount, NULL);
			draw_line(buffer, 125, 102, 2, 160, 220, 255);
			update_display(buffer, parlcd_reg_base);
			clock_nanosleep(CLOCK_MONOTONIC, 0, &slower_delay_amount, NULL);
			draw_line(buffer, 125, 104, 2, 160, 220, 255);
			update_display(buffer, parlcd_reg_base);
			clock_nanosleep(CLOCK_MONOTONIC, 0, &slower_delay_amount, NULL);
			draw_line(buffer, 125, 106, 2, 160, 220, 255);
			update_display(buffer, parlcd_reg_base);
			clock_nanosleep(CLOCK_MONOTONIC, 0, &slower_delay_amount, NULL);
			draw_line(buffer, 125, 108, 2, 160, 220, 255);
			update_display(buffer, parlcd_reg_base);
			clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_amount, NULL);
			draw_text(buffer, 20, 130, 2, 2, 255, 255, 255, font8x16, str1);
			draw_text(buffer, 20, 170, 2, 2, 255, 255, 255, font8x16, str2);
			draw_text(buffer, 20, 210, 2, 2, 255, 255, 255, font8x16, str3);
		}

		uint8_t knob_input = rgb_knob_value(knobs_mem_base, RED); // Control the cursor
		knob_state = rgb_knob_status(knobs_mem_base, RED); // Select

		if (fsize == 0){
			if (knob_input >= 0 && knob_input < 60) {
				objects2buffer_small(buffer, 250, 130, 5, invaders_map); // used as cursor

				if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					game(buffer, parlcd_reg_base, led_mem_base, knobs_mem_base,game_diff, game_speed);
				}
			} else if (60 >= knob_input && knob_input < 120) {
				objects2buffer_small(buffer, 250, 170, 5, invaders_map);

				if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					settings(buffer, parlcd_reg_base, led_mem_base, knobs_mem_base);
				}
			} else {
				objects2buffer_small(buffer, 250, 210, 5, invaders_map);

				if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					how_to_play(buffer, parlcd_reg_base, led_mem_base, knobs_mem_base);
				}
			}
			draw_line(buffer,125, 100, 10, 160, 220, 255);
			draw_text(buffer, 173, 10, 3, 3, 255, 255, 255, font14x16, title1);
			draw_text(buffer, 100, 50, 3, 3, 255, 255, 255, font14x16, title2);
			draw_text(buffer, 20, 130, 2, 2, 255, 255, 255, font8x16, str1);
			draw_text(buffer, 20, 170, 2, 2, 255, 255, 255, font8x16, str2);
			draw_text(buffer, 20, 210, 2, 2, 255, 255, 255, font8x16, str3);
			draw_text(buffer, 20, 300, 1, 1, 255, 255, 255, font8x16, str4);
			draw_settings(buffer);
			update_display(buffer, parlcd_reg_base);
		} else {
			// Control the cursor
			if (knob_input >= 0 && knob_input < 60) {
				objects2buffer(buffer, 320, 135, 5, invaders_map); // used as cursor

				if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					game(buffer, parlcd_reg_base, led_mem_base, knobs_mem_base, game_diff, game_speed);
				}
			} else if (60 >= knob_input && knob_input < 120) {
				objects2buffer(buffer, 320, 195, 5, invaders_map);

				if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					settings(buffer, parlcd_reg_base, led_mem_base, knobs_mem_base);
				}
			} else {
				objects2buffer(buffer, 320, 255, 5, invaders_map);

				if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					how_to_play(buffer, parlcd_reg_base, led_mem_base, knobs_mem_base);
				}
			}
			draw_line(buffer,125, 100, 10, 160, 220, 255);
			draw_text(buffer, 173, 10, 3, 3, 255, 255, 255, font14x16, title1);
			draw_text(buffer, 100, 50, 3, 3, 255, 255, 255, font14x16, title2);
			draw_text(buffer, 20, 140, 3, 3, 255, 255, 255, font14x16, str1);
			draw_text(buffer, 20, 200, 3, 3, 255, 255, 255, font14x16, str2);
			draw_text(buffer, 20, 260, 3, 3, 255, 255, 255, font14x16, str3);
			update_display(buffer, parlcd_reg_base);
		}
	}
}

void settings(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base, unsigned char *led_mem_base, unsigned char *knobs_mem_base){
	font_descriptor_t* font14x16 = &font_winFreeSystem14x16;
	font_descriptor_t* font8x16 = &font_rom8x16;
	const unsigned char invaders_map[] = {0x5C,0xB6,0x3F,0xB6,0x5C};

	unsigned char setting_title[] = "SETTINGS";
	unsigned char setting1[] = "FONT SIZE:";
	unsigned char setting2[] = "GAME SPEED:";
	unsigned char setting3[] = "GAME DIFFICULTY:";
	unsigned char setting4[] = "BACK:";
	unsigned char large_setting4[] = "B:";
	
	unsigned char fsize1[] = "LARGE";
	unsigned char fsize2[] = "SMALL";

	unsigned char speed1[] = "NORMAL";
	unsigned char speed2[] = "FAST";
	unsigned char speed3[] = "SLOW";
	
	unsigned char diff1[] = "NORMAL";
	unsigned char diff2[] = "EASY";
	unsigned char diff3[] = "DIFFICULT";
	unsigned char diff4[] = "PEACEFUL";

	unsigned char large_diff1[] = "N";
	unsigned char large_diff2[] = "E";
	unsigned char large_diff3[] = "D";
	unsigned char large_diff4[] = "P";
	
	while(1){
		// Settings structure
		if (knob_state == 0){
			knob_checker = 1;
		} 
		fill_display(buffer, 0, 0, 0);
		draw_line(buffer,125, 100, 10, 160, 220, 255);
		draw_text(buffer, 135, 50, 3, 3, 255, 255, 255, font14x16, setting_title);
		
		
		uint8_t knob_input = rgb_knob_value(knobs_mem_base, RED); // Control the cursor
		uint8_t knob_input_green = rgb_knob_value(knobs_mem_base, GREEN);
		knob_state = rgb_knob_status(knobs_mem_base, RED); // Select

		if (fsize == 0){
			if (knob_input >= 0 && knob_input < 60) { // FONT SIZE
				
				if (knob_input_green >= 0 && knob_input_green < 50){
					draw_text(buffer, 200, 130, 2, 2, 255, 255, 255, font8x16, fsize2);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					draw_text(buffer, 200, 130, 2, 2, 100, 255, 100, font8x16, fsize2);
					fsize = SMALL;
				}
				} else {
					draw_text(buffer, 200, 130, 2, 2, 255, 255, 255, font14x16, fsize1);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					draw_text(buffer, 200, 130, 2, 2, 100, 255, 100, font14x16, fsize1);
					fsize = LARGE;
					}
				}

				
			} else if (60 >= knob_input && knob_input < 120) { // GAME SPEED
				if (knob_input_green >= 0 && knob_input_green < 50){
					draw_text(buffer, 220, 170, 2, 2, 255, 255, 255, font8x16, speed1);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					draw_text(buffer, 220, 170, 2, 2, 100, 255, 100, font8x16, speed1);
					game_speed = NORMAL_S;
					}
				} else if (knob_input_green >= 50 && knob_input_green < 100){
					draw_text(buffer, 220, 170, 2, 2, 255, 255, 255, font14x16, speed2);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					draw_text(buffer, 220, 170, 2, 2, 100, 255, 100, font14x16, speed2);
					game_speed = FAST;
					}
				} else {
					draw_text(buffer, 220, 170, 2, 2, 255, 255, 255, font14x16, speed3);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					draw_text(buffer, 220, 170, 2, 2, 100, 255, 100, font14x16, speed3);
					game_speed = SLOW;
					}
				}
			} else if (120 >= knob_input && knob_input < 180){ // GAME DIFFICULTY
				if (knob_input_green >= 0 && knob_input_green < 50){
					draw_text(buffer, 300, 210, 2, 2, 255, 255, 255, font8x16, diff1);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					draw_text(buffer, 300, 210, 2, 2, 100, 255, 100, font8x16, diff1);
					game_diff = NORMAL_D;
					}
				} else if (knob_input_green >= 50 && knob_input_green < 100){
					draw_text(buffer, 300, 210, 2, 2, 255, 255, 255, font14x16, diff2);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					draw_text(buffer, 300, 210, 2, 2, 100, 255, 100, font14x16, diff2);
					game_diff = EASY;
					}
				} else if (knob_input_green >= 100 && knob_input_green < 150){
					draw_text(buffer, 300, 210, 2, 2, 255, 255, 255, font14x16, diff3);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					draw_text(buffer, 300, 210, 2, 2, 100, 255, 100, font14x16, diff3);
					game_diff = DIFFICULT;
					}
				} else {
					draw_text(buffer, 300, 210, 2, 2, 255, 255, 255, font14x16, diff4);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					draw_text(buffer, 300, 210, 2, 2, 100, 255, 100, font14x16, diff4);
					game_diff = PEACEFUL;
					}
				}
			} else { // BACK
				objects2buffer_small(buffer, 250, 250, 5, invaders_map);
				if (knob_state == 1) {
					knob_checker = 0;
					menu(buffer, parlcd_reg_base, led_mem_base, knobs_mem_base);
				}
			}
			
			draw_text(buffer, 20, 130, 2, 2, 255, 255, 255, font8x16, setting1);
			draw_text(buffer, 20, 170, 2, 2, 255, 255, 255, font8x16, setting2);
			draw_text(buffer, 20, 210, 2, 2, 255, 255, 255, font8x16, setting3);
			draw_text(buffer, 20, 250, 2, 2, 255, 255, 255, font8x16, setting4);
			draw_settings(buffer);
			update_display(buffer, parlcd_reg_base);
			
		} else { // LARGE FONT
			if (knob_input >= 0 && knob_input < 60) { // FONT SIZE
				
				if (knob_input_green >= 0 && knob_input_green < 50){
					draw_text(buffer, 250, 140, 3, 3, 255, 255, 255, font14x16, fsize2);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					fsize = SMALL;
				}
				} else {
					draw_text(buffer, 250, 140, 3, 3, 255, 255, 255, font14x16, fsize1);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					fsize = LARGE;
					}
				}

				
			} else if (60 >= knob_input && knob_input < 120) { // GAME SPEED
				if (knob_input_green >= 0 && knob_input_green < 50){
					draw_text(buffer, 300, 200, 3, 3, 255, 255, 255, font14x16, speed1);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					game_speed = NORMAL_S;
					}
				} else if (knob_input_green >= 50 && knob_input_green < 100){
					draw_text(buffer, 300, 200, 3, 3, 255, 255, 255, font14x16, speed2);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					game_speed = FAST;
					}
				} else {
					draw_text(buffer, 300, 200, 3, 3, 255, 255, 255, font14x16, speed3);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					game_speed = SLOW;
					}
				}
			} else if (120 >= knob_input && knob_input < 180){ // GAME DIFFICULTY
				if (knob_input_green >= 0 && knob_input_green < 50){
					draw_text(buffer, 415, 260, 3, 3, 255, 255, 255, font14x16, large_diff1);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					game_diff = NORMAL_D;
					}
				} else if (knob_input_green >= 50 && knob_input_green < 100){
					draw_text(buffer, 415, 260, 3, 3, 255, 255, 255, font14x16, large_diff2);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					game_diff = EASY;
					}
				} else if (knob_input_green >= 100 && knob_input_green < 150){
					draw_text(buffer, 415, 260, 3, 3, 255, 255, 255, font14x16, large_diff3);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					game_diff = DIFFICULT;
					}
				} else {
					draw_text(buffer, 415, 260, 3, 3, 255, 255, 255, font14x16, large_diff4);
					if (knob_state == 1 && knob_checker == 1) {
					knob_checker = 0;
					game_diff = PEACEFUL;
					}
				}
			} else { // BACK
				objects2buffer_small(buffer, 430, 15, 5, invaders_map);
				if (knob_state == 1) {
					knob_checker = 0;
					menu(buffer, parlcd_reg_base, led_mem_base, knobs_mem_base);
				}
			}
			
			draw_text(buffer, 5, 140, 3, 3, 255, 255, 255, font14x16, setting1);
			draw_text(buffer, 5, 200, 3, 3, 255, 255, 255, font14x16, setting2);
			draw_text(buffer, 5, 260, 3, 3, 255, 255, 255, font14x16, setting3);
			draw_text(buffer, 380, 10, 3, 3, 255, 255, 255, font14x16, large_setting4);

			update_display(buffer, parlcd_reg_base);
		}
	}
}

void how_to_play(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base, unsigned char *led_mem_base, unsigned char *knobs_mem_base){
	font_descriptor_t* font14x16 = &font_winFreeSystem14x16;
	font_descriptor_t* font8x16 = &font_rom8x16;
	unsigned char how_to[] = "How to play?";
	unsigned char how1[] = "BLUE JS: <- -> + press to shoot";
	unsigned char how2[] = "RED JS: press to reset";
	unsigned char how3[] = "Obtain points by eliminating invaders";
	unsigned char how4[] = "The more points you have the tougher it gets.";
	unsigned char how5[] = "Let's see how far you make it. ;)";
	unsigned char back[] = "BACK:";
	const unsigned char invaders_map[] = {0x5C,0xB6,0x3F,0xB6,0x5C};
	
	while(1){
	if (knob_state == 0){
		knob_checker = 1;
	} 
	knob_state = rgb_knob_status(knobs_mem_base, RED); // Select
	
	if (knob_state == 1 && knob_checker == 1) {
		knob_checker = 0;
		menu(buffer, parlcd_reg_base, led_mem_base, knobs_mem_base);
	}
	
	fill_display(buffer, 0, 0, 0);
	objects2buffer_small(buffer, 430, 290, 5, invaders_map);
	draw_line(buffer,125, 100, 10, 160, 220, 255);
	draw_text(buffer, 115, 50, 3, 3, 255, 255, 255, font14x16, how_to);
	draw_text(buffer, 80, 130, 1, 1, 255, 255, 255, font8x16, how1);
	draw_text(buffer, 80, 150, 1, 1, 255, 255, 255, font8x16, how2);
	draw_text(buffer, 50, 190, 1, 1, 255, 255, 255, font8x16, how3);
	draw_text(buffer, 50, 220, 1, 1, 255, 255, 255, font8x16, how4);
	draw_text(buffer, 50, 250, 1, 1, 255, 255, 255, font8x16, how5);
	draw_text(buffer, 340, 290, 2, 2, 255, 255, 255, font8x16, back);
	update_display(buffer, parlcd_reg_base);
	}
}

void draw_settings(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base){
	font_descriptor_t* font14x16 = &font_winFreeSystem14x16;
	font_descriptor_t* font8x16 = &font_rom8x16;
	unsigned char speed_settings[20];
	unsigned char difficulty_settings[20];
	if (game_speed == 0) sprintf(speed_settings, "Speed: NORMAL");
	else if (game_speed == 1) sprintf(speed_settings, "Speed: FAST");
	else if (game_speed == 2) sprintf(speed_settings, "Speed: SLOW");
	
	if (game_diff == 0) sprintf(difficulty_settings, "Diff: NORMAL");
	else if (game_diff == 1) sprintf(difficulty_settings, "Diff: EASY");
	else if (game_diff == 2) sprintf(difficulty_settings, "Diff: DIFFICULT");
	else if (game_diff == 3) sprintf(difficulty_settings, "Diff: PEACEFUL");
	
	draw_text(buffer, 340, 280, 1, 1, 255, 255, 255, font8x16, speed_settings);
	draw_text(buffer, 340, 300, 1, 1, 255, 255, 255, font8x16, difficulty_settings);
}
