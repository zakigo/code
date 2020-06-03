#include <stdio.h>
#include "zakilog.h"

void main(){
char *a="hello zaki!";
printf("a=%c",a);
zlogli(a);
writeToFile(a);
}
