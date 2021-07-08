// I pledge my honor that I have abided by the Stevens Honor System

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

/**
 * Determines whether or not the input string represents a valid integer.
 * A valid integer has an optional minus sign, followed by a series of digits
 * [0-9].
 */
bool is_integer(char *input) {
    int start = 0, len = strlen(input);

    if (len >= 1 && input[0] == '-') {
        if (len < 2) {
            return false;
        }
        start = 1;
    }
    for (int i = start; i < len; i++) {
        if (!isdigit(input[i])) {
            return false;
        }
    }
    return true;
}

/**
 * Takes as input a string and an in-out parameter value.
 * If the string can be parsed, the integer value is assigned to the value
 * parameter and true is returned.
 * Otherwise, false is returned and the best attempt to modify the value
 * parameter is made.
 */
bool get_integer(char *input, int *value) {
    long long long_long_i;
    if (sscanf(input, "%lld", &long_long_i) != 1) {
        return false;
    }
    *value = (int)long_long_i;
    if (long_long_i != (long long)*value) {
        fprintf(stderr, "Warning: Integer overflow with '%s'.\n", input);
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num forks>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int num_forks = 0;
    if (!is_integer(argv[1]) || !get_integer(argv[1], &num_forks) ||
        num_forks <= 0) {
        fprintf(stderr, "Error: Invalid number of forks '%s'.\n", argv[1]);
        return EXIT_FAILURE;
    }

    /*
      Use fork() and execl().

      Create num_forks children processes.
      If any call to fork fails, an error message should be printed and
      the loop should continue.
      Keep track of how many children were actually spawned.
      The error message will be of the form:
      Error: fork[fork_index] failed. %s.
         fork_index will be an integer [1, ..., num_forks]
         %s will be strerror(errno)

      The children should execute randomsleep with no arguments.
      If execl fails, an error message should be printed and
      EXIT_FAILURE should returned.
      The error message will be of the form:
      Error: execl() failed for pid %ld. %s.
         %s will be strerror(errno)
    */

    int pid[num_forks];
     for (int i = 0; i < num_forks; i++) {
         if ((pid[i] = fork()) == -1) {
             fprintf(stderr, "Error: fork[%d] failed.%s\n", i, strerror(errno));
             continue;
         }
         if (pid[i] == 0) {
             execl("randomsleep", "randomsleep", NULL);
             fprintf(stderr, "Error: execl() failed for pid %ld. %s.\n", (long)pid[i], strerror(errno));
             exit(EXIT_FAILURE);
         }
     }



     /*
       TODO:
       Use wait().

       We are in the parent here. Wait for all children that were actually
       spawned. As each child terminates, print the message:
       Child with pid <pid> has terminated.
       If an error occurs during waiting, instead print the message:
       Error: wait() failed. %s.
          %s will be strerror(errno)
     */
     int cur = 0;
     for (int i = 0; i < num_forks; i++) {
         if ((cur = wait(NULL)) < 0) {
                 fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
                 continue;
         }
         printf("Child with pid %ld has terminated.\n", (long)cur);
     }

    printf("All children are done sleeping.\n");
    return EXIT_SUCCESS;
}
