#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_LINE_SIZE 1024
typedef struct {
  char *input_file;
  char *output_filr;
  int append;
} Redirection;

int parseredirection(Redirection *redir, char **argv) {
  int i = 0;
  int newcount = 0;
  while (argv[i] != NULL) {
    if (strcmp(argv[i], ">") == 0) {
      redir->output_filr = argv[i + 1];
      redir->append = 0;
      i = i + 2;
      continue;

    } else if (strcmp(argv[i], ">>") == 0) {
      redir->output_filr = argv[i + 1];
      redir->append = 1;
      i = i + 2;
      continue;
    }

    else if (strcmp(argv[i], "<") == 0) {
      redir->input_file = argv[i + 1];
      redir->append = 0;
      i = i + 2;
      continue;
    } else {
      argv[newcount++] = argv[i++];
    }
  }
  argv[newcount] = NULL;
  return newcount;
}
void redirection(Redirection *redir) {
  int flags = O_CREAT | O_WRONLY | (redir->append ? O_APPEND : O_TRUNC);

  if (redir->output_filr != NULL) {
    int fd = open(redir->output_filr, flags, 0644);
    if (fd < 0) {
      perror("fd:");
      exit(1);
    }
    dup2(fd, 1);
    close(fd);
  }
  if (redir->input_file != NULL) {
    int fd = open(redir->input_file, O_RDONLY);
    if (fd < 0) {
      perror("fd:");
      exit(1);
    }
    dup2(fd, 0);
    close(fd);
  }
}

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
      Redirection redir;
      redir.input_file = NULL;
      redir.output_filr = NULL;
      redir.append = -1;
      int count = parseredirection(&redir, cmd_argv);
      pid_t p = fork();

      if (p == -1) {
        printf("unable to create child process\n");
      } else if (p == 0) {
        redirection(&redir);
        execvp(cmd_argv[0], cmd_argv);
        perror("exec failed");
      } else {
        waitpid(p, NULL, 0);
      }
    }
  }
  return 0;
}
