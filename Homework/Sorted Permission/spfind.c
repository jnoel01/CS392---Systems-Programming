/*******************************************************************************
 * Name        : spfind.c
 * Date        : June 15, 2021
 * Description : spfind.c
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void print_usage() {
   printf("Usage: ./pfind -d <directory> -p <permissions string> [-h]\n");
}

static bool permverify(char* permstrings) {
	if(strlen(permstrings) != 9){
		return false;
	}
	for (int i = 0; i < 9; i += 3) {
		if ((permstrings[i] != 'r') && (permstrings[i] != '-')) {
			return false;
		}
		if ((permstrings[i+1] != 'w') && (permstrings[i+1] != '-')) {
			return false;
		}
		if ((permstrings[i+2] != 'x') && (permstrings[i+2] != '-')) {
			return false;
		}
	}
	return true;
}

int main(int argc, char *argv[]) {
    if (argc == 1 || argc > 5) {
 	print_usage();
	return EXIT_FAILURE;
    }

    if(!permverify(argv[4])) {
    	printf("Error: Permissions string '-%s' is invalid.\n", argv[4]);
    	return EXIT_FAILURE;
    }
    
    opterr = 0;
    int opt;
    int flag = 0;

    while ((opt = getopt(argc, argv, ":d:p:h")) != -1) {
	switch (opt) {
        case 'd':
            flag += 1;
            break;
	    case 'p':
	    	flag += 2;
	    	break;
	    case 'h':
	        print_usage();
	        return EXIT_FAILURE;
	        break;
				
	    case '?':
		printf("Error: Unknown option '-%c' recieved.\n", optopt);
		return EXIT_FAILURE;
	}
    }

    if (flag == 2 || strcmp(argv[1],"-p") == 0) {
    	printf("Error: Required argument -d <directory> not found.\n");
    	return EXIT_FAILURE;
    }
    if (flag == 1 || (argc == 2 && (strcmp(argv[1], "-p") == 0))) {
    	printf("Error: Required argument -p <permissions string> not found.\n");
    	return EXIT_FAILURE;
    }

    int pfind_to_sort[2], sort_to_parent[2];
    if (pipe(pfind_to_sort) < 0) {
        fprintf(stderr, "Error: Cannot create pipe pfind_to_sort. %s.\n", strerror(errno));
        return EXIT_FAILURE;   
    }
    if (pipe(sort_to_parent) < 0) {
        fprintf(stderr, "Error: Cannot create pipe sort_to_parent. %s.\n", strerror(errno));
        return EXIT_FAILURE;    
    }

    pid_t pid[2];
    if ((pid[0] = fork()) == 0) {
        close(pfind_to_sort[0]);
        close(sort_to_parent[0]);
        close(sort_to_parent[1]);
        
        if(dup2(pfind_to_sort[1], STDOUT_FILENO) == -1){
            fprintf(stderr, "Error: dup2 failed. %s.\n", strerror(errno));
            close(pfind_to_sort[1]);
            exit(EXIT_FAILURE);
        }
        if (execv("./pfind", argv) < 0) {
            fprintf(stderr, "Error: pfind failed. %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }
    }

    if ((pid[1] = fork()) == 0) {
    	close(pfind_to_sort[1]);
        close(sort_to_parent[0]);
    	
    	if(dup2(pfind_to_sort[0], STDIN_FILENO) == -1){
    	    fprintf(stderr, "Error: dup2 failed. %s.\n", strerror(errno));
    	    close(pfind_to_sort[0]);
    	    close(sort_to_parent[1]);
    	    exit(EXIT_FAILURE);
    	 }
        
        if(dup2(sort_to_parent[1], STDOUT_FILENO) == -1){
            fprintf(stderr, "Error: dup2 failed. %s.\n", strerror(errno));
            close(sort_to_parent[1]);
            close(pfind_to_sort[0]);
            exit(EXIT_FAILURE);
        }
        
        if (execlp("sort", "sort", NULL) < 0) {
            fprintf(stderr, "Error: sort failed. %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }
    }
   
    close(pfind_to_sort[0]);
    close(pfind_to_sort[1]);
    close(sort_to_parent[1]);
    
    if(dup2(sort_to_parent[0], STDIN_FILENO) == -1){
		fprintf(stderr, "Error: dup2 failed. %s.\n", strerror(errno));
		close(pfind_to_sort[0]);
		exit(EXIT_FAILURE);
    }
    
    int cnt = 0;
    int lines = 0;
    char buffer[4096];
    
    while (1) {
        size_t count = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (count == -1) {
    	    if (errno == EINTR) {
    	       continue;
    	    } else {
    	    	perror("read()");
    	    	return EXIT_FAILURE;
    	    }        
        }
        else if (count == 0) {
    	    break;
        } else {
        	for (int i = 0; i < count; ++i) {
        		if (buffer[i] == '\n') {
        			lines++;
        		}
        	}  
        	write(STDOUT_FILENO, buffer, count);
        }
    }
    close(sort_to_parent[0]);
   
    int cur = 0;
    for (int i = 0; i < 2; i++) {
        if ((cur = wait(NULL)) < 0) {
                fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
                return EXIT_FAILURE;
        }
    }
    printf("Total matches: %d\n", lines);

    return EXIT_SUCCESS;
}
