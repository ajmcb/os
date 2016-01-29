#include <string.h>
#include <stdio.h>
#include <signal.h>

void signal_handler(int);
void shell_prompt();
void read_input();

int main() {
    signal(SIGINT, signal_handler);

    while(1) {
        shell_prompt();
        read_input();
    }
    return 0;
}

void signal_handler(int sig) {
    printf("SIGINT");
    signal(SIGINT, signal_handler);
}

void shell_prompt() {
    printf("Shell >>");
}

void read_input() {
    char command[100];

    fgets(command, 100, stdin);
}
