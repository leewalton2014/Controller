#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

int state = 1;
void controls();
void sendCommand();
int updatedash();
void delchar(char *str, char ch);
void getCondition();


void *getCondition()
{
	char *host = "127.0.1.1";
    char *port = "65200";
    struct addrinfo *address;
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };

    int fd, err;

    err = getaddrinfo(host, port, &hints, &address);
    if (err) {
        fprintf(stderr, "Error getting lander: %s\n", gai_strerror(err));
        exit(1);
    }
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        fprintf(stderr, "error making socket: %s\n", strerror(errno));
        exit(1);
    }

    /* Client message and  */
    const size_t buffsize = 4096;       /* 4k */
    char incoming[buffsize], outgoing[buffsize];
    size_t msgsize;
    struct sockaddr clientaddr;
    socklen_t addrlen = sizeof(clientaddr);
	
	while (state == 1)
	{
	
    sprintf(outgoing, "condition:?\n");    
    sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);

	msgsize = recvfrom(fd, incoming, buffsize, 0, NULL, 0);
    incoming[msgsize] = '\0';

	//remove %
	delchar(incoming, '%');


	printf("reply \"%s\"\n", incoming);

	int u = updatedash(incoming);
	}
	return null;
}
void delchar(char *str, char ch)
{
	char *src, *dst;
	for (src=dst=str; *src!='\0'; src++)
	{
		*dst=*src;
		if (*dst != ch) dst++;
	}
	*dst = '\0';
}
int updatedash(char *msg)
{
	char *host = "127.0.1.1";
    char *port = "65250";
    struct addrinfo *address;
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };

    int dashsock, err;

    err = getaddrinfo(host, port, &hints, &address);
    if (err) {
        fprintf(stderr, "Error getting lander: %s\n", gai_strerror(err));
        exit(1);
    }
    dashsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (dashsock == -1) {
        fprintf(stderr, "error making socket: %s\n", strerror(errno));
        exit(1);
    }

    /* Client message and  */
    const size_t buffsize = 4096;       /* 4k */
    char incoming[buffsize], outgoing[buffsize];
    size_t msgsize;
    struct sockaddr clientaddr;
    socklen_t addrlen = sizeof(clientaddr);

    strcpy(outgoing, msg);    
    sendto(dashsock, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);
	return 1;
}
void *controls()
{	
	const size_t buffsize = 4096;
    char msg[buffsize];
	system ("/bin/stty raw");
    while(state == 1){
        switch(getchar()){
        case 'w':
            //code to set throttle 100
            sprintf(msg,"command:!\nmain-engine:100\n");
			sendCommand(msg);
            break;
        case 'a':
            //code to set rcs-roll -1
            sprintf(msg,"command:!\nrcs-roll:-1\n");
			sendCommand(msg);
            break;
        case 's':
            //code to set throttle 0
            sprintf(msg,"command:!\nmain-engine:0\n");
			sendCommand(msg);
            break;
        case 'd':
            //code to set rcs-roll 1
            sprintf(msg,"command:!\nrcs-roll:1\n");
			sendCommand(msg);
            break;
		case 'q':
			//code to quit
			state = 0;
			system ("/bin/stty cooked");
			break;
        }
    }
    //check for any keypress
    //send message to lander server  
}
void sendCommand(char command[4096])
{
    char *host = "127.0.1.1";
    char *port = "65200";
    struct addrinfo *address;
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };

    int fd, err;

    err = getaddrinfo(host, port, &hints, &address);
    if (err) {
        fprintf(stderr, "Error getting lander: %s\n", gai_strerror(err));
        exit(1);
    }
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        fprintf(stderr, "error making socket: %s\n", strerror(errno));
        exit(1);
    }

    /* Client message and  */
    const size_t buffsize = 4096;       /* 4k */
    char incoming[buffsize], outgoing[buffsize];
    size_t msgsize;
    struct sockaddr clientaddr;
    socklen_t addrlen = sizeof(clientaddr);

    strcpy(outgoing, command);    
    sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);
}



int main(int argc, char *argv[])
{
	//controls();
	//getCondition();
	int control, condition;
	int th;
	pthread_t thread1, thread2;

	if (pthread_create(&thread1, NULL, controls, NULL) != 0) {
		fprintf(stderr, "thread create failed\n");
		exit(-1);
	} else {
		pthread_join(thread1, NULL); // main thread
		return 1;
	}
	if (pthread_create(&thread2, NULL, getCondition, NULL) != 0) {
		fprintf(stderr, "thread create failed\n");
		exit(-1);
	} else {
		pthread_join(thread2, NULL); // main thread
		return 1;
	}
	

	//th = pthread_create(&thread[0], NULL, controls, NULL);
	
	//th = pthread_create(&thread[1], NULL, getCondition, NULL);
  	
  	//pthread_join(control, NULL);
	//assert(condition == 0);
  	//pthread_join(condition, NULL);
	exit(0);
}
