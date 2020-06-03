#ifndef __ZLOG_H__
#define __ZLOG_H__
#define zlogi(s) printf("%s",s)
#define zlogli(s) printf("%s\n",s)
void writeToFile(char *s)
{
	FILE *fp;
	fp = fopen("/tmp/test.txt","a");
	fprintf(fp, "%s\n---------\n",s);
	fclose(fp);
}
#endif
