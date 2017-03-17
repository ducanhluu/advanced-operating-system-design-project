#include <cpu.h>
#include <stdbool.h>

#include "console.h"
#include "time.h"
#include "segment.h"
#include "debug.h"
#include "../kernel/process.h"

void display_corner(char* time) {
	char* cour = time;
	while (*cour != '\0') {
		cour++;
	}
	int col = 79;
	while (cour != time) {
		cour --;
		ecrit_car(0, col, *cour);
		col --;
	}
}


void tic_PIT() {
	cli();
	printf("\n---TICK---\n\n");
	outb(0x20, 0x20);
	/* Do stuff */
	static int ticks = 0;

	ticks++;
	if (ticks >= 100) {
		time ++;
		ticks=0;
	}

	int v,sec,hr,min;
	hr=time/3600;
	v=time%3600;
	min=v/60;
	sec=v%60;

	char timestring[8];
	sprintf(timestring,"%02d:%02d:%02d", hr, min, sec);
	sti();	
	ordonnance();	
	display_corner(timestring);
}

void init_traitant_IT(int32_t num_IT, void (*traitant)(void)) {
	uint16_t traitant_low = (uint16_t)((uint32_t)traitant) ;
	uint16_t traitant_high = (uint16_t)((uint32_t)traitant >> 16);

	uint32_t first = (KERNEL_CS << 16) | (traitant_low);
	uint32_t second = (traitant_high << 16) | (0x8E00);

	uint32_t* addr1 = (uint32_t*)(IDT_ADDR + num_IT*2*sizeof(uint32_t));
	*addr1 = first;
	uint32_t* addr2 = (uint32_t*)(IDT_ADDR + (num_IT*2 + 1)*sizeof(uint32_t));
	*addr2 = second;
}

void set_clock_fqc() {
	outb(0x34, 0x43);
	outb((QUARTZ / CLOCKFREQ) % 256, 0x40);
	outb((QUARTZ / CLOCKFREQ) >> 8, 0x40);
}

void masque_IRQ(uint32_t num_IRQ, bool masque) {
	/* 1 = masked ; 0 = unmasked*/
	uint8_t mask_array = inb(0x21);

	uint8_t low_mask, high_mask;

	switch (num_IRQ) {
	case 0 :
		low_mask=0;
		high_mask=254;
		break;
	case 1 :
		low_mask=1;
		high_mask=252;
		break;
	case 2 :
		low_mask=3;
		high_mask=248;
		break;
	case 3 :
		low_mask=7;
		high_mask=240;
		break;
	case 4 :
		low_mask=15;
		high_mask=224;
		break;
	case 5 :
		low_mask=31;
		high_mask=192;
		break;
	case 6 :
		low_mask=63;
		high_mask=128;
		break;
	case 7 :
		low_mask=127;
		high_mask=0;
		break;
	}

	uint8_t high_bits = mask_array & high_mask;
	uint8_t low_bits = mask_array & low_mask;

	mask_array = high_bits | masque;
	mask_array = mask_array | low_bits;
	outb(mask_array, 0x21);
}
