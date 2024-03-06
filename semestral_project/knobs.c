#include <stdlib.h>
#include <stdio.h>
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "font_types.h"
#include "knobs.h"

#define RED 1
#define GREEN 2
#define BLUE 3

unsigned char* init_mem_base_knobs(){
	unsigned char* mem_base = map_phys_address(SPILED_REG_BASE_PHYS,
	SPILED_REG_SIZE, 0);

	if (mem_base == NULL) exit(1);

	return mem_base;
}


uint8_t rgb_knob_value(unsigned char* mem_base, int type){
	uint32_t rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
	uint8_t rk;
	if (type == RED) rk = (rgb_knobs_value>>16) & 0xFF;
	if (type == GREEN) rk = (rgb_knobs_value>>8) & 0xFF;
	if (type == BLUE) rk = (rgb_knobs_value) & 0xFF;

	return rk;
}

int rgb_knob_status(unsigned char* mem_base, int type){
	uint32_t rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
	int rb;
	if (type == RED) rb = (rgb_knobs_value>>26) & 1;
	if (type == GREEN) rb = (rgb_knobs_value>>25) & 1;
	if (type == BLUE) rb = (rgb_knobs_value>>24) & 1;

	return rb;
}
