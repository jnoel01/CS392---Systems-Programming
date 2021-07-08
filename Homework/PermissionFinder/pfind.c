/*******************************************************************************
 * Name        : quicksort.h
 * Date        : June 2, 2021
 * Description : Quicksort header.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "permission.h"

void print_usage() {
	printf("Usage: ./pfind -d <directory> -p <permissions string> [-h]\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1 || argc > 5) {
 	print_usage();
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

    char *dir = argv[2];
    char *perm = argv[4];
    
    stat_check(dir, perm);

    return EXIT_SUCCESS;
		
}	

