/*******************************************************************************
 * Name    : mtsieve.c
 * Date    : June 28, 2021
 * Description : Computes the GCD of two command-line arguments.
 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/sysinfo.h>
#include <stdbool.h>
#include <ctype.h>

#include <limits.h>

int total_count = 0;
pthread_mutex_t lock;

typedef struct arg_struct {
	int start;
	int end;
} thread_args; 

bool hasThrees(int n) {
	int count = 0;
    int rem = 0;
    while (n > 0) {
        rem = n%10;
        if (rem == 3) {
            count++;
		}
        n = n /10;
    }
    return (count >= 2);
}

void *sieve(void *ptr) {

	thread_args *ta = (thread_args *) ptr;
	int a =  ta->start, b = ta->end;
	bool *low_primes = (bool *)malloc((((int)sqrt(b)) + 1)*sizeof(bool));
	for (int i = 0; i <= sqrt(b); i++) {
		low_primes[i] = true;
	}
	for (int i = 2; i <= sqrt(sqrt(b)); i++){
		if (low_primes[i] == true) {
			for (int j = pow(i, 2); j <= sqrt(b); j += i){
				low_primes[j] = false;
			}
		}
	}
	bool *high_primes = (bool *)malloc((b - a + 1)*sizeof(bool));
	
	for (int i = 0; i <= b-a; i++) {
		high_primes[i] = true;
	}
	for (int p = 2; p <= sqrt(b); p++) {
		if (low_primes[p] == true) {
			int i = ceil((double)a/p) * p - a;
			if (a <= p) {
				i = i + p;
			}
			for (int j = i; j <= b-a; j++) {
				if ((j + a)%p == 0) {
					high_primes[j] =false;
					j += p - 1;
				}
			}
		}
	}
	int partial_count = 0;
	for (int i = 0; i <= b-a; i++) {
		if (high_primes[i] == true && hasThrees(a + i)) {
			partial_count++;
		}
	}
	int retval;
    if ((retval = pthread_mutex_lock(&lock)) != 0) {
        fprintf(stderr, "Warning: Cannot lock mutex. %s.\n", strerror(retval));
    }

    total_count += partial_count;

    if ((retval = pthread_mutex_unlock(&lock)) != 0) {
	fprintf(stderr, "Warning: Cannot unlock mutex. %s.\n", strerror(retval));
    }
	
	free(low_primes);
	free(high_primes);

	pthread_exit(NULL);
}

bool numbercheck(char* number) {
	for (int i = 0; i < strlen(number); i++) {
		if(!isdigit(number[i])) {
			return false;
		}
	}
	return true;
}

bool overflow(char num[]) {
	long overflow = atol(num);
	if (overflow < INT_MIN || overflow > INT_MAX) {
		return false;
	} 
	return true;
}

void usage_message(){
	printf("Usage: ./mtsieve -s <starting value> -e <ending value> -t <num threads>\n");
}

int main(int argc, char *argv[]) {
	opterr = 0;
	int opt;
	int flag = 0;
	int inputS=0, inputE=0, inputT=0;
	
	if (argc < 2) {
		usage_message();
		return EXIT_FAILURE;
	}
	
	while ((opt = getopt(argc, argv, "s:e:t:")) != -1) {
		switch (opt) {
			case 's':
				if(!numbercheck(optarg)) {
					fprintf(stderr, "Error: Invalid input '%s' received for parameter '-s'.\n", optarg);
					return EXIT_FAILURE;
				}
				if(!overflow(optarg)) {
					printf("Error: Interger overflow for parameter '-s'.\n");
					return EXIT_FAILURE;
				}
				inputS = atoi(optarg);
				flag += 1;
				break;
			case 'e':
				if (!numbercheck(optarg)) {
					fprintf(stderr, "Error: Invalid input '%s' received for parameter '-e'.\n", optarg);
					return EXIT_FAILURE;
				}
				if(!overflow(optarg)) {
					printf("Error: Interger overflow for parameter '-e'.\n");
					return EXIT_FAILURE;
				}
				inputE = atoi(optarg);
				flag += 2;
				break;
			case 't':
				if (!numbercheck(optarg)) {
					fprintf(stderr, "Error: Invalid input '%s' received for parameter '-t'.\n", optarg);
					return EXIT_FAILURE;
				}
				if(!overflow(optarg)) {
					printf("Error: Interger overflow for parameter '-t'.\n");
					return EXIT_FAILURE;
				}
				inputT = atoi(optarg);
				flag += 4;
				break;
			case '?':
				if (optopt == 'e' || optopt == 's' || optopt == 't') {
					fprintf(stderr, "Error: Option -%c requires an argument.\n", optopt);
				}
				else if (isprint(optopt)) {
					fprintf(stderr, "Error: Unknown option '-%c'.\n", optopt);
				}
				else {
					fprintf(stderr, "Error: Unknown option character '\\x%x'.\n", optopt);
				}
				return EXIT_FAILURE;
		}
	}
	if (optind < argc){
		fprintf(stderr, "Error: Non-option argument '%s' supplied.\n", argv[optind]);
		return EXIT_FAILURE;
	}
	if (flag == 2 || flag == 4 || flag == 6) {
		fprintf(stderr, "Error: Required argument <starting value> is missing.\n");
		return EXIT_FAILURE;
	}

	if (inputS < 2) {
		printf("Error: Starting value must be >= 2.\n");
			return EXIT_FAILURE;
	}
	
	if (flag == 1 || flag == 5) {
		fprintf(stderr, "Error: Required argument <ending value> is missing.\n");
		return EXIT_FAILURE;
	}

	if (inputE < 2) {
		fprintf(stderr, "Error: Ending value must be >= 2.\n");
		return EXIT_FAILURE;
	}
	if (inputE < inputS) {
		printf("Error: Ending value must be >= starting value.\n");
		return EXIT_FAILURE;
	}
	if (flag == 3 ) {
		fprintf(stderr, "Error: Required argument <num threads> is missing.\n");
		return EXIT_FAILURE;
	}
	if (inputT < 1) {
		fprintf(stderr, "Error: Number of threads cannot be less than 1.\n");
		return EXIT_FAILURE;
	}
	if(inputT > 2*get_nprocs()) {
		fprintf(stderr, "Error: Number of threads cannot exceed twice the number of processors(%d).\n", get_nprocs());
		return EXIT_FAILURE;
	}
	int a = 0, b = 0;
	a = inputS;
	b = inputE;
	printf("Finding all prime numbers between %d and %d.\n", a, b);
	
	int retval;
    if ((retval = pthread_mutex_init(&lock, NULL)) != 0) {
        fprintf(stderr, "Error: Cannot create mutex. %s.\n", strerror(retval));
        return EXIT_FAILURE;
    }
	
	int num_threads = 0;
	if (inputT > b-a + 1) {
		num_threads = b-a+1;
	}
	 else {
		num_threads = inputT;
	}
	pthread_t threads[num_threads];
	thread_args targs[num_threads];
	int section_length = ((b-a+1)/num_threads) - 1;
	int extra = 0;
	if ( (b-a+1)%num_threads != 0) {
		extra =  (b-a+1)%num_threads;
	}
	
	printf("%d segments:\n", num_threads);
	
	if (extra > 0) {
		targs[0].start = a;
		targs[0].end = section_length + 1 + a;
		extra--;
	} else {
		targs[0].start = a;
		targs[0].end = section_length + a;
	}
	printf("   [%d, %d]\n", targs[0].start, targs[0].end);
	for (int i = 1; i < num_threads; i++) {
		targs[i].start = targs[i-1].end  + 1;
		if (extra > 0) {
			targs[i].end = targs[i].start + section_length + 1;
			extra--;
		} else  {
			targs[i].end = targs[i].start + section_length;
		}
		printf("   [%d, %d]\n", targs[i].start, targs[i].end);
	}
	
	for (int i = 0; i < num_threads; i++) {
		if ((retval = pthread_create(&threads[i], NULL, sieve, (void *) &targs[i])) != 0) {
			fprintf(stderr, "Error: Cannot create thread %d. %s", i + 1, strerror(retval));
	    return EXIT_FAILURE;
        }
	}
	
	for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Warning: Thread %d did not join properly.\n", i + 1);
        }
    }

    if ((retval = pthread_mutex_destroy(&lock)) != 0) {
        fprintf(stderr, "Error: Cannot destroy mutex. %s.\n", strerror(retval));
    }

	printf("Total primes between %d and %d with two or more '3' digits: %d\n", a, b, total_count);

	return EXIT_SUCCESS;
}
