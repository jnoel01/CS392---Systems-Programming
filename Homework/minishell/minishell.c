/*******************************************************************************
 * Name        : minishell.c
 * Date        : June 25, 2021
 * Description : minishell.c
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT "\x1b[0m"

#define MAX_ARG_COUNT 100

volatile sig_atomic_t signall = 0;
void catch_signal(int sig) {
    signall = sig;
}

//String splitter
int string_to_args(char * stinput, char ** stoutput) {
    char * token = strtok(stinput, " ");
    int lines = 0;

    while (token != NULL) {
		strcpy(stoutput[lines], token);
		token = strtok(NULL, " ");
        lines++;
    }
	stoutput[lines] = NULL;

    return lines;
}

int main() {

    struct sigaction sigact;
    memset( & sigact, 0, sizeof(struct sigaction));
    sigact.sa_handler = catch_signal;

    if (sigaction(SIGINT, & sigact, NULL) < 0) {
        fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }

    char cwd[PATH_MAX];
    size_t bytes_read = 0;
    struct passwd * pwd;
    char *argv[MAX_ARG_COUNT];
    char stinput[256] = "";
	int exit_code = EXIT_SUCCESS;
	int file = 0;

	for (int i = 0; i < MAX_ARG_COUNT; i++) {
    	argv[i] = (char*)calloc(1024, sizeof(char));
    }

    while (1) {

        getcwd(cwd, sizeof(cwd));
        if (cwd == NULL) {
            fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
        }

        printf("%s[%s%s%s]$ ", DEFAULT, BRIGHTBLUE, cwd, DEFAULT);
        fflush(stdout);
        if ((bytes_read = read(STDIN_FILENO, stinput, sizeof(stinput))) == -1) {
            if (errno == EINTR) {
				printf("\n");
			}
            continue;
        }

        stinput[bytes_read - 1] = '\0';

        int argc = 0;
		
		if (stinput[0] ==  '.' && stinput[1] == '/') {
			 if ((argc = string_to_args(stinput + 2, argv)) == 0) {
				continue;
            }
			file = 1;
		} else if ((argc = string_to_args(stinput, argv)) == 0) {
            continue;
        }
		
        if (strcmp(argv[0], "exit") == 0) {
            break;
        }
		
        if (strcmp(argv[0], "cd") == 0) {
            int dirr = 0;
            if (argc > 2) {
                fprintf(stderr, "Error: Too many arguments to cd.\n");
                continue;
            }
            if ((pwd = getpwuid(getuid())) == NULL) {
                fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
                continue;
            }

            if (argc == 1 || (strcmp(argv[1], "~")) == 0) {
                if ((dirr = chdir(getenv("HOME"))) == -1) {
                    fprintf(stderr, "Error: Cannot change directory to home. %s.\n", strerror(errno));
                    continue;
                }
                continue;
            }

            if ((dirr = chdir(argv[1])) == -1) {
                fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", argv[1], strerror(errno));
                continue;
            }

            continue;


            } else {
                pid_t pid;
                if ((pid = fork()) < 0) {
                 fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
                    continue;
                }
                else if (pid == 0) {
					if (file == 1) {
						if ((execv(argv[0], argv) == -1)) {
                            fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                            exit_code = EXIT_FAILURE;
						    break;
                        }
					}
                    if (argc == 1) {
                        if ((execlp(argv[0], argv[0], NULL) == -1)) {
                            fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                            exit_code = EXIT_FAILURE;
						    break;
                        }
                    }
                    if (execvp(argv[0], argv) == -1) {
                        fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                        exit_code = EXIT_FAILURE;
					    break;
                    }
                } 
                else {
                    if (wait(NULL) < 0 && errno != EINTR) {
                        fprintf(stderr, "%sError: wait() failed. %s.\n", DEFAULT, strerror(errno));
                    }
                    wait(NULL);
                }
        }
    }

	for (int i = 0; i < MAX_ARG_COUNT; i++) {
		free(argv[i]);
    }
    return exit_code;
}