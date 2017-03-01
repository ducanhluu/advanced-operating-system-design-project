#include "debugger.h"
#include "cpu.h"
#include "console.h"
#include "stdio.h"
#include "time.h"
#include "process.h"
#include "sem.h"

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

	printf("group_14@awsome_pc:~$\n");

        int sem1;
       
        sem1 = screate(2);
        assert(sem1 >= 0);
        assert(scount(sem1) == 2);
        assert(signal(sem1) == 0);
        assert(scount(sem1) == 3);
        assert(signaln(sem1, 2) == 0);
        assert(scount(sem1) == 5);
        assert(wait(sem1) == 0);
        assert(scount(sem1) == 4);
        assert(sreset(sem1, 7) == 0);
        assert(scount(sem1) == 7);
        assert(sdelete(sem1) == 0);
        printf("ok.\n");

	// Unmask external IT
	/* sti(); */
	// on ne doit jamais sortir de kernel_start
	while (1) {
		// cette fonction arrete le processeur
		hlt();
	}


	return;
}
