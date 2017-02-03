#include "debugger.h"
#include "cpu.h"
#include "console.h"
#include "printf.c"
#include "../shared/time.c"

int fact(int n)
{
	if (n < 2)
		return 1;

	return n * fact(n-1);
}


void kernel_start(void)
{
	int i;
//	call_debugger();

	i = 10;

	i = fact(i);

	cli();
	set_clock_fqc();
	init_traitant_IT(32, &traitant_IT_32);
	masque_IRQ(0,0);

	printf("\f");
	loading_bar(2);
	printf("\f");

	printf("guigre@Guigre-PC:~$ idle\n");

	init_process_stack();

	idle();

	// Unmask external IT
	/* sti(); */
	// on ne doit jamais sortir de kernel_start
	while (1) {
		// cette fonction arrete le processeur
		hlt();
	}


	return;
}
