#include <stdio.h>
#include <string.h>

#define MACSTR "%02X:%02X:%02X:%02X:%02X:%02X"
#define PRINT(buf,i) buf[i+0], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5]

int record=20;
unsigned char databuf[20][6]={
{0x40,0xe2,0x30,0x27,0x5f,0x98},
{0x68,0x39,0x43,0x72,0xc4,0x20},
{0x9c,0xae,0xd3,0x28,0xbc,0xcb},
{0x18,0xbb,0x26,0x00,0xfe,0x39},
{0x20,0x7c,0x8f,0x9a,0xba,0xd7},
{0xb0,0x41,0x1d,0xf5,0x81,0x7f},
{0xd0,0x31,0x10,0x0f,0xb9,0x17},
{0xb0,0x41,0x1d,0xf5,0x81,0x7f},
{0xf8,0xa2,0xd6,0x4c,0x17,0x07},
{0xd0,0x31,0x10,0x0f,0xb9,0x17},
{0x20,0x7c,0x8f,0x9a,0xba,0xd7},
{0xd0,0x31,0x10,0x0f,0xb9,0x17},
{0x20,0x7c,0x8f,0x9a,0xba,0xd7},
{0x9c,0xae,0xd3,0x28,0xbc,0xcb},
{0x08,0x36,0xc9,0x2b,0xc2,0x5a},
{0x9c,0xae,0xd3,0x28,0xbc,0xcb},
{0x20,0x7c,0x8f,0x9a,0xba,0xd7},
{0x9c,0xae,0xd3,0x28,0xbc,0xcb},
{0x6c,0x4d,0x73,0x68,0x36,0x61},
{0x38,0xe6,0x0a,0xa5,0xb5,0x8e}
};

void main(){


			for(int k=0; k<record; k++){
				printf("databuf=%02X:%02X:%02X:%02X:%02X:%02X\n",PRINT(databuf[k], 0));
			}
			int c_list,c_buf=0;//存下数据c_list表示列表循环，c_buf表示最终留下的非重复列表
			int k=0;
			for(c_list=0; c_list<record; c_list++){
					    printf("c_list     %d %d\n",c_list,c_buf);
				for(k=0; k<c_buf; k++){
					if(0==memcmp(&databuf[c_list],&databuf[k],6)){
						printf("same break %d %d\n",c_list,c_buf);
						
						break;
					}
				}
				if(k==c_buf){//全比较完成，都不同就需要拷贝
					
					if(c_buf!=c_list)
memcpy(&databuf[c_buf],&databuf[c_list],6);
					c_buf++;
					printf("different  %d %d\n",c_list,c_buf);
				}
				
			}
			for(int k=0; k<c_buf; k++){
				printf("databuf="MACSTR"\n",PRINT(databuf[k], 0));
			}
}
