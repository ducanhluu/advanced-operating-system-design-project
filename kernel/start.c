#include "debugger.h"
#include "cpu.h"
#include "console.h"

#include "stdio.h"
#include "stdlib.h"

#include "time.h"
#include "process.h"
#include "sem.h"
#include "tests.h"

int fact(int n)
{
	if (n < 2)
		return 1;

	return n * fact(n-1);
}

void kernel_start(void) {
        cli();
	set_clock_fqc();
	init_traitant_IT(32, &traitant_IT_32);
	masque_IRQ(0,0);

	printf("\f");
	
	printf("group_14@awsome_pc:~$\n");
        init_process_stack();

	start(test2, "test2", 1024, 128, NULL);
	
        idle(NULL);

	// Unmask external IT
	sti();
	// on ne doit jamais sortir de kernel_start

	while (1) {
		// cette fonction arrete le processeur
		hlt();
	}
}
