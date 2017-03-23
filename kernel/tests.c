#include "stdio.h"
#include "sem.h"
#include "tests.h"

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

        return 5;
}

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
