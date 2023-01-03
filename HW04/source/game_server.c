#include <sys/types.h>      //socket, 정수형 확장 자료형, pthread 관련 자료형을 포함하는 헤더
#include <sys/socket.h>     //socket관련 함수, 자료형들을 사용하기 위한 헤더
#include <netinet/in.h>     //network관련 함수, 자료형을 사용하기 위한 헤더
#include <netdb.h>          //network관련 함수, 자료형을 사용하기 위한 헤더
#include <pthread.h>        //POSIX 쓰레드 관련 함수
#include <stdio.h>          //표준입출력
#include <string.h>         //문자열 처리
#include <sys/ipc.h>        //msgq, semaphore등 ipc를 사용하기 위한 헤더
#include <sys/msg.h>        //msg queue 사용을 위한 확장 자료형을 포함하는 헤더

void* serverthread(void* parm); /* thread function prototype    */
void* evalthread();             /* thread function prototype    */

pthread_mutex_t mut;

#define PROTOPORT 5193 /* default protocol port number */
#define QLEN 6         /* size of request queue        */
#define MAXTEXT 100
int visits = 0; /* counts client connections     */

int evalgame(char, char, char);
main(int argc, char* argv[])
{
	struct hostent* ptrh;   /* pointer to a host table entry */
	struct protoent* ptrp;  /* pointer to a protocol table entry */
	struct sockaddr_in sad; /* structure to hold server's address */
	struct sockaddr_in cad; /* structure to hold client's address */
	int sd, sd2;            /* socket descriptors */
	int port;               /* protocol port number */
	int alen;               /* length of address */
	pthread_t tid;          /* variable to hold thread ID */
	pthread_t eid;          /* variable to hold thread ID */
	pthread_mutex_init(&mut, NULL);
	memset((char*)&sad, 0, sizeof(sad)); /* clear sockaddr structure   */
	sad.sin_family = AF_INET;             /* set family to Internet     */
	sad.sin_addr.s_addr = INADDR_ANY;     /* set the local IP address */

	/* Check  command-line argument for protocol port and extract      */
	/* port number if one is specfied.  Otherwise, use the default     */
	/* port value given by constant PROTOPORT                          */

	if (argc > 1)
	{                         /* if argument specified     */
		port = atoi(argv[1]); /* convert argument to binary*/
	}
	else
	{
		port = PROTOPORT; /* use default port number   */
	}
	if (port > 0) /* test for illegal value    */
		sad.sin_port = htons((u_short)port);
	else
	{ /* print error message and exit */
		fprintf(stderr, "bad port number %s/n", argv[1]);
		exit(1);
	}

	/* Map TCP transport protocol name to protocol number */

	if (((int)(ptrp = getprotobyname("tcp"))) == 0)
	{
		fprintf(stderr, "cannot map \"tcp\" to protocol number");
		exit(1);
	}
	/* Create a socket */
	sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sd < 0)
	{
		fprintf(stderr, "socket creation failed\n");
		exit(1);
	}
	/* Bind a local address to the socket */
	if (bind(sd, (struct sockaddr*)&sad, sizeof(sad)) < 0)
	{
		fprintf(stderr, "bind failed\n");
		exit(1);
	}

	/* Specify a size of request queue */
	if (listen(sd, QLEN) < 0)
	{
		fprintf(stderr, "listen failed\n");
		exit(1);
	}

	alen = sizeof(cad);

	pthread_create(&eid, NULL, evalthread, NULL); // eval thread run first

	/* Main server loop - accept and handle requests */
	fprintf(stderr, "Server up and running.\n");
	while (1)
	{

		printf("SERVER: Waiting for contact ...\n");

		if ((sd2 = accept(sd, (struct sockaddr*)&cad, &alen)) < 0)
		{
			fprintf(stderr, "accept failed\n");
			exit(1);
		}
		pthread_create(&tid, NULL, serverthread, (void*)sd2);

	} /* end of while */
	close(sd);
}

