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
#include "game.h"

// Graphics
#define LCD_WIDTH 480
#define LCD_HEIGHT 320
#define GAMESIZE_WIDTH 5
#define RED 1
#define GREEN 2
#define BLUE 3

// For all
//#define NUMGUNS 3

// For player
#define BULLET_SPEED 10
#define JUMP 20
#define HELP 7

// For invaders_t
#define MAX_INVADER_BULLETS 2
#define INVADER_BULLET_SPEED 5
#define MAXINVADERS 10
#define INVADER_MOVE 30
#define MOVE_LEFT 0
#define MOVE_RIGHT 1

// Game states

#define GAMEINIT 0
#define GAMESTART 1
#define GAMERUN 2
#define GAMEOVER 3

// Invader states
#define DEAD 0
#define ALIVE 1
#define EXPLODED 2

const unsigned char invaders_map[2][6] =
{
    {0x5C,0xB6,0x3F,0xB6,0x5C},
    {0xDC,0x36,0x3F,0x36,0xDC}
};

const unsigned char invader_explosion[] = {0x8A,0x6C,0x13,0xC4,0x34,0x4B,0x88};//7 - Invader explosion

const unsigned char player_map[] = {0xC0,0xC0,0xE0,0xF0,0xE0,0xC0,0xC0};//7

const unsigned char player_explosion1[] = {0x80,0xD5,0xC0,0xA4,0xC0,0x4A,0xA0};//7
const unsigned char player_explosion2[] = {0x2A,0x80,0x84,0x91,0xC0,0x10,0x25};//7

unsigned char invader_bullet[2][3] =
{
    {0x40,0xF8,0x40},
    {0x10,0xA8,0x40}
};

unsigned char bullet_map[] = {0x00,0x1F,0x00};

const unsigned char heart[] = {0x0C,0x1E,0x3E,0x7C,0x7C,0x3E,0x1E,0x0C};


int animation_switch = 0;

// Difficulty
int max_inv_bullets = MAX_INVADER_BULLETS; // 2
int inv_bullet_speed = INVADER_BULLET_SPEED; // 7
int hitbox_help = HELP; // 5
int inv_side_move = INVADER_MOVE; // 30
int p_lives = 3;
int shooting_rate = 12;
// Speed
int move_rate = 9;
int down_rate = 3;
int down_speed = 3;
int timer;


