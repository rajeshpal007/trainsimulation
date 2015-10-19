//
//  station.c
//  csse3410A4
//
//  Created by Haythm Alshehab on 7/10/2015.
//  Copyright © 2015 Haythm Alshehab. All rights reserved.
//
// Started 09/07/2015 5:17 pm


#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define TRUE 1
#define FALSE 0

void check_arguments(int argc, char* argv[]);
int check_authfile(char* argv[]);
int check_valid_port_num(char *argv[]);


void check_arguments(int argc, char* argv[]) {
    if (argc < 3 || argc > 5) {
        fprintf(stderr, "Usage: station name authfile log-file [port] [host]\n");
        exit(1);
    }
}

/* TODO: Handle other two conditions:
 1. only the first line is empty
 2. string is empty
 */
int check_authfile(char* argv[]) {
    FILE *authFile = fopen(argv[2], "r");
    if (authFile == NULL) {
        fprintf(stderr, "Invalid name/auth\n");
        exit(3);
    }
    return TRUE;
}


int check_valid_port_num(char *argv[]) {
    char *lastLetter;
    int portNum;
    portNum = (int) strtol(argv[3], &lastLetter, 10);

    if (*lastLetter != '\0' || portNum < 2 || portNum > 4) {
        fprintf(stderr, "Invalid port\n");
        exit(3);
    }
    return portNum;
}


int check_argument_number(int argc){
    if(argc < 4) {
        fprintf(stderr, "Usage: station name authfile lof-file [port] [host]\n");
        return 1;
    }

    if (argc > 6) {
        fprintf(stderr, "Usage: station name authfile lof-file [port] [host]\n");
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    /*
     //must have at least 4 arguments.
     if (argc < 4) {
	    fprintf(stderr,"Usage: %s name authfile log-file [port] [host]\n", argv[0]);
	    return 1;
     }

     //cannot have more than 6 arguments.
     if (argc > 6) {
	    fprintf(stderr,"Usage: %s name authfile log-file [port] [host]\n", argv[0]);
	    return 1;
     }*/
    int ret;
    ret = check_argument_number(argc);
    if (ret) {
        return ret;
    }

    //first arg is name
    char *name = argv[1];

    //name must be at least 1 char long
    if (strlen(name) < 1) {
        fprintf(stderr,"Invalid name/auth\n");
        return 2;
    }

    //second argument is the authfile
    FILE *fauth = fopen(argv[2],"r");

    //could not open authfile
    if (!fauth) {
        fprintf(stderr,"Invalid name/auth\n");
        return 2;
    }

    char authstr[128];

    //could not read authstring
    // fgets will read 128 chars or will stop if it getes into newline or EOF
    if (!fgets(authstr, 128, fauth)) {
        fprintf(stderr,"Invalid name/auth\n");
        return 2;
    }

    //authstring must be more than 0 in length
    if (strlen(authstr) == 0) {
        fprintf(stderr,"Invalid name/auth\n");
        return 2;
    }

    //third arg is the logfile
    // a stands for appendnig
    FILE *flog = fopen(argv[3],"a");

    //could not open logfile
    if (!flog) {
        fprintf(stderr,"Invalid name/auth\n");
        return 2;
    }

    //default port is 0 (any)
    int port = 0;

    //parse optional 4th argument
    if (argc > 4) {
        char *end;
        port = strtol(argv[4], &end, 10);

        //the port argument is not a number
        if (*end != '\0') {
            fprintf(stderr,"Invalid port\n");
            return 3;
        }
    }

    char *iface = NULL;

    //parse optional 5th argument
    if (argc > 5) {
        iface = argv[5];
    }

    printf("name:  %s\n", name);
    printf("iface: %s\n", iface);
    printf("port:  %d\n", port);
    printf("auth:  %s\n", argv[2]);
    printf("log:   %s\n", argv[3]);

    int listenfd;
    listenfd = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in servaddr;

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

    servaddr.sin_port=htons(port);
    
    bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    
    if (listen(listenfd,1024) == -1) {
        fprintf(stderr, "Unspecified system call failure\n");
        exit(99);
    }
    
    for (;; ) {
        int connfd;
        socklen_t clilen;
        struct sockaddr_in cliaddr;
        
        connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&clilen);
        
        //TODO: start thread to handle connection here...
        printf("go a connection\n");
        
        close(connfd);
    }
    
    close(listenfd);
    return 0;
    
    
}
