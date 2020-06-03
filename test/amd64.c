#include <stdio.h>
void test();
__weak void test()
{
	printf("zal\n");
}
int main(int argc, char *argv[])
{
        printf("Hello World\n");
 test();
        return 0;
}