void game(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base, unsigned char *led_mem_base, unsigned char *knobs_mem_base, int game_diff, int game_speed){
	ledstrip_value(led_mem_base, (0x00000000));
	font_descriptor_t* font14x16 = &font_winFreeSystem14x16;
	font_descriptor_t* font8x16 = &font_rom8x16;
	struct timespec delay_amount = {
		.tv_nsec = 500000000,
		.tv_sec = 0
	};
	init_difficulty(game_diff);
	init_speed(game_speed);
	unsigned char open_message[] = "HOLD SHOOT TO START";
	invader invaders[MAXINVADERS];
	bullet my_bullet;
	bullet invader_bullets[max_inv_bullets];
	player p;
	int invaders_dead;
	int temp_score;

    int knob_state;
    int release_button = 0;

    knob blue_knob = {
        .value = rgb_knob_value(knobs_mem_base, BLUE),
        .state = rgb_knob_status(knobs_mem_base, BLUE)
    };
    uint8_t knob_input;
    uint8_t knob_diff;
    unsigned char game_state = GAMESTART;
    timer = 0;

    fill_display(buffer,0, 0, 0);
    update_display(buffer, parlcd_reg_base);

	while(1) {
        timer += 1;
        if (timer % move_rate == 0) animation_switch = !(animation_switch);
        if (timer > 100) timer = 0;
        knob_state = rgb_knob_status(knobs_mem_base, RED);
        
		if (knob_state == 0) release_button = 0;
		
        if (knob_state == 1 && release_button == 0) {
			release_button = 1;
            game_state = GAMESTART;
            fill_display(buffer,0, 0, 0);
            update_display(buffer, parlcd_reg_base);
        }
        switch (game_state) {
            case GAMESTART:
                init_player(&p);
                init_invaders(invaders,max_inv_bullets, invader_bullets);
                init_bullet(&my_bullet);
                update_game(buffer, &p, &my_bullet, invaders, invader_bullets, &timer);
                update_scoreboard(buffer, &p);
                while(1){
					draw_text(buffer, 80, 140, 2, 2, 255, 255, 255, font8x16, open_message);
					update_display(buffer, parlcd_reg_base);
					clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_amount, NULL);
					draw_text(buffer, 80, 140, 2, 2, 0, 0, 0, font8x16, open_message);
					update_display(buffer, parlcd_reg_base);
					clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_amount, NULL);
					knob_state = rgb_knob_status(knobs_mem_base, BLUE);
					if (knob_state == 1) {
						release_button = 1;
						break;
					}
				}
                game_state = GAMERUN;
                break;
            case GAMERUN:
				temp_score = p.score % 80;
				
				if (p.level_score >= 10) {
					ledstrip_value(led_mem_base, (0xf0000000));
					led2_value(led_mem_base, 0x00000000);
				}
				if (p.level_score >= 20) ledstrip_value(led_mem_base, (0xff000000));
				if (p.level_score >= 30) ledstrip_value(led_mem_base, (0xfff00000));
				if (p.level_score >= 40) ledstrip_value(led_mem_base, (0xffff0000));
				if (p.level_score >= 50) ledstrip_value(led_mem_base, (0xfffff000));
				if (p.level_score >= 60) ledstrip_value(led_mem_base, (0xffffff00));
				if (p.level_score >= 70) ledstrip_value(led_mem_base, (0xfffffff0));
				if (p.level_score >= 80) {
					ledstrip_value(led_mem_base, (0xffffffff));
					p.level += 1;
					p.level_score = p.level_score - 80;
					
					if (p.level % 2) {
						if (hitbox_help > 0) hitbox_help -= 1;
					} else if (p.level % 3) {
						if (move_rate > 3) move_rate -= 1;
						hitbox_help -= 1; 
					} else if (p.level % 5) {
						down_speed += 1;
					}
				}
				
				invaders_dead = 0;
                for (int i = 0; i < MAXINVADERS; i++){
					if (invaders[i].status == DEAD || invaders[i].status == EXPLODED) invaders_dead += 1;
				}
				if (invaders_dead == MAXINVADERS) {
					init_invaders(invaders, max_inv_bullets, invader_bullets);
					p.score += 50;
					p.level_score += 50;
					if (p.lives < 5) p.lives += 1;
					led2_value(led_mem_base, 0x000000ff);
				}
				
                knob_input = rgb_knob_value(knobs_mem_base, BLUE);
                knob_state = rgb_knob_status(knobs_mem_base, BLUE);

                if (knob_state == 0) release_button = 0;

                if (knob_state == 1 && release_button == 0) {
                    if (my_bullet.status == 0 && release_button == 0){
                        release_button = 1;
                        my_bullet.status = 1;
                        my_bullet.x = p.x + (2*8);
                        my_bullet.y = p.y - 8;
                    }
                } else if (knob_input != blue_knob.value){
                    move_player(&p, blue_knob.value, knob_input);
                    blue_knob.value = knob_input;
                }
                if (my_bullet.status == 1){
                    move_bullet(&my_bullet);
                    bullet_hit(&my_bullet, &p, invaders);
                }
                move_invaders(invaders, &timer, 0);
                move_invaders(invaders, &timer, 1);
                invaders_shoot(invaders, max_inv_bullets, invader_bullets, &timer);
                move_invaders_bullets(invader_bullets);
                invaders_bullets_hit(invader_bullets, &p);

                fill_display(buffer,0, 0, 0);
                update_game(buffer, &p, &my_bullet, invaders, invader_bullets, &timer);
                invaders_shoot(invaders,max_inv_bullets, invader_bullets, &timer);

                if (p.lives == 0) {
                    game_state = GAMEOVER;
                    break;
                }
                update_scoreboard(buffer, &p);
                update_display(buffer, parlcd_reg_base);
                break;
                
            case GAMEOVER:
                fill_display(buffer,100, 0, 0);
                update_scoreboard(buffer, &p);
                objects2buffer(buffer, p.x, p.y, 7, player_map);
                update_display(buffer, parlcd_reg_base);
                clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_amount, NULL);
                fill_display(buffer,150, 0, 0);
                update_scoreboard(buffer, &p);
                objects2buffer(buffer, p.x, p.y, 7, player_explosion1);
                update_display(buffer, parlcd_reg_base);
                clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_amount, NULL);
                fill_display(buffer,200, 0, 0);
                update_scoreboard(buffer, &p);
                objects2buffer(buffer, p.x, p.y, 7, player_explosion2);
                update_display(buffer, parlcd_reg_base);
                clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_amount, NULL);
                clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_amount, NULL);
				end_screen(buffer, parlcd_reg_base, led_mem_base, knobs_mem_base, &p);
        }
	}
}

