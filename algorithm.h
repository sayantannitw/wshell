#ifndef ALGORITHM_H
#define ALGORITHM_H


#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>

int max(int x,int y);
int min(int x,int y);
void swap_char(char* x,char* y);


int max(int x,int y)
{
	return (x>y)?x:y;
}
int min(int x,int y)
{
	return (x<y)?x:y;
}
void swap_char(char* x,char* y)
{
	char t=*x;
	*x=*y;
	*y=t;
}

#endif