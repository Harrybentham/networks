#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>


//file reading sever

#define SERVER_SOCKET_FILE "/tmp/serverfileread.sock"
#define CLIENT_SOCKET_FILE "/tmp/clientfileread.sock"

int main(){
	int srv, clt, con;
	int addr_len;
	int data=0;
	int i;
	int tmp;
	int count=0;
	char filename[256];
	char filecontent[256];
	int filecontentsize=0;
	FILE *file;


	struct sockaddr_un server_addr;
	struct sockaddr_un client_addr;

	printf("starting sever\n");

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
        printf("Socket Success\n");
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
		printf("got there\n");

		while((con=accept(srv, (struct sockaddr *)&client_addr, &addr_len))>0){ // we wait for a request to arrive
			if(fork()==0){ // we create a new process to handle the request
                    printf("request arrived, recieving content\n");

					if(recv(con, &filename, 256, MSG_WAITALL)>0){ // we read the request
						printf("Server Receieved %s\n", filename);

						file=fopen(filename,"r");

                        if(file==NULL){
                            printf("danger,danger\n");
                        }else{
                        printf("good good\n");
                        }

      //                for(count=0; (count<255) && ((filecontent[count]=fgetc(file)) != EOF);count++){
        //                   printf("%s\n",filecontent);
          //                  printf("count %d\n",count);

             //           }

                        fread(filecontent,255,1,file);
                        printf("%s\n",filecontent);

                        filecontentsize=strlen(filecontent);
                        filecontentsize++;



                        filecontent[filecontentsize]='\r';
                        printf("Server Sending %s\n", filecontent);
                        printf("Size %d\n", filecontentsize);


						send(con, &filecontent,filecontentsize, 0);
						fclose(file);

				}
				close(con);
				exit(0);
			}
			close(con); // server process
			exit(0);
		}
		if(con<0){
			printf("Error %d\n", con);
	}
	}

	return 0;
}