void init_difficulty(int game_diff){
	if (game_diff == 0){ // NORMAL
		max_inv_bullets = 2;
		inv_bullet_speed = 5;
		inv_side_move = 30;
		hitbox_help = 7;
		p_lives = 3;
	} else if (game_diff == 1){ // EASY
		max_inv_bullets = 1;
		inv_bullet_speed = 3;
		inv_side_move = 10;
		hitbox_help = 10;
		p_lives = 5;
	} else if (game_diff == 2){ // DIFFICULT
		max_inv_bullets = 4;
		inv_bullet_speed = 5;
		inv_side_move = 40;
		hitbox_help = 5;
		p_lives = 2;
	} else { // PEACEFUL
		max_inv_bullets = 8;
		inv_bullet_speed = 5;
		inv_side_move = 45;
		hitbox_help = 100;
		p_lives = 10000;
	}
}

void init_speed(int game_speed){
	if (game_speed == 0){ // NORMAL
		move_rate = 9;
		down_rate = 12;
		down_speed = 10;
	} else if (game_speed == 1){ // FAST
		move_rate = 6;
		down_rate = 6;
		down_speed = 4;
	} else { // SLOW
		move_rate = 9;
		down_rate = 6;
		down_speed = 1;
	}
}
	
	
void init_player(player *p){
    p->level = 0;
	p->score = 0;
	p->level_score = 0;
	p->kills = 0;
    p->lives = p_lives;
    p->x = 212;
    p->y = 210;
}

    void init_bullet(bullet *b){
        b->status = 0;
    }

void init_invaders(invader invaders[MAXINVADERS], int max_inv_bullets, bullet invader_bullets[max_inv_bullets]){
    // First row
    for (int i = 0; i < MAXINVADERS; i++){
        invaders[i].x = 31 + (84*i);
        invaders[i].y = 0;
        invaders[i].status = ALIVE;
    }
    for (int i = 0; i < MAXINVADERS/2; i++){
        invaders[i+MAXINVADERS/2].x = 73 + (84*i);
        invaders[i+MAXINVADERS/2].y = 160;
        invaders[i+MAXINVADERS/2].status = DEAD;
    // Second row
    }
    for (int i = 0; i < max_inv_bullets; i++){
		invader_bullets[i].status = 0;
	}
}

void update_game(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], player *p, bullet *b, invader invaders[MAXINVADERS], bullet invader_bullets[max_inv_bullets], int *timer){
    objects2buffer(buffer, p->x, p->y, 7, player_map);
    if (b->status == 1){
        objects2buffer(buffer, b->x, b->y, 3, bullet_map);
    } 
	
	for (int i = 0; i < max_inv_bullets; i++){
		if (invader_bullets[i].status == 1){
			 objects2buffer(buffer, invader_bullets[i].x, invader_bullets[i].y, 3, invader_bullet[animation_switch]);
		}
	}
	
    for (int i = 0; i < (MAXINVADERS); i++){
        if ((invaders[i].y % 320) == 0){
            invaders[i].y = 0;
            invaders[i].status = ALIVE;
        } else if ((invaders[i].y > 215) && (invaders[i].status == ALIVE)) {
            invaders[i].status = DEAD;
            if (p->lives > 0) p->lives -= 1;
        }
    }

    for (int i = 0; i < (MAXINVADERS); i++){
        if (invaders[i].status == ALIVE){
            objects2buffer(buffer, invaders[i].x, invaders[i].y, 5, invaders_map[animation_switch]);
        } else if (invaders[i].status == EXPLODED){
			objects2buffer(buffer, invaders[i].x, invaders[i].y, 5, invader_explosion);
			invaders[i].status = DEAD;
		}
    }
}

