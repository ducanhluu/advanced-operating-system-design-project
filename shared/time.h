#ifndef __TIME_H__
#define __TIME_H__

#include "inttypes.h"
#include <stdbool.h>

#define IDT_ADDR 0x1000
#define CLOCKFREQ 100
#define QUARTZ 0x1234DD

int time;

extern void traitant_IT_32();

void tic_PIT();

void init_traitant_IT(int32_t num_IT, void (*traitant)(void));

void set_clock_fqc();

void masque_IRQ(uint32_t num_IRQ, bool masque);

#endif
