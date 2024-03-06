#ifndef KNOBS_H
#define KNOBS_H

typedef struct {
    uint8_t value;
    uint8_t state;
} knob;

unsigned char *init_mem_base_knobs();

uint8_t rgb_knob_value(unsigned char* mem_base, int type);

int rgb_knob_status(unsigned char* mem_base, int type);

#endif
