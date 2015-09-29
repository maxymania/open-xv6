#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	int* i = 511<<21; // 10000000
	*i = 100;
	*i = *i+*i;
	return *i;
}
