#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define SERVER_SOCKET_FILE "/tmp/server.sock"
#define CLIENT_SOCKET_FILE "/tmp/client.sock"

int main(){
	int srv, clt, con;
	int addr_len;
	int data=0;
	int i;

	struct sockaddr_un server_addr;
	struct sockaddr_un client_addr;

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
        //do nothing
	}else{ // parent process
		srv = socket(AF_UNIX, SOCK_SEQPACKET, 0); // we create a socket!
		if(srv<0){
			printf("Socket failed %d\n", errno);
			return -1;
		}
		unlink(server_addr.sun_path); // we make sure our file is free
		if(bind(srv, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){ // we associate our address with our socket
			close(srv);
			printf("Bind failed %d\n", errno);
			return -1;
		}
		if(listen(srv, 0)<0){ // we listen for people trying to talk to us
			close(srv);
			printf("Listen failed %d\n", errno);
			return -1;
		}
		addr_len = sizeof(client_addr);
		while((con=accept(srv, (struct sockaddr *)&client_addr, &addr_len))>0){ // we wait for a request to arrive
			if(fork()==0){ // we create a new process to handle the request
				while(data<100){
					if(recv(con, &data, sizeof(int), MSG_WAITALL)>0){ // we read the request
						printf("Server Receieved %d\n", data);
						data++;
						printf("Server Sending %d\n", data);
						send(con, &data, sizeof(int), 0); // we send our answer
					}
				}
				close(con);
				exit(0);
			}
			close(con); // server process
			exit(0);
		}
		if(con<0)
			printf("Error %d\n", con);
	}
	return 0;
}
