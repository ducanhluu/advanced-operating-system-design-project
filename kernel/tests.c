#include "stdio.h"
#include "sem.h"
#include "tests.h"
#include "process.h"
int test0(void *arg) {
  (void)arg;
        register unsigned reg1 = 1u;
        register unsigned reg2 = 0xFFFFFFFFu;
        register unsigned reg3 = 0xBADB00B5u;
        register unsigned reg4 = 0xDEADBEEFu;


        printf("I'm a simple process running ...");

        unsigned i;
        for (i = 0; i < 10000000; i++) {
             if (reg1 != 1u || reg2 != 0xFFFFFFFFu || reg3 != 0xBADB00B5u || reg4 != 0xDEADBEEFu) {
                printf(" and I feel bad. Bybye ...\n");
                assert(0);
             }
        }

        printf(" and I'm healthy. Leaving.\n");

        return 0;
}

/*******************************************************************************
 * Test 1
 *
 * Demarrage de processus avec passage de parametre
 * Terminaison normale avec valeur de retour
 * Attente de terminaison (cas fils avant pere et cas pere avant fils)
 ******************************************************************************/
#define DUMMY_VAL 78
int dummy1(void *arg) {
        printf("1");
        assert((int) arg == DUMMY_VAL);
        return 3;
}

int dummy2(void *arg)
{
        printf(" 5");
        assert((int) arg == DUMMY_VAL + 1);
        return 4;
}

int test1(void *arg)
{
        int pid1;
        int r;
        int rval;

        (void)arg;

        pid1 = start(dummy1, "dummy1", 4000, 192, (void *) DUMMY_VAL);
        assert(pid1 > 0);
        printf(" 2");
        r = waitpid(pid1, &rval);
        assert(r == pid1);
        //assert(rval == 3);
        printf(" 3");
        pid1 = start(dummy2, "dummy2", 4000, 100, (void *) (DUMMY_VAL + 1));
        assert(pid1 > 0);
        printf(" 4");
        r = waitpid(pid1, &rval);
        assert(r == pid1);
        //assert(rval == 4);
        printf(" 6.\n");
        return 0;
}


/*******************************************************************************
 * End Test 1
 ******************************************************************************/

void test10_sem() {
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
  printf("test10_sem passed.\n");
  return;
}

void test15_sem() {
        int sem, i;

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
        for (i=0; i<32767; i++) {
                assert(wait(sem) == 0);
        }
        assert(try_wait(sem) == -3);
        assert(scount(sem) == 0);
        assert(sdelete(sem) == 0);
        printf("test15_sem passed\n");
}
