#include<stdio.h>
#include<string.h>
#define MAX_LINE_SIZE 1024
int main(int argc,char *argv[]){
char buffer[MAX_LINE_SIZE];

while(1){
    printf(">>spX~~");
 fflush(stdout);                          // flush prompt before blocking

    if (fgets(buffer, MAX_LINE_SIZE, stdin) == NULL)
        break;                               // Ctrl-D exits cleanly

    buffer[strcspn(buffer, "\n")] = '\0';    // strip trailing newline

    if (strcmp(buffer, "exit") == 0)
        break;

    printf("you typed: %s\n", buffer); 

    
}


    return  0;
}

