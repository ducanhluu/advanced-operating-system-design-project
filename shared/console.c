#include "console.h"
#include "cpu.h"
#include "inttypes.h"
#include "cpu.h"
#include "stddef.h"
#include "string.h"

/* GLOBAL VARS */
#define NB_LIG 25
#define NB_COL 80
#define START_VID_ADDR 0xB8000

// Coordonnées du curseur
static uint16_t lig = 0;
static uint16_t col = 0;
// clignottement
static uint8_t cl = 0;
// couleur de fond
static uint8_t bc = 0;
// couleur de texte
static uint8_t tc = 2;

uint16_t* ptr_mem(uint32_t lig, uint32_t col) {
	if (lig >= NB_LIG || col >= NB_COL) {
		// ERROR
		return NULL;
	} else {
		uint16_t* addr = (uint16_t*)(START_VID_ADDR + 2*(lig*NB_COL+col));
		return addr;
	}
}

void ecrit_car(uint32_t lig, uint32_t col, char c) {
	uint16_t* addr = ptr_mem(lig, col);
  
	*addr = c | (cl << 15) | (bc << 12) | (tc << 8);
}

void efface_ecran(void) {
	for (uint32_t l = 0 ; l < NB_LIG ; l++) {
		for (uint32_t c = 0 ; c < NB_COL ; c++) {
			// 32 is the ASCII code for 'space'
			ecrit_car(l, c, 32);
		}
	}
}

void place_curseur(uint32_t lig, uint32_t col) {

	uint16_t pos = col+lig*NB_COL;
	uint8_t low_pos = (uint8_t)(pos & 0xff);
	uint8_t high_pos = (uint8_t)(pos >> 8);

	outb(CURSOR_LOW_CODE, CURSOR_CMD_PORT);
	outb(low_pos, CURSOR_DATA_PORT);

	outb(CURSOR_HIGH_CODE, CURSOR_CMD_PORT);
	outb(high_pos, CURSOR_DATA_PORT);
}

void defilement(void) {
	memmove((void*)ptr_mem(0,0), (void*)ptr_mem(1,0), 24*NB_COL*2);
	for (uint32_t c = 0 ; c < NB_COL ; c++) {
		ecrit_car(24, c, ' ');
	}
}

void traite_car(char c) {
	if (c < 32 || c == 127 ) {
		switch (c) {
		case 8:
			if (col > 0) col--;			
			break;
		case 9:
			if (col <= 72) col += 4;
			break;
		case 10:
			if (lig < 24) {
				col = 0;
				lig ++;
			} else {
				defilement();
				col = 0;
				lig = 24;
			}
			break;
		case 12:
			efface_ecran();
			col = 0;
			lig = 0;
			break;
		case 13:
			col = 0;
			break;
		default:
			
			break;
		}

	} else if (c >= 32 && c <= 126) {
		// Ecrire le caractère
		ecrit_car(lig, col, c);
		// Deplacer le curseur
		col++;
		if (col >= NB_COL) {
			col = 0;
			lig ++;
			if (lig >= NB_LIG) {
				defilement();
				lig = 24;
			}
		}
	} else {
		traite_car(c%128);
	}
	place_curseur(lig,col);
}

void console_putbytes(const char *s, int len) {
	for (uint16_t i = 0 ; i < len ; i++) {
		traite_car(s[i]);
	}	
}
