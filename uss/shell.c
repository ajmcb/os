#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void signal_handler(int);
void shell_prompt();
void read_input(char*);

int main() {
  char *command = NULL;
  int read = 0;
  char *commandArray[1024];
  char *cmd;
  char *savePtr;
  char *first;
  char ex[5];
  char cd[3];
  char pwd[4];
  char cwd[1024];
  char *wd;
  size_t len = 0;
  int i = 0;
  int j = 0;
  int success = -1;
  int exitCode = 0;
  pid_t child;
  pid_t w;
  int childStatus = 0;

  signal(SIGINT, signal_handler);

  strcpy(ex, "exit");
  strcpy(cd, "cd");
  strcpy(pwd, "pwd");

  while(1) {
      shell_prompt();

      read = getline(&command, &len, stdin);

      if (read == -1) {
        printf("Failed to read user input. Exiting...\n");
        exitCode = EXIT_FAILURE;
        break;
      } else if (read > 1) {
        command[read - 1] = 0;
      } else {
        continue;
      }

      first = strtok_r(command, " ", &savePtr);
      cmd = strtok_r(NULL, " ", &savePtr);
      //strcpy(first, cmd);
      printf("First: %s\n", first);
      printf("Rest: ");

      while (cmd != NULL) {
        commandArray[i] = cmd;
        cmd = strtok_r(NULL, " ", &savePtr);
        i++;
      }


      printf("\n");
      if (strcmp(first, ex) == 0) {
            printf("Exiting...\n");
            break;
      } else if (strcmp(first, cd) == 0) {
            success = chdir(commandArray[1]);
            if (success != 0) {
              printf("No such file or directory.\n");
            }
      } else if (strcmp(first, pwd) == 0) {
            wd = getcwd(cwd, sizeof(cwd));
            if (wd != NULL) {
              printf("%s\n", cwd);
            }
      } else {
            child = fork();
            if (child == -1) {
              printf("Could not fork and exec. Exiting...\n");
              exitCode = EXIT_FAILURE;
              break;
            } else if (child == 0) {
              execvp(first, commandArray);
              exitCode = EXIT_FAILURE;
              break;
            } else {
              w = wait(&childStatus);
              while (w != child) {
                ;
              }
              if (w == -1) {
                printf("Error in child process. Exiting...\n");
                exitCode = EXIT_FAILURE;
                break;
              }

              if (WIFEXITED(childStatus)) {
                //printf("Child process terminated. Exiting...\n");
                exitCode = WEXITSTATUS(childStatus);
              }
            }
      }

  }

  free(command);
  exit(exitCode);
  return 0;
}

void signal_handler(int sig) {
    printf("SIGINT");
    signal(SIGINT, signal_handler);
}

void shell_prompt() {
    printf("Shell >> ");
}

void read_input(char *command) {
    size_t len = 0;
    ssize_t read;

    read = getline(&command, &len, stdin);

    if (read == -1) {
      printf("ERROR");
    }
}
