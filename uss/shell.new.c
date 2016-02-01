#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

void shellPrompt(void);
char *readInput(void);
char **tokeniseCommand(char *cmd);
int internalCommands(char **tokenarray);
int externalCommands(char **tokenarray);
void signalHandler(int sign);
void beginSignals(void);
void exitProgram(int exitCode);

int main() {
    char *command = NULL;
    char **tokenised;
    int internal = -1;
    int external = -1;

    while(1) {
        beginSignals();

        shellPrompt();

        command = readInput();

        if (command == NULL) {
            continue;
        }

        tokenised = tokeniseCommand(command);

        internal = internalCommands(tokenised);

        if (internal == 0) {
            free(command);
            continue;
        }

        external = externalCommands(tokenised);

        if (external == -1) {
            printf("No such file or directory.\n");
            continue;
        }

        free(command);

    }

    exit(0);
    return 0;
}

void shellPrompt(void) {
    char hostname[1024];
    char username[1024];

    memset(hostname, 0, sizeof(hostname));
    memset(username, 0, sizeof(username));

    gethostname(hostname, sizeof(hostname));
    getlogin_r(username, sizeof(username));
    printf("%s@%s: ", username, hostname);
}

char *readInput(void) {
    char *input =  NULL;
    size_t length = 0;
    int read;

    read = getline(&input, &length, stdin);

    if (read < 1) {
        exit(0);
    } else if (read == 1) {
        return NULL;
    }

    input[read - 1] = 0;

    return input;
}

char **tokeniseCommand(char *cmd) {
    static char *arr[sizeof(cmd)];
    char *saveptr;
    int i = 0;
    char *token;

    token = strtok_r(cmd, " ", &saveptr);
    while (token != NULL) {
        arr[i] = token;
        token = strtok_r(NULL, " ", &saveptr);
        ++i;
    }

    for (; i < sizeof(cmd); ++i) {
        *(arr + i) = 0;
    }

    return arr;
}

int internalCommands(char **tokenarray) {
    char *first = NULL;
    char *second = NULL;
    char *third = NULL;
    char *cd = "cd";
    char *pwd = "pwd";
    char *ex = "exit";
    char *endptr;
    int dirSuccess = -1;
    char *workingDirectory = NULL;
    char cwd[1024];
    int exitCode = 0;

    first = *tokenarray;
    second = *(tokenarray + 1);
    third = *(tokenarray + 2);

    if (strcmp(first, ex) == 0) {
        if (third != NULL) {
            printf("Too many arguments.\n");
        } else  if (second == NULL) {
            exitProgram(exitCode);
        } else {
            exitCode = strtol(second, &endptr, 10);
            exitProgram(exitCode);
        }
    } else if (strcmp(first, cd) == 0) {
        if (third != NULL) {
            printf("Too many arguments.\n");
        } else if (second == NULL) {
            dirSuccess = chdir(getenv("HOME"));
            if (dirSuccess != 0) {
                printf("No such file or directory.\n");
            }
        } else {
            if (second == "~") {
                dirSuccess = chdir(getenv("HOME"));
                if (dirSuccess != 0) {
                    printf("No such file or directory.\n");
                }
            } else {
                dirSuccess = chdir(second);
                if (dirSuccess != 0) {
                    printf("No such file or directory.\n");
                }
            }
        }
    } else if (strcmp(first, pwd) == 0) {
        if (second != NULL) {
            printf("Too many arguments.\n");
        } else {
            workingDirectory = getcwd(cwd, sizeof(cwd));
            printf("%s\n", workingDirectory);
        }
    } else {
        return -1;
    }
}

int externalCommands(char **tokenarray) {
    int pid = 0;
    int childStatus;
    int exitCode = 0;
    int w = 0;
    char *params[sizeof(tokenarray)];
    char *temp;
    int i = 0;
    char *first;

    first = *tokenarray;
    *params = "\0";

    temp = *(tokenarray + i + 1);
    while (temp != NULL) {
        *(params + i) = temp;
        temp = *(tokenarray + i + 1);
        ++i;
    }

    pid = fork();
    if (pid == -1) {
        printf("Could not fork and exec. Exiting...\n");
    } else if (pid == 0) {
        execvp(first, params);
    } else {
        w = wait(&childStatus);
        if (w == -1) {
            printf("Error in child process. Exiting...\n");
        }

        if (WIFEXITED(childStatus)) {
            exitCode = WEXITSTATUS(childStatus);
        }
    }

    return exitCode;
}

void signalHandler(int sign) {
    return;
}

void beginSignals(void) {
    struct sigaction sigact;

    sigact.sa_handler = signalHandler;
    sigact.sa_flags = SA_RESTART;
    sigfillset(&sigact.sa_mask);

    if (sigaction(SIGINT, &sigact, 0) == -1) {
        printf("Could not catch SIGINT");
    }
}

void exitProgram(int exitCode) {
    printf("Exiting with exit code %d...\n", exitCode);
    exit(exitCode);
}
