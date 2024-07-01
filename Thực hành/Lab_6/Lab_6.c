#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>

#define MAX_LINE 80 
#define HISTORY_COUNT 10 // Số lượng lệnh lịch sử tối đa
pid_t pid;

void on_sigint(){
    printf("\nCtrl C was pressed\n");
    kill(pid, SIGINT);
}

char history[HISTORY_COUNT][MAX_LINE]; // Mảng lưu trữ lịch sử lệnh
int history_count = 0; // Số lượng lệnh đã lưu trữ trong lịch sử

void add_to_history(const char *input) {
    if (history_count < HISTORY_COUNT) {
        strcpy(history[history_count++], input);
    } else {
        // Nếu mảng lịch sử đầy, xóa lệnh đầu tiên và thêm lệnh mới vào cuối
        for (int i = 1; i < HISTORY_COUNT; i++) {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[HISTORY_COUNT - 1], input);
    }
}

void print_history() {
    printf("Command history:\n");
    for (int i = history_count - 1; i >= 0; i--) {
        printf("%d: %s\n", history_count - i - 1, history[i]);
    }
}

int main(void) {
    char *args[MAX_LINE]; 
    int should_run = 1; 
    char input[MAX_LINE];
    char *args_pipe[MAX_LINE];

    while (should_run){ 
        signal(SIGINT, on_sigint);
        printf("it007sh>"); 
        fflush(stdout);
        
        fgets(input, MAX_LINE, stdin);
        input[strlen(input) - 1] = '\0'; 

        if (strcmp(input, "exit") == 0){
            should_run = 0;
            break;
        }

        if (strcmp(input, "history") == 0) {
            print_history();
            continue;
        }

        if (strcmp(input, "HF") == 0) {
            if (history_count > 0) {
                print_history();
                continue;
            } else {
                printf("No commands in history.\n");
                continue;
            }
        }

        add_to_history(input); // Thêm lệnh vào lịch sử

        char *token = strtok(input, " ");
        int i = 0;
        while (token != NULL){
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        int chuyenhuongdauvao = 0, chuyenhuongdaura = 0, pipe_idx = -1;
        char *inputFile = NULL, *outputFile = NULL;
        for (int i = 0; args[i] != NULL; i++){
            if (strcmp(args[i], "<") == 0){
                chuyenhuongdauvao = 1;
                inputFile = args[i + 1];
                args[i] = NULL;
            }
            else if (strcmp(args[i], ">") == 0){
                chuyenhuongdaura = 1;
                outputFile = args[i + 1];
                args[i] = NULL;
            }
            else if (strcmp(args[i], "|") == 0){
                pipe_idx = i;
                args[i] = NULL;
                int j = 0;
                for (int m = i+1; args[m] != NULL; m++){
                    args_pipe[j++] = args[m];
                    args[m] = NULL;
                }
                args_pipe[j] = NULL;
                break;
                }
            }
        pid = fork();
        if (pid < 0){
            printf("Failed to fork.\n");
            exit(1);
        }
        else if (pid == 0){
            if (chuyenhuongdauvao){
                int fd = open(inputFile, O_RDONLY);
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            if (chuyenhuongdaura){
                int fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            if (pipe_idx != -1){
                int fd[2];
                pipe(fd);
                pid_t pid2 = fork();
                if (pid2 == 0){
                    close(fd[0]);
                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[1]);
                    execvp(args[0], args);
                    exit(1);
                }
                else{
                    close(fd[1]);
                    dup2(fd[0], STDIN_FILENO);
                    close(fd[0]);
                    execvp(args_pipe[0], args_pipe);
                    exit(1);
                }
            }
            else{
                execvp(args[0], args);
                perror("execvp failed");
                exit(1);
            }
        }
        else 
            wait(NULL);
    }
    return 0;
}
