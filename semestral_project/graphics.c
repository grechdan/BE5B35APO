#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "font_types.h"
#include "graphics.h"

#define LCD_WIDTH 480
#define LCD_HEIGHT 320

#define GAMESIZE_WIDTH 5

void update_display(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base){
	for (int y = 0; y < LCD_HEIGHT; y++) {
		for (int x = 0; x < LCD_WIDTH; x++) {
			parlcd_write_data(parlcd_reg_base, buffer[x][y].data);
		}
	}
	return;
}

void fill_display(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], uint8_t r, uint8_t g, uint8_t b){
	for (int y = 0; y < LCD_HEIGHT; y++) {
		for (int x = 0; x < LCD_WIDTH; x++) {
			draw_pixel(buffer, x, y, r, g, b);
		}
	}
	return;
}

void fill_display_space(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], uint8_t r, uint8_t g, uint8_t b){
	for (int y = 0; y < LCD_HEIGHT; y++) {
		for (int x = 0; x < LCD_WIDTH; x++) {
			int rnd = rand();
			if (rnd % 17) {
				draw_pixel(buffer, x, y, 255, 255, 255);
			} else {
			draw_pixel(buffer, x, y, r, g, b);
			}
		}
	}
	return;
}

void draw_line(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, int thickness, uint8_t r, uint8_t g, uint8_t b){
	for (int i = y; i < y + thickness; i++) {
		for (int j = x; j < (LCD_WIDTH - x); j++) {
			draw_pixel(buffer, j, i, r, g, b);
			}
		}
}

void object_into_buffer(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], ship object, int x, int y){
	if ((x < 0) || ((x + object.w) > LCD_WIDTH) || (y < 0) || ((y + object.h) > LCD_HEIGHT)) return;
	for (int i = 0; i < object.w; i++) {
		for (int j = 0; j < object.h; j++) {
			if (object.pixels[i][j].r == 0 & object.pixels[i][j].g == 0 & object.pixels[i][j].b == 0) continue;
			draw_pixel(buffer, x+i, y+j,object.pixels[i][j].r,object.pixels[i][j].g,object.pixels[i][j].b);
		}
	}
	return;
}

void draw_pixel(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, uint8_t r, uint8_t g, uint8_t b) {
	buffer[x][y].r = r;
	buffer[x][y].g = g;
	buffer[x][y].b = b;
}

int char_width(font_descriptor_t* f, char c) {
   int w = 0;

   if ((c >= f->firstchar) && ((c - f->firstchar) < f->size)) {
      c -= f->firstchar;
      if (f->width)
         w = f->width[c];
      else
         w = f->maxwidth;
   }
   return w;
}


void display_char(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, font_descriptor_t* f, int scale,  char c, uint8_t r, uint8_t g, uint8_t b) {
	// Calculating w
	int w = 16;
	int h = f->height;
	uint32_t offset = 0;
	// Checking presence
	if (((c - f->firstchar) < f->size) && (c >= f->firstchar)) {
      offset = f->height * (c - f->firstchar);
    }

	for (int i = 0; i < h; i++) {
		uint16_t bits = *(f->bits + offset + i);
		for (int j = 0; j < w; j++) {
         uint16_t mask = 1 << (15 - j);
			if (bits & mask) {
				int block_x = x + j * scale;
				int block_y = y + i * scale;

				for (int i = 0; i < scale; ++i) {
					if ((block_x + i >= 0) && (block_x + i < LCD_WIDTH)) {
						for (int j = 0; j < scale; ++j) {
							if ((block_y + j >= 0) && (block_y + j < LCD_HEIGHT)) {
								// place on the screen
								draw_pixel(buffer, block_x + i, block_y + j, r, g, b);
							}
						}
					}
				}
			}
		}
	}
	return;
}

void draw_text(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, int x_size, int y_size, uint8_t r, uint8_t g, uint8_t b, font_descriptor_t* f, char* str) {
    char* c = str;

    for (int i = 0; i < strlen(str); i++) {
        display_char(buffer, x, y, f, y_size, *c, r, g, b);

        int w = 0;
        w = char_width(f, *c);

		x += x_size * w + 1; //offset for next char
        c++;

    }
	return;
}

void objects2buffer(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, int width, unsigned char object[width]){
	unsigned char temp;
	unsigned char bit;
	int offset;

	for (int i = 0; i < width; i++){
		// 0x5C
		temp = object[i];
		for (int j = 0; j < 8; j++){
			offset = j;
			bit = (temp >> offset) & 0x01;
			if (bit == 1) draw_object_pixel(buffer, x + (i * 8), y + (j * 8));
		}
    }
}

void draw_object_pixel(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y){
	for (int i = 0; i < 8; i++){
		for (int j = 0; j < 8; j++){
			draw_pixel(buffer, x+i, y+j, 255, 255, 255);
		}
	}
}

void objects2buffer_small(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, int width, unsigned char object[width]){
	unsigned char temp;
	unsigned char bit;
	int offset;

	for (int i = 0; i < width; i++){
		// 0x5C
		temp = object[i];
		for (int j = 0; j < 8; j++){
			offset = j;
			bit = (temp >> offset) & 0x01;
			if (bit == 1) draw_object_pixel_small(buffer, x + (i * 4), y + (j * 4));
		}
    }
}

void draw_object_pixel_small(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y){
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			draw_pixel(buffer, x+i, y+j, 255, 255, 255);
		}
	}
}

void draw_heart(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, unsigned char object[8]){
	unsigned char temp;
	unsigned char bit;
	int offset;

	for (int i = 0; i < 8; i++){
		// 0x5C
		temp = object[i];
		for (int j = 0; j < 8; j++){
			offset = j;
			bit = (temp >> offset) & 0x01;
			if (bit == 1) draw_heart_pixel(buffer, x + (i * 2), y + (j * 2));
		}
    }
}

void draw_heart_pixel(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y){
	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 2; j++){
			draw_pixel(buffer, x+i, y+j, 255, 0, 0);
		}
	}
}

void draw_scorebox(union pixel buffer[LCD_WIDTH][LCD_HEIGHT]){
	for (int i = 0; i < LCD_WIDTH; i++){
		for (int j = 0; j < LCD_HEIGHT; j++){
			if ((i < GAMESIZE_WIDTH) || (i >= (479 - GAMESIZE_WIDTH)) || (j >= 280 && j <= 285) || (j >= 314)){
			draw_pixel(buffer, i, j, 160, 220, 255);
			}
		}
	}
}
