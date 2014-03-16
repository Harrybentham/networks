#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

//file reading client

#define SERVER_SOCKET_FILE "/tmp/serverfileread.sock"
#define CLIENT_SOCKET_FILE "/tmp/clientfileread.sock"

int main(){
	int srv, clt, con;
	int addr_len;
	int data=0;
	int i;
	char filename[256];
	char* filecontent=NULL;
	int filecontentsize=0;

	struct sockaddr_un server_addr;
	struct sockaddr_un client_addr;

    printf("please input file name with \n");
    gets(filename);

    printf("you entered %s\n",filename);


	// we set the server address
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX; // this is a local socket, we are on our machine not using the network yet!
	strncpy(server_addr.sun_path, SERVER_SOCKET_FILE, 104);


	// we set the client address
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sun_family = AF_UNIX;
	strncpy(client_addr.sun_path, CLIENT_SOCKET_FILE, 104);


	// fork create a new process, look online if you do not understand!
	if(fork()==0){ // child process
		clt = socket(AF_UNIX, SOCK_SEQPACKET, 0); // we create a socket
		unlink(client_addr.sun_path); // we make sure our file is free
		if(bind(clt, (struct sockaddr *)&client_addr, sizeof(client_addr))<0){ // we tell it to associate our address with that socket
			close(clt);
			printf("Bind failed %d\n", errno);
			return -1;
		}
		if (connect(clt, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){ // we connect to the server
			close(clt);
			printf("Connect failed %d\n", errno);
			return -1;
		}

        filecontentsize=18;
		 // we send data!
        printf("Client sending %s\n" ,filename);

		send(clt, &filename, strlen(filename), 0); // we send a request

//        i=recv(clt, &filecontent, filecontentsize, MSG_WAITALL); // we read the request
        addr_len = sizeof(client_addr);
        while((con=accept(clt, (struct sockaddr *)&server_addr, sizeof(server_addr)))>0){ // we wait for a request to arrive
            if(fork()==0){ // we create a new process to handle the request

                if(recv(clt, &filecontent, filecontentsize, MSG_WAITALL)>0){ // we read the request


                    printf("Client received %s\n", filecontent);

                    close(con);// we close the socket
                }
            }
        }
	}
	return 0;
}
