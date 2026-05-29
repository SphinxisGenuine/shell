#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_LINE_SIZE 1024
#define BLUE "\033[34m"
#define RESET "\033[0m"
#define CYAN "\033[36m"
#define BOLD "\033[1m"
typedef struct {
  char *input_file;
  char *output_filr;
  int append;
} Redirection;
typedef struct {
  char **argv;
  Redirection redir;
} Command;
void print_banner() {
  printf(
      CYAN
      "╔══════════════════════════════════════════════════════════════╗\n"
      "║                                                             ║\n" RESET

      BOLD "║    ███████╗██████╗ ██╗  ██╗                                 ║\n"
      "║    ██╔════╝██╔══██╗╚██╗██╔╝                                 ║\n"
      "║    ███████╗██████╔╝ ╚███╔╝                                  ║\n"
      "║    ╚════██║██╔═══╝  ██╔██╗                                  ║\n"
      "║    ███████║██║     ██╔╝ ██╗                                 ║\n"
      "║    ╚══════╝╚═╝     ╚═╝  ╚═╝                                 ║\n" RESET

      CYAN
      "║                                                             ║\n" RESET

      "║            S P H I N X   T E R M I N A L                    ║\n"

      CYAN "║                                                             ║\n"
      "╚══════════════════════════════════════════════════════════════╝"
      "\n" RESET);
}
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
void pipeparser(char **argv, Command *left, Command *right) {
  int pipeindex = -1;
  int i = 0;
  while (argv[i] != NULL) {
    if (strcmp(argv[i], "|") == 0) {
      pipeindex = i;
    }
    i++;
  }
  if (pipeindex == -1) {
    return;
  }
  int leftcount = 0;
  int rightcount = 0;
  for (i = 0; i < pipeindex; i++) {
    left->argv[leftcount++] = argv[i];
  }
  i = pipeindex + 1;
  while (argv[i] != NULL) {
    right->argv[rightcount++] = argv[i++];
  }
  left->argv[leftcount] = NULL;
  right->argv[rightcount] = NULL;
  leftcount = parseredirection(&left->redir, left->argv);
  rightcount = parseredirection(&right->redir, right->argv);

  // Pipe cmd_pipe = malloc(sizeof(Pipe));
  // cmd_pipe.left = left;
  // cmd_pipe.right = right;
}
void pipeExecution(Command *left, Command *right) {
  int fds[2];
  pipe(fds);
  pid_t left_pid = fork();
  if (left_pid == 0) {
    dup2(fds[1], 1);
    close(fds[0]);
    close(fds[1]);
    redirection(&left->redir);
    execvp(left->argv[0], left->argv);
    perror("pipe: exec failed");
    exit(1);
  }
  pid_t right_pid = fork();
  if (right_pid == 0) {
    dup2(fds[0], 0);
    close(fds[0]);
    close(fds[1]);
    redirection(&right->redir);
    execvp(right->argv[0], right->argv);
    perror("pipe: exec failed");
    exit(1);
  }
  close(fds[0]);
  close(fds[1]);
  waitpid(left_pid, NULL, 0);
  waitpid(right_pid, NULL, 0);
}
void executeline(char **line) {
  int haspipe = 0;
  int i = 0;
  while (line[i] != NULL) {
    if (strcmp(line[i], "|") == 0) {
      haspipe = 1;
    }
    i++;
  }
  if (strcmp(line[0], "cd") == 0) {
    // compare empty string
    if (line[1] != NULL) {
      if (chdir(line[1]) != 0)
        perror("cd");

      return;
      ;
    }
    char *HOME = getenv("HOME");

    if (chdir(HOME) < 0) {
      perror("cd");
      return;
    }
  } else if (strcmp(line[0], "pwd") == 0) {
    char buffer[100];
    getcwd(buffer, 100);
    printf("%s\n", buffer);
    return;
  } else if (haspipe) {
    Command left, right;
    char *left_argv[100];
    char *right_argv[100];
    left.argv = left_argv;
    right.argv = right_argv;
    right.redir.input_file = NULL;
    right.redir.output_filr = NULL;
    right.redir.append = 0;
    left.redir.input_file = NULL;
    left.redir.output_filr = NULL;
    left.redir.append = 0;
    pipeparser(line, &left, &right);
    pipeExecution(&left, &right);

  } else {
    Redirection redir;
    redir.input_file = NULL;
    redir.output_filr = NULL;
    redir.append = -1;
    int count = parseredirection(&redir, line);
    pid_t p = fork();

    if (p == -1) {
      printf("unable to create child process\n");
    } else if (p == 0) {
      redirection(&redir);
      signal(SIGINT, SIG_DFL);
      execvp(line[0], line);
      perror("exec failed");
    } else {
      waitpid(p, NULL, 0);
    }
  }
}
int main(int argc, char *argv[]) {
  char buffer[MAX_LINE_SIZE];
  char *cmd_argv[100];
  signal(SIGINT, SIG_IGN);
  print_banner();
  FILE *fptr;
  fptr = fopen(".sPxrc", "r");
  char filecontent[100];
  while (fgets(filecontent, sizeof(filecontent), fptr)) {
    tokenize(filecontent, cmd_argv);
    executeline(cmd_argv);
  }
  fclose(fptr);
  while (1) {
    // int fd = open(".sPxrc",O_RDONLY);

    printf(BLUE "sPx" RESET ">>");
    fflush(stdout);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
      break;
    buffer[strcspn(buffer, "\n")] = '\0';

    if (strcmp(buffer, "exit") == 0)
      break;
    int count = tokenize(buffer, cmd_argv);
    executeline(cmd_argv);
    continue;
  }
  return 0;
}
