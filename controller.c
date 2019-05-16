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
char *userinput;
void sendCommand();
void updatedash();
void delchar(char *str, char ch);


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
        fprintf(stderr, "error making socket 'condition': %s\n", strerror(errno));
        exit(1);
    }

    /* Client message and  */
    const size_t buffsize = 4096;       /* 4k */
    char incoming[buffsize], outgoing[buffsize];
    size_t msgsize;

	while (state == 1){
    	sprintf(outgoing, "condition:?\n");    
    	sendto(fd, outgoing, strlen(outgoing), 0, address->ai_addr, address->ai_addrlen);

		msgsize = recvfrom(fd, incoming, buffsize, 0, NULL, 0);
    	incoming[msgsize] = '\0';

		//remove % from response
		delchar(incoming, '%');
		//send response to dashboard
		updatedash(incoming);
	}
	return kill;
}
void delchar(char *str, char ch)
{
	char *source, *dest;
	for (source=dest=str; *source!='\0'; source++)
	{
		*dest=*source;
		if (*dest != ch) dest++;
	}
	*dest = '\0';
}
void updatedash(char *msg)
{
	char *host = "127.0.1.1";
    char *port = "65250";
    struct addrinfo *dashaddress;
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };

    int dashsock, err;

    err = getaddrinfo(host, port, &hints, &dashaddress);
    if (err) {
        fprintf(stderr, "Error getting lander: %s\n", gai_strerror(err));
        exit(1);
    }
    dashsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (dashsock == -1) {
        fprintf(stderr, "error making socket 'update dash': %s\n", strerror(errno));
        exit(1);
    }

    /* Client message and  */
    const size_t buffsize = 4096;       /* 4k */
    char outgoing[buffsize];
	
	//send message to dashboard
    strcpy(outgoing, msg);    
    sendto(dashsock, outgoing, strlen(outgoing), 0, dashaddress->ai_addr, dashaddress->ai_addrlen);
	close(dashsock);
}

void *controls()
{
	//start dashboard update thread
	pthread_t condition_thr;
	int th2;
	th2 = pthread_create(&condition_thr, NULL, getCondition, NULL);
	if (th2 != 0) {
		fprintf(stderr, "thread create failed\n");
		exit(-1);
	}
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
	return kill;
}
void sendCommand(char command[4096])
{
    char *host = "127.0.1.1";
    char *port = "65200";
    struct addrinfo *cmdaddress;
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_DGRAM,
    };

    int cmd_sock, err;

    err = getaddrinfo(host, port, &hints, &cmdaddress);
    if (err) {
        fprintf(stderr, "Error getting lander: %s\n", gai_strerror(err));
        exit(1);
    }

	//create socket
    cmd_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (cmd_sock == -1) {
        fprintf(stderr, "error making socket 'send command': %s\n", strerror(errno));
        exit(1);
    }

    //send command to lander   
    sendto(cmd_sock, command, strlen(command), 0, cmdaddress->ai_addr, cmdaddress->ai_addrlen);
}



int main(int argc, char *argv[])
{
	pthread_t control_thr;

	if (pthread_create(&control_thr, NULL, controls, NULL) != 0) {
		fprintf(stderr, "thread create failed\n");
		exit(-1);
	} else {
		pthread_join(control_thr, NULL);
		return 1;
	}
	exit(0);
}
