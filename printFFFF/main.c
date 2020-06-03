#include <stdio.h>
char a=0xbf;
void main(){
	printf("0x%02X\n",0xbf);
	printf("0x%02X\n",(char)0xbf);
	printf("0x%02X\n",a);
//solution:
	printf("0x%02X\n",(unsigned char)a);
}