void move_player(player *p, uint8_t before, uint8_t after){
    if (before < after){
        if (before < 20 && after > 20){
            p->x -= JUMP;
        }
        if (((p->x + JUMP) < (424 - GAMESIZE_WIDTH)) && ((p->x + JUMP) > (0 + GAMESIZE_WIDTH))){
            p->x += JUMP;
        }
    } else if (before > after) {
        if (after < 20 && before > 20){
            p->x += JUMP;
        }
        if (((p->x - JUMP) < (424 - GAMESIZE_WIDTH)) && ((p->x - JUMP) > (0 + GAMESIZE_WIDTH))){
            p->x -= JUMP;
        }
    }
}

void move_bullet(bullet *b){
    if ((b->y - BULLET_SPEED) < 0){
        b->status = 0;
    } else {
    b->y -= BULLET_SPEED;
    }
}

void bullet_hit(bullet *b, player *p, invader invaders[MAXINVADERS]){
    for (int i = 0; i < (MAXINVADERS); i++){
        if ((invaders[i].status == DEAD) || (invaders[i].status == EXPLODED)) continue;
        if ((b->x+23 >= invaders[i].x) && (b->x <= (invaders[i].x + 40))
            && (b->y >= invaders[i].y) && (b->y <= (invaders[i].y + 64))){
            invaders[i].status = EXPLODED;
            b->status = 0;
            p->score += 10;
            p->level_score += 10;
        }
    }
}

void invaders_bullets_hit(bullet invader_bullets[max_inv_bullets], player *p){
	for (int i = 0; i < (max_inv_bullets); i++){
		if (invader_bullets[i].status == 0) continue;
        if (((invader_bullets[i].x+23 - hitbox_help)  >= p->x) && (invader_bullets[i].x <= (p->x + 56 - hitbox_help))
            && (invader_bullets[i].y >= (p->y - 20)) && (invader_bullets[i].y <= (p->y))){
			if (p->lives > 0) p->lives -= 1;
            invader_bullets[i].status = 0;
        } else if (invader_bullets[i].y > 215){
			invader_bullets[i].status = 0;
		}
    }
}

void move_invaders(invader invaders[MAXINVADERS], int *timer, int first_half){
    int rnd = rand();
    int choice = MOVE_LEFT;
    int max_distance;
    int move_by = 0;
    int move_down = 0;

    if (first_half == 1){
        if (*timer % move_rate == 0){
            if (rnd%2 == 0) choice = MOVE_RIGHT;

            if (choice == MOVE_LEFT){
                max_distance = invaders[0].x;
                if ((max_distance - GAMESIZE_WIDTH) < inv_side_move) {
                    move_by = max_distance;
                } else {
                    move_by = inv_side_move;
                }
            } else {
                max_distance = ((439 - GAMESIZE_WIDTH) - invaders[MAXINVADERS/2 - 1].x);
                if (max_distance < inv_side_move) {
                    move_by = max_distance;
                } else {
                    move_by = inv_side_move;
                }
            }
        }

        if (*timer % down_rate == 0){
            move_down = down_speed;
        }

        for (int i = 0; i < (MAXINVADERS/2); i++){
            if (choice == MOVE_LEFT){
                invaders[i].x -= move_by;
            } else {
                invaders[i].x += move_by;
            }
            invaders[i].y += move_down;
        }
    } else {
        if (*timer % move_rate == 0){
            if (rnd%2 == 0) choice = MOVE_RIGHT;

            if (choice == MOVE_LEFT){
                max_distance = invaders[MAXINVADERS/2].x;
                if ((max_distance - GAMESIZE_WIDTH) < inv_side_move) {
                    move_by = max_distance;
                } else {
                    move_by = inv_side_move;
                }
            } else {
                max_distance = ((439 - GAMESIZE_WIDTH) - invaders[MAXINVADERS - 1].x);
                if (max_distance < inv_side_move) {
                    move_by = max_distance;
                } else {
                    move_by = inv_side_move;
                }
            }
        }
        if (*timer % down_rate == 0){
            move_down = down_speed;
        }

        for (int i = MAXINVADERS/2; i < (MAXINVADERS); i++){
            if (choice == MOVE_LEFT){
                invaders[i].x -= move_by;
            } else {
                invaders[i].x += move_by;
            }
            invaders[i].y += move_down;
        }
    }


}

