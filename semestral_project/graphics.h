#ifndef GRAPHICS_H
#define GRAPHICS_H
#define LCD_WIDTH 480
#define LCD_HEIGHT 320
#include <stdint.h>

union pixel {
    struct {
        unsigned b : 5;
        unsigned g : 6;
        unsigned r : 5;
    };
    uint16_t data;
};

typedef struct {
	int w;
	int h;
	union pixel pixels[20][30];
} ship;

void update_display(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], unsigned char *parlcd_reg_base);

void change_pixel(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, uint8_t r, uint8_t g, uint8_t b);

void fill_display(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], uint8_t r, uint8_t g, uint8_t b);

void fill_display_space(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], uint8_t r, uint8_t g, uint8_t b);

void draw_line(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, int thickness, uint8_t r, uint8_t g, uint8_t b);

void object_into_buffer(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], ship object, int x, int y);

int char_width(font_descriptor_t* f, char c);

void draw_pixel(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, uint8_t r, uint8_t g, uint8_t b);

void display_char_pixel(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, uint8_t r, uint8_t g, uint8_t b);

void display_char(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, font_descriptor_t* f, int scale,  char c, uint8_t r, uint8_t g, uint8_t b);

void draw_text(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, int x_size, int y_size, uint8_t r, uint8_t g, uint8_t b, font_descriptor_t* f, char* str);

void objects2buffer(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, int width, unsigned char object[width]);

void draw_object_pixel(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y);

void objects2buffer_small(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, int width, unsigned char object[width]);

void draw_object_pixel_small(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y);

void draw_heart(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y, unsigned char object[8]);

void draw_heart_pixel(union pixel buffer[LCD_WIDTH][LCD_HEIGHT], int x, int y);

void draw_scorebox(union pixel buffer[LCD_WIDTH][LCD_HEIGHT]);
#endif
