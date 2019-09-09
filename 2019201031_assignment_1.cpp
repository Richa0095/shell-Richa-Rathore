#include <bits/stdc++.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

using namespace std;

int piped=0;
int flag=0;
unordered_map<char*,char*> m;

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
   pid_t pid = getpid();
  char strpid[10];
  sprintf(strpid,"%d", pid);
  fputs(strpid,fp);
  fclose(fp);
}

void run_alias(char **inp){
    char* p = (char*) malloc(256*sizeof(char));
    char* q = (char*) malloc(256*sizeof(char));
    int i=1;
    int j=0;
    if(m.find(inp[0])==m.end()){
     while(inp[1][i+1]){
        p[j++]=inp[1][i++];
     }
     i=1,j=0;
     while(inp[0][i]){
        q[j++]=inp[0][i++];
     }
     m.insert({q,p});
   }
  return;
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
  else if(strcmp(data,"HOME")==0){
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
  else if(strcmp(data,"$")==0){
    while (fgets(data,sizeof(data),fp) != NULL)
    {
      if (count == 4)
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


char **get_tokens(char *input)
{
  char *temp;
  char **tok;
  tok = (char**) malloc(64 * sizeof(char *));
  int i = 0;
  temp = strtok(input, " ");
  if(strcmp(temp,"alias")==0){
    flag=1;
    temp=strtok(NULL,"=");
    while(temp!=NULL){
     tok[i++]=temp;
     temp=strtok(NULL,"=");
    }
    return tok;
  }
  while (temp != NULL)
  {
    //printf("coming outside m.find::%s",temp);
    if(m.find(temp)!=m.end()){
        temp=m[temp];
       // printf("coming inside m.find");
     }
    tok[i++] = temp;
    if (strcmp(temp, "|") == 0 || strcmp(temp, "<") == 0 || strcmp(temp, ">") == 0 || strcmp(temp, ">>") == 0)
    {
      piped++;
    }
    temp = strtok(NULL, " ");
  }
  return tok;
}

char *replace_io_util(char * inputs, int token_id)
{
  int new_size = 4*strlen(inputs)*sizeof(char);
  char *replaced = (char *)malloc(new_size);

  switch (token_id)
  {
    case 1: 
    {
      char *read_io_redir = strstr(inputs, "<");
      if (read_io_redir != NULL)
      {
        char *replaced_it = replaced;
        strcat(replaced_it, "cat ");
        replaced_it = replaced_it + 4;
        char *file_name_it = read_io_redir + 1;
        while(*file_name_it == ' ' && *file_name_it != '\0')
          file_name_it++;

        char *file_name_end = strstr(file_name_it, " ");
        
        while(*file_name_it != '\0' && file_name_it !=  file_name_end)
        {
          *(replaced_it++) = *(file_name_it++);
        }

        strcat(replaced_it, " | ");
        replaced_it += 3;

        char *input_it = inputs;
        while(input_it != read_io_redir)
        {
          *(replaced_it++) = *(input_it++);
        }

        if (file_name_end != NULL)
        {
          char *right = file_name_end;
          while(*right == ' ' && *right != '\0')
            right++;

          strcat(replaced_it, right);
          replaced_it += strlen(right);

        }
        
        *(replaced_it++) = '\0';
      }
      else
      {
        strcpy(replaced, inputs);
      }
      

      break;
    }

    case 2:
    {
      char *read_io_redir = strstr(inputs, ">>");
      char token[3];
      strcpy(token, ">>");
      if (read_io_redir == NULL)
      {
        read_io_redir = strstr(inputs, ">");
        strcpy(token, ">");
      }

      if (read_io_redir != NULL)
      {
        char *replaced_it = replaced;

        char *input_it = inputs;
        while(input_it != read_io_redir)
        {
          *(replaced_it++) = *(input_it++);
        }

        char redirection_command[50];
        strcpy(redirection_command, (strcmp(token, ">") == 0) ? "| write_redirection " : "| append_redirection ");
        
        strcat(replaced_it, redirection_command);
        replaced_it += strlen(redirection_command);

        char *file_name_it = read_io_redir + 1;
        while((*file_name_it == '>' || *file_name_it == ' ') && *file_name_it != '\0')
          file_name_it++;

        char *file_name_end = strstr(file_name_it, " ");
        
        while(*file_name_it != '\0' && file_name_it !=  file_name_end)
        {
          *(replaced_it++) = *(file_name_it++);
        }
        
        *(replaced_it++) = '\0';
      }
      else
      {
        strcpy(replaced, inputs);
      }

      break;
    }
  }

  return replaced;
}

char *replace_io_redirection(char *inputs)
{
  // replace command1 < filename | command2 | .. | commandN _WITH_ cat filname | command | command2 | .. | commandN
  char *replaced = replace_io_util(inputs, 1);
  
  // replace > or >> with | write_redirection
  replaced = replace_io_util(replaced, 2);

  // fprintf(stdout, "\n____replaced: %s____\n", replaced);

  return replaced;
}


void write_fd_to_file(char *path, char *mode, int fd)
{
  int BUFFER_SIZE = 4096;
  void *buffer = malloc(BUFFER_SIZE);
  FILE *fp = fopen(path, mode);
  if (fp == 0)
  {
    fprintf(stderr, "\nAn error occured while writing to file: %s, mode: %s\n", path, mode);
  }

  int bytes_read;
  bytes_read = read(fd, buffer, 64);

  while(bytes_read > 0)
  {
    fwrite(buffer, bytes_read, 1, fp);
    bytes_read = read(fd, buffer, 64);
  }

  fclose(fp);
}

void exec_pipe_cmd(char **input, char **cmd, int fd, int command_no)
{
  if (strcmp(cmd[0], "write_redirection") == 0)
  {
    write_fd_to_file(cmd[1], "w", fd);
    close(fd);
    exit(0);
  }
  else if (strcmp(cmd[0], "append_redirection") == 0)
  {
    write_fd_to_file(cmd[1], "a", fd);
    close(fd);
    exit(0);
  }
  else if (execvp(cmd[0], cmd) < 0)
  {
    fprintf(stderr, "\nCould not run command %d (%s)..\n", command_no, input[command_no-1]);
    exit(0);
  }
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

     // close(fd_2[0]);
      close(fd[1]);
        
      char **tok;
      tok = get_tokens(input[count-1]);

      int status;
      wait(&status);
      exec_pipe_cmd(input, tok, fd_2[0], count);      
    }    
  }
}



void runcmd_pipe(char** input){
    int fd[2];
    runpipe_recursive(input, piped+1, fd);
}

char **get_pipetok(char *input)
{
  char *temp;
  char **tok;
  tok = (char **)malloc(64 * sizeof(char *));
  int i = 0;
  char *replaced_input = replace_io_redirection(input);
  temp = strtok(replaced_input, "|");
  while (temp != NULL)
  {
    tok[i++] = temp;
    // printf("%s\n",temp);
    temp = strtok(NULL, "|");
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
            in=(char*)realloc(in,buffer);
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
  int fd, r; 
  pid_t pid;
    while(1){
    printf("\n$:");
    input = read_input();
    if(strcmp(input,"exit")==0){
          //exit(0);
          return 0;
          return 0;
       }
    if(input[0]=='$') rc_search(input);
    else if(strcmp(input,"history")==0) view_history();
    else{
       int len = strlen(input);
       char *input_copy = (char *) malloc(len * sizeof(char));
       strcpy(input_copy,input);
       char** p;
       p = get_tokens(input);
      if(flag==1){
         run_alias(p);
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
    flag=0;
   }
}
  exit(0);
  return 0;
}
