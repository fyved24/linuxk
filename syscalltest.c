#include <syscall.h>
#include <stdio.h>
int main(void)
{
	printf("%d\n",syscall(251));
	return 0;
}
