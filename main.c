#include<stdio.h>
#include<string.h>
#define MAX_LINE_SIZE 1024
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

}


    return  0;
}

