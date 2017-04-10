#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "inttypes.h"

#define CURSOR_CMD_PORT 0x3D4
#define CURSOR_DATA_PORT 0x3D5

#define CURSOR_LOW_CODE 0x0F
#define CURSOR_HIGH_CODE 0x0E

/*
 * This is the function called by printf to send its output to the screen. You
 * have to implement it in the kernel and in the user program.
 */
extern void console_putbytes(const char *s, int len);

// renvoit un pointeur sur la case mémoire correspondant aux coordonnées fournies
uint16_t* ptr_mem(uint32_t lig, uint32_t col);

// écrit le caractère c aux coordonnées spécifiées
void ecrit_car(uint32_t lig, uint32_t col, char c);

// doit parcourir les lignes et les colonnes de l'écran pour écrire dans chaque case un espace en blanc sur fond noir
void efface_ecran(void);

// place le curseur à la position donnée
void place_curseur(uint32_t lig, uint32_t col);

void traite_car(char c);

#endif
