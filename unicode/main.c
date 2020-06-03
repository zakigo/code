#include<stdio.h>
#define _LINE_LENGTH 300
int main(void) 
{
    FILE *file;
    char line[_LINE_LENGTH];
    file = fopen("unicode.txt", "w");
int a=0;
	fprintf(file,"%c",0xFE);
	fprintf(file,"%c",0xFF);
    if (NULL != file)
    {
	for (int i=0; i<65535; i++)
	{
		fprintf(file,"%4X",i);
	fprintf(file,"%c%c\n",i>>8,i);
	}
    }
    else
    {
        return 1;
    }
    fclose(file);
    return 0;
}
