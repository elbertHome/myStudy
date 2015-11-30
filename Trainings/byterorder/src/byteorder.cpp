/*
 * byteorder.cpp
 *
 *  Created on: Jul 13, 2015
 *      Author: elbert
 */


#include "unp.h"

int main(int argc, char ** argv)
{
	union
	{
		short s;
		char c[sizeof(short)];
	} un;

	un.s = 0x0102;
	printf("sizeof(char) = %d \n",sizeof(char));
	printf("sizeof(short) = %d \n",sizeof(short));
	printf("sizeof(int) = %d \n",sizeof(int));
	printf("sizeof(long) = %d \n",sizeof(long));
	printf("sizeof(longlong) = %d \n",sizeof(long long));
	printf("sizeof(int*) = %d \n",sizeof(int *));
	printf("%s: ", CPU_VENDOR_OS);

	if (sizeof(short) == 2)
	{
		if (un.c[0] == 1 && un.c[1] == 2)
		{
			printf("big-endian \n");
		}
		else if(un.c[0] == 2 && un.c[1] == 1)
		{
			printf("little-endian \n");
		}
		else
		{
			printf("unknown \n");
		}
	}
	else
	{
		printf("sizeof(short) = %d \n",sizeof(short));
	}

	exit(0);

}



