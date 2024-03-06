#ifndef GAME_STATE_H
#define GAME_STATE_H
#define LCD_WIDTH 480
#define LCD_HEIGHT 320
#include <stdint.h>
#include <graphics.h>
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "font_types.h"
#include "graphics.h"
#include "game_state.h"
#include "led_control.h"
#include "knobs.h"
#include "game.h"

void menu(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base, unsigned char *led_mem_base, unsigned char *knobs_mem_base);

void settings(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base, unsigned char *led_mem_base, unsigned char *knobs_mem_base);

void how_to_play(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base, unsigned char *led_mem_base, unsigned char *knobs_mem_base);

#endif
