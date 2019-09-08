#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

int piped=0;

void runcmd(char** p){
    if(strcmp(p[0],"cd")==0) 
     { 
       chdir(p[1]);
       return;
     }
    execvp(p[0],p);
    return;
}


char** get_tokens(char* input){
    char * temp;
    char** tok;
    tok=(char**)malloc(64*sizeof(char*));
    int i=0;
    temp = strtok (input," ");
    while (temp != NULL)
    {  tok[i++]=temp;
      // printf("%s\n",temp);
       if(strcmp(temp,"|")==0){
         piped++;
       }
       temp = strtok (NULL, " ");
    }
    return tok;
}


void runpipe_recursive(char ** input, int count, int fd[2])
{
  if (count == 1)
  {
    close(fd[0]);
    dup2(fd[1], 1);
    close(fd[1]);
    char **tok;
    tok = get_tokens(input[0]);
    if (execvp(tok[0], tok) < 0)
    {
      fprintf(stderr, "\nCould not run command 1 (%s)..\n", input[0]);
      exit(0);
    }
  }
  else
  {
    int fd_2[2];
    pid_t pid;

    if (count == piped+1)
    {
      if (pipe(fd) < 0)
      {
        fprintf(stderr, "pipe error between command %d and %d\n", count-1, count);
        exit(0);
      }
    }

    if (pipe(fd_2) < 0)
    {
      fprintf(stderr, "pipe error between command %d and %d\n", count-1, count);
      exit(0);
    }

    pid = fork();
    if (pid < 0)
    {
      fprintf(stderr, "fork error between command %d and %d\n", count-1, count);
      exit(0);
    }
    else if (pid == 0)
    {
      runpipe_recursive(input, count - 1, fd_2);
    }
    else
    {
      close(fd_2[1]);
      close(fd[0]);
      dup2(fd_2[0], 0);
      if (count < piped + 1)
      {
        dup2(fd[1], 1);
      }

      close(fd_2[0]);
      close(fd[1]);
        
      char **tok;
      tok = get_tokens(input[count-1]);

      int status;
      wait(&status);

      if (execvp(tok[0], tok) < 0)
      {
        fprintf(stderr, "\nCould not run command %d (%s)..\n", count, input[count-1]);
        exit(0);
      }
    }    
  }
}



void runcmd_pipe(char** input){
    int fd[2];
    runpipe_recursive(input, piped+1, fd);
   /* pid_t pid;
    int status;
    if(pipe(fd)<0){
         printf("pipe error\n");
         return;
       }
     pid = fork();
     if(pid<0)
     {
      printf("fork error\n");
      exit(0);
     }
     else if (pid == 0)    
     {
      /*close(fd[0]);
      dup2(fd[1], 1); 
      runcmd(input);                         
      close(fd[1]);*/
    /*  close(fd[0]);
      dup2(fd[1], 1);
      close(fd[1]);
      char** tok1;
      tok1=get_tokens(input[0]);
      execvp(tok1[0],tok1);
    } 
    else                                          
    {
      /*close(fd[1]);
      dup2(fd[0], 0);
      runcmd(p);
      close(fd[0]);*/
   /*   close(fd[1]);
      dup2(fd[0], 0);
      close(fd[0]);
      char** tok;
      tok=get_tokens(input[1]);
      execvp(tok[0],tok);
    }
    wait(&status);*/
}

char** get_pipetok(char* input){
    char* temp;
    char** tok;
    tok = (char**)malloc(64*sizeof(char*));
    int i=0;
    temp = strtok (input,"|");
    while(temp!=NULL){
      tok[i++]=temp;
       temp = strtok (NULL, "|");
    }
    return tok;
}



char* read_input(void){
    int buffer = 2048;
    char* in = (char*)malloc(buffer*sizeof(char));
    if(!in) fprintf(stderr,"memory allocation problem\n");
    int i=0;
    char c;
    while((c=getchar())!='\n')
    {
       if(i>buffer){
            buffer=2*buffer;
            in=realloc(in,buffer);
        }
          in[i++]=c;
    }
     in[i]='\0';
     return in;
}

int main(){
  static char* input;
  char* input_copy = (char*)malloc(2048*sizeof(char));
  int fd, r; 
  pid_t pid;
    while(1){
    printf("\n$:");
    input = read_input();
    strcpy(input_copy,input);
    char** p;
    p = get_tokens(input);
    if(strcmp(p[0],"exit")==0){
      exit(0);
      return 0;
     }
    pid = fork();
    if(pid < 0) printf("error in fork\n");
    if(pid==0){
        if(piped==0) runcmd(p);
        else {
             char** piped_inp = (char**)malloc(sizeof(char*));
              piped_inp = get_pipetok(input_copy);
             runcmd_pipe(piped_inp);
        }
    }
   // printf("%d\n",piped);
    wait(&r);
    piped=0;
}
  exit(0);
  return 0;
}
