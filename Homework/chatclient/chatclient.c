/*******************************************************************************
 * Name    : chatclient.c
 * Date    : July 7, 2021
 * Description : Chat client using TCP/IP
 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "util.h"

int client_socket = -1;
char username[MAX_NAME_LEN + 1];
char inbuf[BUFLEN + 1];
char outbuf[MAX_MSG_LEN + 1];

int handle_stdin() {

    if (get_string(outbuf, MAX_MSG_LEN) == TOO_LONG) {
		fprintf(stderr, "Sorry, limit your message to %d characters.\n", MAX_MSG_LEN);
	}

    if (send(client_socket, outbuf, strlen(outbuf), 0) < 0) {
		fprintf(stderr, "Error: Failed to send message to server. %s.\n", strerror(errno));
    }

    if (strcmp(outbuf, "bye") == 0) {
		printf("Goodbye.\n");
		return 1;
	}
    return 0;
}

int handle_client_socket() {
    int bytes_recvd;
	if ((bytes_recvd = recv(client_socket, inbuf, BUFLEN, 0)) < 0) {
		fprintf(stderr,"Warning: Failed to receive incoming message. %s.\n", strerror(errno));
		return 1;
	}
    
    else if (bytes_recvd == 0 && errno != EINTR) {
		fprintf(stderr, "\nConnection to server has been lost.\n");
		return 1;
	}
	
	inbuf[bytes_recvd] = '\0';

	if (strcmp(inbuf, "bye") == 0) {
		printf("\nServer initiated shutdown.\n");
		return 1;
	}
	else {
		printf("\n%s\n", inbuf);
		return 0;
	}
	return 0;
}


int main(int argc, char *argv[]) {
	int retval = EXIT_SUCCESS;
	if (argc == 1) {
		fprintf(stderr, "Usage: %s <server IP> <port>\n", argv[0]);
		return EXIT_FAILURE;
	}	
	
	struct sockaddr_in server_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&server_addr, 0, addrlen);
	char* ip_input = argv[1];
	
	if (inet_pton(AF_INET, ip_input, &server_addr.sin_addr) <= 0) {
		fprintf(stderr, "Error: Invalid IP address '%s'.\n", ip_input);
		retval = EXIT_FAILURE;
		goto EXIT;
	}
	
	int port = 0;
	if (!parse_int(argv[2], &port, "port number")) {
		retval = EXIT_FAILURE;
		goto EXIT;
	}
	
	if (port < 1024 || port > 65535) {
		fprintf(stderr, "Error: Port must be in range [1024, 65535].\n");
		retval = EXIT_FAILURE;
		goto EXIT;
	}
	
	int userstring = -1;

	while (userstring != OK) {
		printf("Enter your username: ");
		fflush(stdout);
		userstring = get_string(username, MAX_NAME_LEN + 1);
		if (userstring == TOO_LONG) {
			fprintf(stderr, "Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
		}
  	}
	
	server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port); 
	
	printf("Hello, %s. Let's try to connect to the server.\n", username);
	
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error: Failed to create socket. %s.\n", strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }
	
	if (connect(client_socket, (struct sockaddr *)&server_addr, addrlen) < 0) {
        fprintf(stderr, "Error: Failed to connect to server. %s.\n", strerror(errno));
        retval = EXIT_FAILURE;
		goto EXIT;
    }
	
	int bytes_recvd;
	if ((bytes_recvd = recv(client_socket, inbuf, BUFLEN, 0)) < 0) {
		fprintf(stderr, "Error: Failed to receive message from server. %s.\n", strerror(errno));
		retval = EXIT_FAILURE;
		goto EXIT;
	}
	
	if (bytes_recvd == 0) {
		fprintf(stderr, "All connections are busy. Try again later.\n");
		retval = EXIT_FAILURE;
		goto EXIT;
	}
	
	inbuf[bytes_recvd] = 0;
	printf("\n%s\n\n", inbuf);
	
	if (send(client_socket, username, strlen(username), 0) < 0) {
        fprintf(stderr, "Error: Failed to send username to server. %s.\n", strerror(errno));
		retval = EXIT_FAILURE;
		goto EXIT;
	}

	fd_set socks;
	int sockethandle;

 	while(1) {
    	FD_ZERO(&socks);
    	FD_SET(client_socket, &socks);
    	FD_SET(STDIN_FILENO, &socks);
		printf("[%s]: ", username);
    	fflush(stdout);

    	if (select(FD_SETSIZE, &socks, NULL, NULL, NULL) < 0) {
    		fprintf(stderr, "Error: Select Failed. %s.\n", strerror(errno));
    		retval = EXIT_FAILURE;
    		goto EXIT;
    	}

    	if (FD_ISSET(STDIN_FILENO, &socks) && client_socket > -1) {
    		if (handle_stdin() == 1) {
    			retval = EXIT_SUCCESS;
    			goto EXIT;
    		}
    	}

    	if (FD_ISSET(client_socket, &socks) && client_socket > -1) {
    		sockethandle = handle_client_socket();
    		if (sockethandle == 1) {
    			retval = EXIT_SUCCESS;
    			goto EXIT;
    		}
    	}
    }

EXIT:
    if (fcntl(client_socket, F_GETFD) >= 0) {
        close(client_socket);
    }
    return retval;
}