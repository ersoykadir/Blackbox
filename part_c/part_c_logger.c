/**
 * Taken from tut6.c "http://www.qnx.com/developers/docs/qnx_4.25_docs/tcpip50/prog_guide/sock_ipc_tut.html"
 * */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define TRUE 1

/**
 * Creates a scoket and waits for a connection.
 * **/
int
main (int argc, char *argv[])
{
    int sock, length;
    struct sockaddr_in server;
    int msgsock;
    char buf[10000];
    int rval;
    int i;
    FILE *log_file;

    /* Create socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("opening stream socket");
        exit(1);
    }

    /* Name socket using wildcards */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;

    server.sin_port = htons(atoi(argv[2]));/* port number taken from arguments*/

    if (bind(sock, (struct sockaddr *)&server, 
             sizeof(server))) {
        perror("binding stream socket");
        exit(1);
    }
    /* Find out assigned port number and print it out */
    length = sizeof(server);
    if (getsockname(sock, (struct sockaddr *)&server, 
                    &length)) {
        perror("getting socket name");
        exit(1);
    }
    printf("Socket has port #%d\n", ntohs(server.sin_port));

    /* Start accepting connections */
    listen(sock, 5);
    do {
        msgsock = accept(sock, 0, 0);
        if (msgsock == -1) {
            perror("accept");
            return EXIT_FAILURE;
        } else do {
            memset(buf, 0, sizeof(buf));
            if ((rval  = read(msgsock, buf,  10000)) < 0)/* read from the socket stream */
                perror("reading stream message");
            else if (rval == 0){
                //printf("Ending connection\n");
            }
            else{   /* read is successfull*/
                /* write to log file */
            	log_file = fopen(argv[1], "a");
                //printf("-->%s\n", buf);
                fprintf(log_file, "%s\n", buf);
                fclose(log_file);
            }
        } while (rval > 0);
        close(msgsock);
    } while (TRUE);
}