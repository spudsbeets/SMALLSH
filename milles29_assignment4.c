/*
Citations:
1) Used and modified code from sample_parse.c, provided in assignment description: https://canvas.oregonstate.edu/courses/2031516/assignments/10318861
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_LENGTH 2048
#define MAX_ARGS 512

/* Citation #1 */
struct command_line {
	char *argv[MAX_ARGS + 1];
	int argc;
	char *input_file;
	char *output_file;
	bool is_bg;
};

/* Citation #1 */
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

int main() {
    /* Citation #1 */
    struct command_line *curr_command;

    printf("$ smallsh\n");

    while (true) {
        curr_command = parse_input();
    }

    return EXIT_SUCCESS;
}