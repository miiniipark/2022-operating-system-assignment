#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define closesocket      close
#define PROTOPORT        5193        /* default protocol port number */

extern int               errno;
char   localhost[] = "localhost";    /* default host name            */

main(int argc, char *argv[])
{
    struct hostent *ptrh;   /* pointer to a host table entry       */
    struct protoent *ptrp;  /* point to a protocol table entry     */
    struct sockaddr_in sad; /* structure to hold server's address  */
    int sd;                 /* socket descriptor                   */
    int port;               /* protocol port number                */
    char *host;             /* pointer to host name                */
    int n;                  /* number of characters read           */
    char buf[100];          /* buffer for data from the server     */
    int flag = 1;

    memset((char *)&sad, 0, sizeof(sad)); /* clear sockaddr structure */
    sad.sin_family = AF_INET;             /* set family to Internet   */

    /* Check command-line argument for protocol port and extract     */
    /* port number if on is specified.  Otherwise, use the default   */
    /* port value biven by constant PROTOPORT                        */

    if (argc > 2)
        port = atoi(argv[2]);
    else
        port = PROTOPORT;
    if (port > 0)
        sad.sin_port = htons((u_short)port);
    else
    {
        fprintf(stderr, "bad port number %s\n", argv[2]);
        exit(1);
    }
    if (argc > 1)
        host = argv[1];
    else
        host = localhost;
    ptrh = gethostbyname(host);
    if (((char *)ptrh) == NULL)
    {
        fprintf(stderr, "invalid host:  %s\n", host);
        exit(1);
    }
    memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
    if (((int)(ptrp = getprotobyname("tcp"))) == 0)
    {
        fprintf(stderr, "cannot map \"tcp\" to protocol number\n");
        exit(1);
    }
    sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sd < 0)
    {
        fprintf(stderr, "socket creation failed\n");
        exit(1);
    }
    if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0)
    {
        fprintf(stderr, "connect failed\n");
        exit(1);
    }
    n = recv(sd, buf, sizeof(buf), 0);
    fprintf(stderr, "CLIENT: %s \n", buf);
    fprintf(stderr, "CLIENT: connected \n");
    buf[n] = '\0';
    while (flag)
    {
        switch (flag)
        {
        case 1:
            gets(buf);
            fprintf(stderr, "CLIENT: you type %s \n", buf);
            if (buf[0] == 'W' || buf[0] == 'M' || buf[0] == 'V')
            {
                fprintf(stderr, "CLIENT: valid and send it to server  %s \n", buf);
                send(sd, buf, strlen(buf), 0);
                flag = 2;
            }
            else
                flag = 1; /* continue */
            break;
        case 2:
            n = recv(sd, buf, sizeof(buf), 0);
            buf[n] = '\0';
            fprintf(stderr, "CLIENT: %s \n\n", buf);
            if (strncmp(buf, "retry", 5) == 0)
            {
                printf("CLIENT : same value.. Retry again..\n");
                flag = 1;
            }
            else
            {
                fprintf(stderr, "CLIENT: play the game again ? (y/n)..\n");
                gets(buf);
                if (buf[0] == 'y')
                {
                    flag = 1;
                    buf[0] = '\0';
                }
                else
                {
                    buf[0] = 'Q';
                    buf[1] = '\0';
                    send(sd, buf, strlen(buf), 0);
                    fprintf(stderr, "CLIENT: finish game..\n");
                    closesocket(sd);
                    exit(0);
                }
            } // else
            break;
        } // end of switch
    }     // end of while
    closesocket(sd);
    exit(0);
}
