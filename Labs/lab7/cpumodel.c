// Pledge: I pledge my honor that I have abided by the Stevens Honor System.


#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool starts_with(const char *str, const char *prefix) {
    /* 
       Return true if the string starts with prefix, false otherwise.
       Note that prefix might be longer than the string itself.
     */
	 size_t len = strlen(prefix);
	    if (strncmp(str, prefix, len) != 0) {
	        return false;
	    }
	    return true;

}

int main() {
    /* 
       Open "cat /proc/cpuinfo" for reading, redirecting errors to /dev/null.
       If it fails, print the string "Error: popen() failed. %s.\n", where
       %s is strerror(errno) and return EXIT_FAILURE.
     */
	FILE *info = popen("cat /proc/cpuinfo pd 2>/dev/null", "r");
	if(info == NULL){
		fprintf(stderr, "Error: popen() failed. %s.\n", strerror(errno));
		return EXIT_FAILURE;
	}

    /* 
       Allocate an array of 256 characters on the stack.
       Use fgets to read line by line.
       If the line begins with "model name", print everything that comes after
       ": ".
       For example, with the line:
       model name      : AMD Ryzen 9 3900X 12-Core Processor
       print
       AMD Ryzen 9 3900X 12-Core Processor
       including the new line character.
       After you've printed it once, break the loop.
     */
    char buf[256];
    while (fgets(buf, 256, info)) {
        if (starts_with(buf, "model name")) {
            for(int i  = 0; i < 256; i++) {
                if(buf[i]==':' && buf[i+1]==' ') {
                    char *cpu = &buf[i+2];
                    printf("%s", cpu);
                }
            }
            break;
        }
    } 

    /*
       Close the file descriptor and check the status.
       If closing the descriptor fails, print the string
       "Error: pclose() failed. %s.\n", where %s is strerror(errno) and return
       EXIT_FAILURE.
     */
	int status = pclose(info);
	if(status == -1){
		fprintf(stderr, "Error: pclose() failed. %s.\n", strerror(errno));
		return EXIT_FAILURE;
	}
    return !(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS);
}