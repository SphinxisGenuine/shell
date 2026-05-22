#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_LINE_SIZE 1024

int tokenize(char *line, char **arg) {
  int count = 0;
  char *token = strtok(line, " ");

  while (token != NULL) {
    arg[count] = token;
    count++;
    token = strtok(NULL, " ");
  }
  arg[count] = NULL;
  return count;
}
int main(int argc, char *argv[]) {
  char buffer[MAX_LINE_SIZE];
  char *cmd_argv[100];

  while (1) {
    printf("sPx>>");
    fflush(stdout);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
      break;
    buffer[strcspn(buffer, "\n")] = '\0';

    if (strcmp(buffer, "exit") == 0)
      break;

    printf("You have typed %s\n", buffer);

    int count = tokenize(buffer, cmd_argv);
    if (strcmp(cmd_argv[0], "cd") == 0) {
      // compare empty string
      if (cmd_argv[1] != NULL) {
        if (chdir(cmd_argv[1]) != 0)
          perror("cd");

        continue;
      }
      char *HOME = getenv("HOME");

      if (chdir(HOME) < 0) {
        perror("cd");
        continue;
      }
    } else if (strcmp(cmd_argv[0], "pwd") == 0) {
      char buffer[100];
      getcwd(buffer, 100);
      printf("%s\n", buffer);
      continue;
    } else {
      pid_t p = fork();

      if (p == -1) {
        printf("unable to create child process\n");
      } else if (p == 0) {
        execvp(cmd_argv[0], argv);
        perror("exec failed");
      } else {
        waitpid(p, NULL, 0);
      }
    }
  }
  return 0;
}
