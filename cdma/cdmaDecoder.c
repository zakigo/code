/*
for 计算机网络基础 CDMA

*/
#include <stdio.h>
void main (){
	int a[]={-1,-1,-1,+1,+1,-1,+1,+1};
	int b[]={-1,-1,+1,-1,+1,+1,+1,-1};
	int c[]={-1,+1,-1,+1,+1,+1,-1,-1};
	int d[]={-1,+1,-1,-1,-1,-1,+1,-1};
	int m[]={1,0,-1};
	int ans[]={-1,+1,-3,+1,-1,-3,+1,+1};
	while(1){
		for(int i =0;i<8;i++)
		printf("%d ",a[i]);
	getchar();
		for(int j=0; j<8; j++)
		{
			for(int k=0;k<3; k++)
			{
				if(ans[j] == a[j]*m[0]+b[j]*m[]+c[j]*m[]+d[j]*m[])
				got=1;
			}
		}
	}


}
