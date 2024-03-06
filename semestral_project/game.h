#ifndef GAME_H
#define GAME_H

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

#define MAXINVADERS 10
#define MAX_INVADER_BULLETS 4

typedef struct {
	int x;
	int y;
	unsigned char status;
} invader;

typedef struct {
	int x;
	int y;
	unsigned char status;
} bullet;

typedef struct
{
	unsigned int level;
	unsigned int  score;
	unsigned int  level_score;
	unsigned int  lives;
	unsigned int kills;
	int x;
	int y;
} player;

void game(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base, unsigned char *led_mem_base, unsigned char *knobs_mem_base, int game_diff, int game_speed);

void init_difficulty(int game_diff);

void init_speed(int game_speed);

void init_player(player *p);

void init_bullet(bullet *b);

void init_invaders(invader invaders[MAXINVADERS], int max_inv_bullets, bullet invader_bullets[max_inv_bullets]);

void update_game(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], player *p, bullet *b, invader invaders[MAXINVADERS], bullet invader_bullets[MAX_INVADER_BULLETS], int *timer);

void move_player(player *p, uint8_t before, uint8_t after);

void move_bullet(bullet *b);

void bullet_hit(bullet *b, player *p, invader invaders[MAXINVADERS]);

void invaders_bullets_hit(bullet invader_bullets[MAX_INVADER_BULLETS], player *p);

void move_invaders(invader invaders[MAXINVADERS], int *timer, int first_half);

void invaders_shoot(invader invaders[MAXINVADERS], int max_inv_bullets, bullet invader_bullets[max_inv_bullets], int *timer);

void move_invaders_bullets(bullet invader_bullets[MAX_INVADER_BULLETS]);

void update_scoreboard(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], player *p);

void end_screen(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base, unsigned char *led_mem_base, unsigned char *knobs_mem_base, player *p);
#endif
