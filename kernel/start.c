#include "debugger.h"
#include "cpu.h"
#include "console.h"
#include "stdio.h"
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

	printf("\f");
	printf("Coucou _ hello, i=%d", i);

	while(1)
	  hlt();

	return;
}
