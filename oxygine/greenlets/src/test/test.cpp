#include "test.h"
#include "../greenlet.h"
#include <stdio.h>

int func2(int)
{
	printf("func2 begin\n");
	greenlet *parent = greenlet_getparent();
	greenlet_switch(parent, 0);
	printf("func2 end\n");
	return 0;
}

double vv = 0.0;
int func(int count)
{
	printf("func begin\n");
	greenlet *parent = greenlet_getparent();
	greenlet_switch(parent, 0);

	greenlet *gr = greenlet_create(func2);
	greenlet_run(gr, 0);
	printf("from func2\n");
	greenlet_switch(gr, 0);

	for (int i = 0; i < count; ++i)
	{
		double nv = vv + 1.0;
		greenlet_switch(parent, i);
		printf("nv = %lf\n", nv);
	}

	printf("func end\n");

	return -1;
}

void test()
{
	greenlet *gr = greenlet_create(func);
	greenlet_run(gr, 10);

	while(1)
	{
	    double kv = vv * 5.0;
		int n = greenlet_switch(gr, 0);
		printf("n: %d\n", n);
		printf("kv = %lf\n", kv * 5.0);
		if (n == -1)
			break;
	}
	printf("done\n");
}