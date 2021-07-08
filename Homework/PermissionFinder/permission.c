/*******************************************************************************
 * Name        : perms.c
 * Date        : June 2, 2021
 * Description : permission  header.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <stdbool.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "permission.h"

static char* permission_string(struct stat *statbuf);
static bool permverify(char *permstrings);

int perms[] = {S_IRUSR, S_IWUSR, S_IXUSR,
               S_IRGRP, S_IWGRP, S_IXGRP,
               S_IROTH, S_IWOTH, S_IXOTH};

static char* permission_string(struct stat *statbuf) {
    char* buf = malloc(sizeof(char) * 10);
    int permission_valid;
    
    for (int i = 0; i < 9; i += 3) {
        permission_valid = statbuf->st_mode & perms[i];
        if (permission_valid) {
            buf[i] = 'r';
            }
            else {
                 buf[i] = '-';
            }
            permission_valid = statbuf->st_mode & perms[i+1];
            if (permission_valid) {
                buf[i+1] = 'w';
             }
             else {
                 buf[i+1] = '-';
             }
             permission_valid = statbuf->st_mode & perms[i+2];
             if (permission_valid) {
                 buf[i+2] = 'x';
             } else {
                 buf[i+2] = '-';
             }
        }
    buf[9] = '\0';
    return buf;
    }


//Verifies the input permission string
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

bool stat_check(char* directory, char* permstrings) {
	char path[PATH_MAX];
    if (realpath(directory, path) == NULL) {
        fprintf(stderr, "Error: Cannot stat '%s'. %s.\n", directory, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(!permverify(permstrings)){
    	printf("Error: Permissions string '%s' is invalid.\n", permstrings);
    	return false;
    }
    
    //code from statfullpath.c
    DIR *dir;
    if((dir = opendir(path)) == NULL) {
    	fprintf(stderr, "Error: Cannot open directory '%s'. %s.\n", path, strerror(errno));
	return false;
    }
    
    struct dirent *entry;
    struct stat buff, sb;
    char full_filename[PATH_MAX + 2];
    size_t pathlen = 0;
    full_filename[0] = '\0';
    
    if(strcmp(path, "/")) {
    	size_t copy_len = strnlen(path, PATH_MAX);
    	memcpy(full_filename, path, copy_len);
    	full_filename[copy_len] = '\0';
    }
    pathlen = strlen(full_filename) + 1;
    full_filename[pathlen - 1] = '/';
    full_filename[pathlen] = '\0';
    
    while ((entry = readdir(dir)) != NULL) {
    	if (strcmp(entry->d_name, ".") == 0 ||
    	strcmp(entry->d_name, "..") == 0) {
    		continue;
    	}
    	strncpy(full_filename + pathlen, entry->d_name, PATH_MAX - pathlen);
    	if (lstat(full_filename, &sb) < 0) {
    		fprintf(stderr, "Error: Cannot stat file '%s'. %s\n", full_filename, strerror(errno));
    	}
    	buff = sb;
    	char* directory_perm = permission_string(&buff);
    	
        if(entry->d_type == DT_DIR){
            if(strcmp(permstrings, directory_perm) == 0){
                printf("%s\n", full_filename);
            }
            stat_check(full_filename, permstrings);
        }
        else {
            if(strcmp(permstrings, directory_perm) == 0){
                printf("%s\n", full_filename);
            }
        }
        free(directory_perm);
    }
    closedir(dir);
    return true;
}

