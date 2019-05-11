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
void controls();
void sendCommand();
int main(int argc, char *argv[])
{
	controls();
}


void controls()
{
    int state = 1;
	const size_t buffsize = 4096;
    char msg[buffsize];
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
