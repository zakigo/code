#include <stdio.h>
#include "sys/time.h"
#include <stdlib.h>
#ifndef int64
#define int64 long long
#endif

void main(int argc,char **argv){
	struct timeval tv;
	struct timezone tz;
	int64 time=0;
	if(argv[1!=NULL]){
		time =(int64)atoll(argv[1]);
	}
	gettimeofday(&tv,&tz);
	printf("Now is %lld\n", ((int64)tv.tv_sec)*1000 + ((int64)tv.tv_usec)/1000);
	int sec,min,hour,day,mon,year=0;

	printf("%s %s %ld\n",__DATE__,__TIME__,time);
}
