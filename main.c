#include<stdio.h>
#include<string.h>
#define MAX_LINE_SIZE 1024


int tokenize (char *line,char **arg){
int count=0;
char *token = strtok(line, " ");

while (token!=NULL) {
    arg[count]=token;
    count++;
    token = strtok(NULL, " ");
}
arg[count]=NULL;
return count;
}
int main(int argc,char *argv[]){
char buffer[MAX_LINE_SIZE];

while(1){
    printf("sPx>>");
    fflush(stdout);
    if(fgets(buffer,sizeof(buffer),stdin)==NULL)
    break;
     buffer[strcspn(buffer, "\n")] = '\0';

     if (strcmp(buffer,"exit")==0) break;

     printf("You have typed %s\n",buffer);

int count =tokenize(buffer,argv);

for(int i  =0;i<=count;i++){
    printf("[%d]  %s \n",i,argv[i]);
}
}
    return  0;
}

