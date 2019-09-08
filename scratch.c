#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>


int piped=0;
int single_out=0;
int double_out=0;

void initialization(){
 FILE *fp;
 fp=fopen("my_rc.txt","w+");
 if(fp==NULL){
   printf("file opening error\n");
   return;
 }
 // fputs("PATH :");
  char* strr = getenv("PATH");
  fputs(strr,fp);
  fputs("\n",fp);
  char* strr2 = getenv("USER");
  fputs(strr2,fp);
  fputs("\n",fp);
  char* strr4 = getenv("USERNAME");
  fputs(strr4,fp);
  fputs("\n",fp);
  char* strr5 = getenv("HOME");
  fputs(strr5,fp);
  fputs("\n",fp);
  fclose(fp);
}

void view_history(){
   FILE* fp;
   fp=fopen("history.txt","r");
   if(fp==NULL){
      printf("file opening error\n");
      return;
   }
   char a[400];
   while (fgets(a,sizeof(a),fp) != NULL)
    {  
       printf("%s\n",a);
    }
  }
 

void runcmd(char** p){
    if(strcmp(p[0],"cd")==0) 
     { 
       chdir(p[1]);
       return;
     }
    execvp(p[0],p);
    return;
}

void rc_search(char* a){
  FILE* fp;
  fp=fopen("my_rc.txt","r");
  int count=0;
  char data[128];
  int i=0,j=1;
  while(a[j]!='\0') data[i++]=a[j++];
  data[i]='\0';
  if(strcmp(data,"PATH")==0) 
  {
    while (fgets(data,sizeof(data),fp) != NULL)
    {
      if (count == 0)
        {
           printf("%s\n",data);
           break;
        }
    }
  }
  else if(strcmp(data,"USER")==0) 
  {
    while (fgets(data,sizeof(data),fp) != NULL)
    {
      if (count == 1)
        {
           printf("%s\n",data);
           break;
        }
        else
        {
            count++;
        }
    }
  }
  else if(strcmp(data,"USERNAME")==0) 
  {
    while (fgets(data,sizeof(data),fp) != NULL)
    {
      if (count == 2)
        {
           printf("%s\n",data);
           break;
        }
        else
        {
            count++;
        }
    }
  }
  else{
    while (fgets(data,sizeof(data),fp) != NULL)
    {
      if (count == 3)
        {
           printf("%s\n",data);
           break;
        }
        else
        {
            count++;
        }
    }
  }
  fclose(fp);
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
       if(strcmp(temp,">")==0){
         if(strcmp(tok[i-2],">")==0){
             single_out--;
             double_out++;
         }
         else single_out++;
       }
       temp = strtok (NULL, " ");
    }
    return tok;
}

/*void runcmd_io(char** input){
  int fd;
  int pid;
  int output;
  pid = fork();
//  output = open(input[1], O_WRONLY|O_CREAT|O_TRUNC, 0777);
  /* if (output < 0)
    {
      printf("The opening of %s did not complete and has failed\n",input[1]);
      exit(0);
    }
    if (dup2(output, fd) < 0) 
    {
      fprintf(stderr, "dup2 failed to execute\n");
      exit(0);
    }*/
 /*  fd = creat(input[1], 0644);
    dup2(fd, 1);
    //out = 0;
    output = dup(1);
    if (pid == 0) {       
      execvp(input[0], input);
      perror("execvp");
      _exit(1);
} else {
    waitpid(pid, 0, 0);
    dup2(input[1], 1);
    free(res);
}
}*/


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
}

char** get_redirectok(char* input){
    char* temp;
    char** tok;
    tok = (char**)malloc(64*sizeof(char*));
    int i=0;
    temp = strtok (input,">");
    while(temp!=NULL){
      tok[i++]=temp;
       temp = strtok (NULL, ">");
    }
    return tok;
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
    FILE* fp;
    fp=fopen("history.txt","a");
    if(fp==NULL){
       printf("file opening error\n");
       return NULL;
    }
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
     fputs(in,fp);
     fputs("\n",fp);
     fclose(fp);
     return in;
}


int main(){
  initialization();
  static char* input;
  char* input_copy = (char*)malloc(2048*sizeof(char));
  int fd, r; 
  pid_t pid;
    while(1){
    printf("\n$:");
    input = read_input();
    if(input[0]=='$') rc_search(input);
    else if(strcmp(input,"history")==0) view_history();
    else{
    strcpy(input_copy,input);
    char** p;
    p = get_tokens(input);
   // if(strcmp(p[0],"alias")==0) run_alias(p);
    if(strcmp(p[0],"exit")==0){
      exit(0);
      return 0;
     }
    pid = fork();
    if(pid < 0) printf("error in fork\n");
    if(pid==0){
        if(piped==0) runcmd(p);
        else{
             char** piped_inp = (char**)malloc(sizeof(char*));
              piped_inp = get_pipetok(input_copy);
             runcmd_pipe(piped_inp);
        }
    }
    wait(&r);
    piped=0;
   }
}
  exit(0);
  return 0;
}
