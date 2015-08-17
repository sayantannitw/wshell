#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "algorithm.h"

enum ERROR
{
	EIO=-100,
	EMEMORY,
	EFAILURE,
	EFORK
};
enum BOOLEAN
{
	FALSE,
	TRUE
};

int finish(char x)
{
	if(x=='\n' || x=='\0' || x==EOF)return TRUE;
	else return FALSE;
}
int is_trimmable(char x)
{
	if(x==' ')return TRUE;
	else return FALSE;
}

int trim(char buffer[])
{
	int i,cnt,len;
	i=cnt=len=0;
	while(!finish(buffer[i]))len++,i++; //counting length
	buffer[len]='\0'; 				
	i=0;while(i<len && is_trimmable(buffer[i]) )cnt++,i++; //counting initial spaces
	if(cnt>0)for(i=0;i<=len-cnt;i++)buffer[i]=buffer[i+cnt]; //removin initial space
	len=len-cnt;//adjusting length
	for(i=len-1;i>=0 && is_trimmable(buffer[i]);i--)//removing trailing spaces
			swap_char(&buffer[i+1],&buffer[i]),len--;
	return len;
	//printf("trimmed buffer:%s %d\n",buffer,len);
}


char** parse_args(char* buffer,int *cnt)
{
	int max_len,token_cnt,word_count,i,j,pos,ind,len;
	char ** args=NULL;

	len=trim(buffer);
	if(len==0)return NULL;

	max_len=token_cnt=word_count=i=j=0;
	while(i<len)
	{
		if(buffer[i]==' ')
		{
			max_len=max(max_len,word_count);
			word_count=0;
			token_cnt++;
			j=i+1;
			while(j<len && buffer[j]==' ')j++;
			i=j;
		}
		else
		{
			word_count++;
			i++;
		}
	}
	max_len=max(word_count,max_len);
	token_cnt+=2;//for program name and NULL value
	//printf("%d %d\n",max_len,token_cnt);

	args=(char**)malloc(sizeof(char*)*token_cnt);
	if(args==NULL)exit(EMEMORY);
	for(i=0;i<=token_cnt;i++){
			args[i]=(char*)malloc(sizeof(char)*max_len);
			if(args[i]==NULL)exit(EMEMORY);
	}

	pos=ind=i=0;
	while(i<=len){
		if(i==len){
			args[pos++][ind++]='\0';
			i++;
		}
		else if(buffer[i]==' '){
			j=i+1;
			args[pos][ind]='\0';
			pos++;
			ind=0;
			while(j<len && buffer[j]==' ')j++;
			i=j;
		}
		else{
			args[pos][ind++]=buffer[i];
			i++;
		}
	}
	args[pos]=NULL;
	for(i=0;i<pos;i++)printf("%s\n",args[i]);
	*cnt=token_cnt;	
	return args;
}


char* read_line()
{
	int buffer_size=256;
	char *buffer;
	buffer=(char*)malloc(sizeof(char)*buffer_size);
	if(buffer==NULL)exit(EMEMORY);

	int ch,pos;
	pos=0;
	while(TRUE)
	{
		ch=getchar();
		if(finish(ch)){
			buffer[pos]='\n';
			break;
		}
		else{
			buffer[pos++]=ch;
			if(pos==buffer_size)
			{
				buffer_size+=buffer_size;
				buffer=(char*)realloc(buffer,sizeof(char)*buffer_size);
				if(buffer==NULL)exit(EMEMORY);
			}
		}
	}
	return buffer;

}





int wcd(char **args);
int wexit(char **args);
int whelp(char **args);

char *builtin_name[]={
						"cd",
						"exit",
						"help"
					};

int (*builtin_function[])(char **)={
										&wcd,
										&wexit,
										&whelp
									};
char *help_text[]={
					"cd $dir_name to change directoty\n",
					"exit to quit shell\n",
					"help $command_name -- to know about a command\n"
				};

int num_bultin()
{
	return sizeof(builtin_name)/sizeof(char*);
}	

int wcd(char **args)
{
	if(args==NULL || args[0]==NULL)return 1;
	if(args[1]==NULL)fprintf(stderr, "Expected arguments to \"cd\" \n" );
	if(chdir(args[1])!=0)
		perror("wcd");
	return 1;
}
int wexit(char **args)
{
	return 0;
}
int whelp(char **args)
{
	if(args==NULL || args[0]==NULL)return 1;
	int i,len;
	i=0;
	len=num_bultin();

	for(i=0;i<len;i++)
		if(!strcmp(args[0],builtin_name[i]))break;
	if(args[1]==NULL)printf("%s\n",help_text[i]);
	else{
		for(i=0;i<len;i++)
			if(!strcmp(args[1],builtin_name[i]))break;
		if(i==len)fprintf(stderr, "not a valid command\n");
		else printf("%s\n",help_text[i]);
	}
}

int exec_child(char **args)
{
	pid_t pid,wpid;
	int status;

	pid=fork();
	if(pid==0)
	{
		if(execvp(args[0],args)==-1)
			perror("exec_child");
		exit(EFAILURE);
	}
	else if(pid<0)exit(EFORK);
	else
	{
		do{
			wpid=waitpid(pid,&status,WUNTRACED);
		}while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}


int execute(char **args)
{
	if(args==NULL || args[0]==NULL)
		return 1;
	int i,len;
	i=0;
	len=num_bultin();
	for(i=0;i<len;i++)
	{
		if(!strcmp(args[0],builtin_name[i]))
			return (*builtin_function[i])(args);
	}
	return exec_child(args);
}


void shell_loop(void)
{
  char *line;
  char **args;
  int status,cnt;
  status=TRUE;

  do{
    printf("wshell$ ");
    line =read_line();
    //printf("%s",line);
    args = parse_args(line,&cnt);
    status=execute(args);
    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv)
{
  // Load config files, if any.

  shell_loop();

  // Perform any shutdown/cleanup.
  return 0;
}

