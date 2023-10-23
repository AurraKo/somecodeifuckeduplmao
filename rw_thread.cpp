#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <iostream>
using namespace std;

//global variables
extern pthread_t t[50], t_r[50], t_w[50];
int j=0;
extern pthread_mutex_t lock;
extern int sd;
extern int addr_sz;

// thread functions
void *read_thread(void *arg);

// structs
struct operation{
	int new_socket;
	bool is_dead;
	bool do_kill;
} ;
extern struct sockaddr_in addr;
extern struct sockaddr_storage addr_s;


//code
//
//HANDLES EACH SOCKET. IF THE SOCKET IS CLOSED IT TRIES TO PICK UP ANOTHER ONE.
void *handler(void *arg){
	int new_socket = *(int*)arg; 
	struct operation o;
	int i=0;
	pthread_mutex_lock(&lock);
		i=j;
		o.new_socket = new_socket;
		o.is_dead = false;
		o.do_kill = false;	
		pthread_create(&t_r[j], NULL, read_thread, &o);
		j++;
	pthread_mutex_unlock(&lock); 

	//MAIN LOOP
	while (true){
		if (o.do_kill == true){
			exit(0);
		}
		if (o.is_dead == true){
			close(new_socket);
			new_socket = -1;
			pthread_cancel(t_r[i]);
			while (new_socket==-1){
				new_socket = accept(sd, (struct sockaddr*)&addr_s, (socklen_t*)&addr_sz);	  
				sleep(1);
			}
			o.is_dead=false;
			pthread_create(&t_r[j], NULL, read_thread, &o);
		}
		sleep(1);
	}
	
	pthread_exit(NULL);
	return NULL;
}

void *read_thread(void *arg){
	struct operation *o = (struct operation*)arg;

	int st=0;	
	int sz;
	char BUFF[512]={0};
	for (;;){
			st=recv(o->new_socket, &BUFF, sizeof(BUFF), MSG_NOSIGNAL);
			if (!strcmp(BUFF, ":q")){
				o -> is_dead = true;
				break;
			} else if (!strcmp(BUFF, "*")){
				o -> do_kill = true;
				break;
			}

			sz=strlen(BUFF);
			if (BUFF[0] != 0){
				if (BUFF[sz-1] != '\n'){
				BUFF[sz] = '\n';
				BUFF[sz+1] = '\0';	
			}
			printf("\t\033[33m:%s\033[0m", BUFF);
			fflush(stdout);
		}
		memset(BUFF, 0, sizeof(BUFF));
	}
	
	pthread_exit(NULL);
	return NULL;
}

