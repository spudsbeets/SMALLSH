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
    if WIFEXITED(prior_status) {
        printf("exit value %d\n", WEXITSTATUS(prior_status));
    } else if (WIFSIGNALED(prior_status)) {
        printf("terminated by signal %d\n", WTERMSIG(prior_status));
    }
}


int main() {
    printf("$ smallsh\n");

    /* Citation #1 */
    struct command_line *curr_command;

    /* pid_t sampleSpawn = fork();

    switch(sampleSpawn) {
        case -1:  
            perror("fork() failure!");
            exit(1);
            break;
        case 0:
            sleep(30);
            exit(0);
        default:
            children_pids[child_count++] = sampleSpawn; */

    while (true) {
        curr_command = parse_input();

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
        }
    }

    return EXIT_SUCCESS;
}