void* serverthread(void* parm)
{
	int tsd;
	char buf[100]; /* buffer for string the server sends */
	int n;
	pthread_t id;
	int mqid;
	// struct gameid my_data;

	tsd = (int)parm;
	pthread_mutex_lock(&mut);
	++visits;
	printf(" %d clients connected...\n", visits);
	pthread_mutex_unlock(&mut);

	if (visits == 1)
	{
		if ((mqid = msgget((key_t)1234, 0666 | IPC_CREAT)) == -1)
		{
			printf("1234 msgget failed \n");
			pthread_exit(0);
		}
	}
	else if (visits == 2)
	{
		if ((mqid = msgget((key_t)1235, 0666 | IPC_CREAT)) == -1)
		{
			printf("1235 msgget failed \n");
			pthread_exit(0);
		}
	}
	else if (visits == 3)
	{
		if ((mqid = msgget((key_t)1236, 0666 | IPC_CREAT)) == -1)
		{
			printf("1236 msgget failed \n");
			pthread_exit(0);
		}
	}

	id = pthread_self();
	sprintf(buf, "Your PID in Server is %u .. \n", (unsigned int)id);
	printf("SERVER thread: %s .. waiting..\n", buf);
	send(tsd, buf, strlen(buf), 0);

	while (1)
	{
		n = recv(tsd, buf, sizeof(buf), 0);
		buf[n] = '\0';
		printf("Server thread : you type ->  %s\n", buf);
		switch (buf[0])
		{
		case 'W':
		case 'M':
		case 'V':
			msgsnd(mqid, buf, MAXTEXT, 0);
			printf("Server thread : send it to evalthread \n");
			break;
		case 'Q':
			sprintf(buf, " Finish game .. \n");
			send(tsd, buf, strlen(buf), 0);
			printf("SERVER thread: %u .. exit()..", id);
			if (msgctl(mqid, IPC_RMID, NULL) == -1)
			{
				printf("msgctl failed \n");
			}
			close(tsd);
			pthread_exit(0);
			break;
		default:
			sprintf(buf, " invalid  input.. try again.. \n");
			send(tsd, buf, strlen(buf), 0);
			break;
		}
		if (msgrcv(mqid, buf, MAXTEXT, 0, MSG_NOERROR) == -1)
		{
			printf("mq_b msgrcv failed with error \n");
			pthread_exit(0);
		}
		printf("SERVER : thread(%u) is  %s \n", id, buf);
		send(tsd, buf, strlen(buf), 0);
		buf[0] = '\0'; // clear buff string
	}                  /** end of while */
}

void* evalthread()
{
	int t;
	int mqid_a;
	int mqid_b;
	int mqid_c;
	char bufA[10], bufB[10], bufC[10];
	key_t key_qa = 1234, key_qb = 1235, key_qc = 1236;

	printf("SERVER: eval thread running .\n");
	if ((mqid_a = msgget(key_qa, IPC_CREAT | 0666)) == -1)
	{
		printf("key_qa msgget failed \n");
		pthread_exit(0);
	}
	if ((mqid_b = msgget(key_qb, IPC_CREAT | 0666)) == -1)
	{
		printf("key_qb msgget failed \n");
		pthread_exit(0);
	}
	if ((mqid_c = msgget(key_qc, IPC_CREAT | 0666)) == -1)
	{
		printf("key_qc msgget failed \n");
		pthread_exit(0);
	}
	printf(" three message queues connected\n");
	while (1)
	{
		bufA[0] = '\0';
		bufB[0] = '\0';
		bufC[0] = '\0';
		if (msgrcv(mqid_a, bufA, MAXTEXT, 0, MSG_NOERROR) == -1)
		{
			printf("mg_a msgrcv failed with error \n");
			pthread_exit(0);
		}
		printf(" I get one and waiting : A-> %s \n", bufA);

		if (msgrcv(mqid_b, bufB, MAXTEXT, 0, MSG_NOERROR) == -1)
		{
			printf("mq_b msgrcv failed with error \n");
			pthread_exit(0);
		}
		printf(" I get two and waiting : B-> %s \n", bufB);

		if (msgrcv(mqid_c, bufC, MAXTEXT, 0, MSG_NOERROR) == -1)
		{
			printf("mq_b msgrcv failed with error \n");
			pthread_exit(0);
		}
		printf(" I get three and eval it : C-> %s \n", bufC);

		if (bufA[0] != '\0' && bufB[0] != '\0' && bufC[0] != '\0')
		{
			t = evalgame(bufA[0], bufB[0], bufC[0]);
			printf("SERVER evalgame: eval -> %d\n", t);

			switch (t)
			{
			case 0:
				strcpy(bufA, "retry");
				strcpy(bufB, "retry");
				strcpy(bufC, "retry");
				break;
			case 1:
				strcpy(bufA, "loose");
				strcpy(bufB, "loose");
				strcpy(bufC, "win");
				break;
			case 2:
				strcpy(bufA, "win");
				strcpy(bufB, "win");
				strcpy(bufC, "loose");
				break;
			case 3:
				strcpy(bufA, "loose");
				strcpy(bufB, "win");
				strcpy(bufC, "loose");
				break;
			case 4:
				strcpy(bufA, "loose");
				strcpy(bufB, "win");
				strcpy(bufC, "win");
				break;
			case 5:
				strcpy(bufA, "win");
				strcpy(bufB, "loose");
				strcpy(bufC, "win");
				break;
			case 6:
				strcpy(bufA, "win");
				strcpy(bufB, "loose");
				strcpy(bufC, "loose");
				break;
			} /* end of switch */
			msgsnd(mqid_a, bufA, MAXTEXT, 0);
			msgsnd(mqid_b, bufB, MAXTEXT, 0);
			msgsnd(mqid_c, bufC, MAXTEXT, 0);
		} // end of if
	}     /* end of whie */
}

