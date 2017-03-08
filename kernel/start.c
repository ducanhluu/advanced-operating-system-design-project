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
/* ------------------------------------------------ */
/* ---- TEST 10 - SEMAPHORE ----------------------- */
/* ------------------------------------------------ */
        int sem;
        assert(screate(-2) == -1);
        assert((sem = screate(2)) >= 0);
        assert(signaln(sem, -4) < 0);
        assert(sreset(sem, -3) == -1);
        assert(scount(sem) == 2);
        assert(signaln(sem, 32760) == 0);
        assert(signaln(sem, 6) == -2);
        assert(scount(sem) == 32762);
        assert(wait(sem) == 0);
        assert(scount(sem) == 32761);
        assert(signaln(sem, 30000) == -2);
        assert(scount(sem) == 32761);
        assert(wait(sem) == 0);
        assert(scount(sem) == 32760);
        assert(signaln(sem, -2) < 0);
        assert(scount(sem) == 32760);
        assert(wait(sem) == 0);
        assert(scount(sem) == 32759);
        assert(signaln(sem, 8) == 0);
        assert(scount(sem) == 32767);
        assert(signaln(sem, 1) == -2);
        assert(scount(sem) == 32767);
        assert(signal(sem) == -2);
        assert(scount(sem) == 32767);
        for (int i=0; i<32767; i++) {
            assert(wait(sem) == 0);
        }
        assert(try_wait(sem) == -3);
        assert(scount(sem) == 0);
        assert(sdelete(sem) == 0);
        printf("ok.\n");
/* -------------------------------------------------- */
/* ---- END OF TEST 10 ------------------------------ */
/* -------------------------------------------------- */

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
