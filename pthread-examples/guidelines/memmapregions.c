#include <stdio.h>
int gi;
int gii= 10;
float gf[20000];
static int gs;
static int gsi = 20;
int func(int a)
{
	int i;
	printf("Func : a : %x, i : %x\n", &a, &i);
}
int main()
{
	int local;
	int larr[10000];
	static int si = 50;

	printf("gi : %x, gii : %x, gs = %x, gf : %x, gsi = %x, \nfunc: %x, main : %x, local : %x, larr : %x, si : %x\n", 
		&gi, &gii, &gs, gf, &gsi, func, main, &local, larr, &si);
	func( 20 );
}