int evalgame(char a, char b, char c)
{
	switch (a)
	{
	case 'W':
		switch (b)
		{
		case 'W':
			switch (c)
			{
			case 'W':
				return 0; /* retry... */
				break;
			case 'V':
				return 1; /* C win...*/
				break;
			case 'M':
				return 2; /* A,B win...*/
				break;
			}
			break;
		case 'V':
			switch (c)
			{
			case 'W':
				return 3; /* B win...*/
				break;
			case 'V':
				return 4; /* B,C win...*/
				break;
			case 'M':
				return 0; /* retry... */
				break;
			}
			break;
		case 'M':
			switch (c)
			{
			case 'W':
				return 5; /* A,C win...*/
				break;
			case 'V':
				return 0; /* retry... */
				break;
			case 'M':
				return 6; /* A win...*/
				break;
			}
			break;
		}
		break;
	case 'V':
		switch (b)
		{
		case 'W':
			switch (c)
			{
			case 'W':
				return 6; /* A win...*/
				break;
			case 'V':
				return 5; /* A,C win...*/
				break;
			case 'M':
				return 0; /* retry... */
				break;
			}
			break;
		case 'V':
			switch (c)
			{
			case 'W':
				return 2; /* A,B win...*/
				break;
			case 'V':
				return 0; /* retry... */
				break;
			case 'M':
				return 1; /* C win...*/
				break;
			}
			break;
		case 'M':
			switch (c)
			{
			case 'W':
				return 0; /* retry... */
				break;
			case 'V':
				return 3; /* B win...*/
				break;
			case 'M':
				return 4; /* B,C win...*/
				break;
			}
			break;
		}
		break;
	case 'M':
		switch (b)
		{
		case 'W':
			switch (c)
			{
			case 'W':
				return 4; /* B,C win...*/
				break;
			case 'V':
				return 0; /* retry... */
				break;
			case 'M':
				return 3; /* B win...*/
				break;
			}
			break;
		case 'V':
			switch (c)
			{
			case 'W':
				return 0; /* retry... */
				break;
			case 'V':
				return 6; /* A win...*/
				break;
			case 'M':
				return 5; /* A,C win...*/
				break;
			}
			break;
		case 'M':
			switch (c)
			{
			case 'W':
				return 1; /* C win...*/
				break;
			case 'V':
				return 2; /* A,B win...*/
				break;
			case 'M':
				return 0; /* retry... */
				break;
			}
			break;
		}
		break;
	}
} /** end of evalgame */
