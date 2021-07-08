/*******************************************************************************
 * Name        : sort.c
 * Date        : June 2, 2021
 * Description : Uses quicksort to sort a file of either ints, doubles, or
 *               strings.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quicksort.h"

#define MAX_STRLEN     64 // Not including '\0'
#define MAX_ELEMENTS 1024

typedef enum {
    STRING,
    INT,
    DOUBLE
} elem_t;

/**
 * Reads data from filename into an already allocated 2D array of chars.
 * Exits the entire program if the file cannot be opened.
 */

int row = 0; 

size_t read_data(char *filename, char **data) {
    // Open the file.
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot open '%s'. %s.\n", filename,
                strerror(errno));
        free(data);
        exit(EXIT_FAILURE);
    }

    // Read in the data.
    size_t index = 0;
    char str[MAX_STRLEN + 2];
    char *eoln;
    while (fgets(str, MAX_STRLEN + 2, fp) != NULL) {
        eoln = strchr(str, '\n');
        if (eoln == NULL) {
            str[MAX_STRLEN] = '\0';
        } else {
            *eoln = '\0';
        }
        // Ignore blank lines.
        if (strlen(str) != 0) {
            data[index] = (char *)malloc((MAX_STRLEN + 1) * sizeof(char));
            strcpy(data[index++], str);
            row++;
        }
    }

    // Close the file before returning from the function.
    fclose(fp);

    return index;
}

/**
 * Basic structure of sort.c:
 *
 * Parses args with getopt.
 * Opens input file for reading.
 * Allocates space in a char** for at least MAX_ELEMENTS strings to be stored,
 * where MAX_ELEMENTS is 1024.
 * Reads in the file
 * - For each line, allocates space in each index of the char** to store the
 *   line.
 * Closes the file, after reading in all the lines.
 * Calls quicksort based on type (int, double, string) supplied on the command
 * line.
 * Frees all data.
 * Ensures there are no memory leaks with valgrind. 
 */


//Usage Message
void usage_message(){
	printf("Usage: ./sort [-i|-  d] filename\n"
			"   -i: Specifies the file contains ints.\n"
			"   -d: Specifies the file contains doubles.\n"
			"   filename: The file to sort.\n"
			"   No flags defaults to sorting strings.\n");
}

int main(int argc, char **argv) {
	elem_t type = STRING;
	if(argc == 1) {
		usage_message();
		return EXIT_FAILURE;
	}
	opterr = 0;
	int opt;
	int flag;
	while ((opt = getopt(argc, argv, "id")) != -1) {
		switch (opt) {
				case 'i':
					flag = 1;
					type += INT;
					break;
				case 'd':
					flag = 2;
					type += DOUBLE;
					break;
				case '?':
					printf("Error: Unknown option '-%c' recieved.\n", optopt);
					usage_message();
					return EXIT_FAILURE;
		}
	}
	if(argc == 2 && optind == 2) {
		printf("Error: No input file specified.\n");
		return EXIT_FAILURE;
	}

	if((argc > 2 && optind == 1) || (argc > 3 && optind == 2)) {
		printf("Error: Too many files specified.\n");
		return EXIT_FAILURE;
	}

	if(type > 2) {
	   printf("Error: Too many flags specified.\n");
		return EXIT_FAILURE;
	}
	
	//Opens file
	FILE *infile;
	char **data = (char **)malloc(MAX_ELEMENTS);
	
	if(type == STRING) {
		if(!(infile = fopen(argv[1], "r"))){
			printf("Error: Can not open '%s'. %s\n", argv[1], strerror(errno));
			return EXIT_FAILURE;
		}
		
		//Reading in strings
		read_data(argv[1], data);
		
		quicksort(data, row, sizeof(char*), str_cmp);
		for(int i = 0; i < row; ++i){
			printf("%s\n", data[i]);
		}
	}
	else{
		if(!(infile = fopen(argv[2], "r"))){
			printf("Error: Can not open '%s'. %s\n", argv[2], strerror(errno));
			return EXIT_FAILURE;
		}
		read_data(argv[2], data);
		
		//Reading in int/doubles
		if (flag == 1){
			int intarray[row];
			for (int i =0; i < row; ++i){
				intarray[i] = atoi(data[i]);
			}
			quicksort(intarray, row, sizeof(int), int_cmp);
			for(int i = 0; i < row; ++i){
				printf("%i\n", intarray[i]);
			}
		}
		else {
			double dblarray[row];
			for (int i = 0; i < row; ++i) {
				dblarray[i] = atof(data[i]);
			}
			quicksort(dblarray, row, sizeof(double), dbl_cmp);
			for(int i = 0; i < row; ++i){
				printf("%f\n", dblarray[i]);
			}
		}
	}
	
	for (int i = 0; i < row; ++i){
		free(data[i]);
	}
	
	free(data);
	
    return EXIT_SUCCESS;
}