void invaders_shoot(invader invaders[MAXINVADERS], int max_inv_bullets, bullet invader_bullets[max_inv_bullets], int *timer){
	if (*timer % 6 == 0) return;
	for (int i = 0; i < max_inv_bullets; i++){
		if (invader_bullets[i].status == 0){
			int rnd = rand();
			if (rnd%2 == 0) continue;
				int rnd_choice = rand();
				rnd_choice = rnd_choice % MAXINVADERS;
				if (invaders[rnd_choice].status == ALIVE){
					invader_bullets[i].status = 1;
					invader_bullets[i].x = invaders[rnd_choice].x + 8;
					invader_bullets[i].y = invaders[rnd_choice].y + 30;
				}

		}
		
	}
}
void move_invaders_bullets(bullet invader_bullets[max_inv_bullets]){
    for (int i = 0; i < max_inv_bullets; i++){
		if (invader_bullets[i].status == 1){
			invader_bullets[i].y += inv_bullet_speed;
		}
	}
}

void update_scoreboard(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], player *p){
    char score[20];
    char lives[] = "Lives:";
    char level[20];
    sprintf(score, "Score: %d", p->score);
    sprintf(level, "Level: %d", p->level);
    draw_scorebox(buffer);
    draw_text(buffer, 55, 294, 1, 1, 255, 255, 255, &font_winFreeSystem14x16, score);
    draw_text(buffer, 180, 294, 1, 1, 255, 255, 255, &font_winFreeSystem14x16, lives);
    draw_text(buffer, 350, 294, 1, 1, 255, 255, 255, &font_winFreeSystem14x16, level);
    if (p->lives <= 5){
		for (int i = 0; i < p->lives; i++){
        draw_heart(buffer, 240 + (i * 21), 293, heart);
		}
	} else {
		for (int i = 0; i < 5; i++){
        draw_heart(buffer, 240 + (i * 21), 293, heart);
		}
	}
    

}

void end_screen(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base, unsigned char *led_mem_base, unsigned char *knobs_mem_base, player *p){
	font_descriptor_t* font14x16 = &font_winFreeSystem14x16;
	font_descriptor_t* font8x16 = &font_rom8x16;
    struct timespec delay_amount = {
		.tv_nsec = 500000000,
		.tv_sec = 0
	};
	int knob_state = 0;
	unsigned char title1[] = "SPACE";
	unsigned char title2[] = "REMAINDERS";

	unsigned char result1[20];
	unsigned char result2[20];
	unsigned char result3[20];
	unsigned char result4[30] = "Press RED JS to go back...";

	sprintf(result1, "Total score: %d", p->score);
	sprintf(result2, "Invader kills: %d", p->kills);
	sprintf(result3, "Achieved level: %d", p->level);
	while(1){
		fill_display(buffer, 0, 0, 0);
		draw_line(buffer,125, 100, 10, 160, 220, 255);
		draw_text(buffer, 173, 10, 3, 3, 255, 255, 255, font14x16, title1);
		draw_text(buffer, 100, 50, 3, 3, 255, 255, 255, font14x16, title2);
		draw_text(buffer, 20, 130, 2, 2, 255, 255, 255, font8x16, result1);
		draw_text(buffer, 20, 170, 2, 2, 255, 255, 255, font8x16, result2);
		draw_text(buffer, 20, 210, 2, 2, 255, 255, 255, font8x16, result3);
		update_display(buffer, parlcd_reg_base);
		led1_value(led_mem_base, 0x00000000);
		clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_amount, NULL);
		draw_text(buffer, 20, 260, 1, 1, 255, 255, 255, font8x16, result4);
        update_display(buffer, parlcd_reg_base);
        led1_value(led_mem_base, 0x00ff0000);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &delay_amount, NULL);
        knob_state = rgb_knob_status(knobs_mem_base, RED);
		if (knob_state == 1) {
			menu(buffer, parlcd_reg_base, led_mem_base, knobs_mem_base);
		}
        
	}

}
