/*
Citations:
1) Used and modified code from sample_parse.c, provided in assignment description: https://canvas.oregonstate.edu/courses/2031516/assignments/10318861
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define INPUT_LENGTH 2048
#define MAX_ARGS 512
#define MAX_CHILDREN 25

/* Citation #1 */
/* Struct to store command line information. */
struct command_line {
	char *argv[MAX_ARGS + 1];
	int argc;
	char *input_file;
	char *output_file;
	bool is_bg;
};

/* Setup array for children pids for exit process. */
pid_t children_pids[MAX_CHILDREN];
int child_count = 0;

/* Int type for tracking exit statuses. */
int prior_status = 0;

/* Citation #1 */
/* Parse command lines. */
struct command_line *parse_input() {
    char input[INPUT_LENGTH];
    struct command_line *curr_command = (struct command_line *) calloc(1, sizeof(struct command_line));

    printf(": ");
    fflush(stdout);
    fgets(input, INPUT_LENGTH, stdin);

    char *token = strtok(input, " \n");
    while (token) {
        if (!strcmp(token, "<")) {
            curr_command->input_file = strdup(strtok(NULL, " \n"));
        } else if (!strcmp(token, ">")) {
            curr_command->output_file = strdup(strtok(NULL, " \n"));
        } else if (!strcmp(token, "&")) {
            curr_command->is_bg = true;
        } else {
            curr_command->argv[curr_command->argc++] = strdup(token);
        }
        token = strtok(NULL, " \n");
    }
    return curr_command;
}

/* Built in exit function for shell. */
void builtin_exit() {
    for (int i = 0; i < child_count; i++) {
        kill(children_pids[i], SIGTERM);
    }

    for (int i = 0; i < child_count; i++) {
        waitpid(children_pids[i], NULL, 0);
    }

    exit(0);
}

void builtin_cd(struct command_line *curr_command) {
    char *path;

    if (curr_command->argc > 1) {
        path = curr_command->argv[1];
    } else {
        path = getenv("HOME");
        if (path == NULL) {
            fprintf(stderr, "HOME not set.\n");
            return;
        }
    }

    if (chdir(path) == -1) {
        perror("cd");
    }
}


void builtin_status() {
    if (WIFEXITED(prior_status)) {
        printf("exit value %d\n", WEXITSTATUS(prior_status));
    } else if (WIFSIGNALED(prior_status)) {
        printf("terminated by signal %d\n", WTERMSIG(prior_status));
    }
}


void run_other_cmds(struct command_line *curr_command) {
    pid_t spawn = fork();

    switch(spawn) {
        case -1:  
            perror("fork() failure!");
            exit(1);
            break;
        case 0:
            curr_command->argv[curr_command->argc] = NULL;

            if (curr_command->input_file != NULL) {
                int fd = open(curr_command->input_file, O_RDONLY);

                if (fd == -1) {
                    fprintf(stderr, "cannot open %s for input\n", curr_command->input_file);
                    exit(1);
                }

                dup2(fd, 0);
                close(fd);
            } else if (curr_command->is_bg) {
                int dev_null = open("/dev/null", O_RDONLY);
                dup2(dev_null, 0);
                close(dev_null);
            }

            if (curr_command->output_file != NULL) {
                int fd = open(curr_command->output_file, O_WRONLY | O_CREAT | O_TRUNC);

                if (fd == -1) {
                    fprintf(stderr, "cannot open %s for output\n", curr_command->output_file);
                    exit(1);
                }

                dup2(fd, 1);
                close(fd);
            } else if (curr_command->is_bg) {
                int dev_null = open("/dev/null", O_WRONLY);
                dup2(dev_null, 1);
                close(dev_null);
            }

            execvp(curr_command->argv[0], curr_command->argv);

            perror(curr_command->argv[0]);
            exit(1);
        default:
            if (curr_command->is_bg) {
                printf("background pid is %d\n", spawn);
                fflush(stdout);

                children_pids[child_count++] = spawn;
            } else {
                waitpid(spawn, &prior_status, 0);

                if (WIFSIGNALED(prior_status)) {
                    printf("terminated by signal %d\n", WTERMSIG(prior_status));
                    fflush(stdout);
                }
            }
    }
}


int main() {
    printf("$ smallsh\n");

    /* Citation #1 */
    struct command_line *curr_command;

    while (true) {
        curr_command = parse_input();

        pid_t finished_pid;
        int child_status;

        while ((finished_pid = waitpid(-1, &child_status, WNOHANG)) > 0) {
            printf("background pid %d is done: ", finished_pid);
            
            if (WIFEXITED(child_status)) {
                printf("exit value is %d\n", child_status);
            } else if (WIFSIGNALED(child_status)) {
                printf("terminated by signal %d\n", WTERMSIG(child_status));
            }

            fflush(stdout);
        }

        if (curr_command->argc == 0 || strncmp(curr_command->argv[0], "#", 1) == 0) {
            continue;
        } 
        else if (strcmp(curr_command->argv[0], "exit") == 0) {
            builtin_exit();
        }
        else if (strcmp(curr_command->argv[0], "cd") == 0) {
            builtin_cd(curr_command);
        } 
        else if (strcmp(curr_command->argv[0], "status") == 0) {
            builtin_status();
        } else {
            run_other_cmds(curr_command);
        }
    }

    return EXIT_SUCCESS;
}