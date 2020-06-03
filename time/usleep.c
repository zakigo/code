#include <stdio.h>
#include "sys/time.h"
#ifndef int64
#define int64 long long
#endif

void main(int argc,char **argv[]){
	struct timeval tv;
	struct timezone tz;
	while(1)
	{
		usleep(1000*100);//100ms
		gettimeofday(&tv,&tz);
		printf("Now is  %s %s:%d\n",__DATE__,__TIME__,((int64)tv.tv_usec)/1000);
	}
}
