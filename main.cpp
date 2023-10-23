#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

//global variables
struct sockaddr_in addr;
struct sockaddr_storage addr_s;
int addr_sz;
pthread_t h, t[50], t_r[50];
pthread_mutex_t lock; 

int sd=-1, ns=-1;
int fr_v[10]={0};

//functions


//threads
extern void *handler(void *arg);

int main(){
	pthread_mutex_init(&lock, NULL);
	int opt=1;
	sd = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(8081);
	inet_pton(AF_INET, "192.168.192.231", &addr.sin_addr);

	bind(sd, (struct sockaddr*)&addr, sizeof(addr));
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));  

	listen(sd, 10); // 10 clients
	
	//idk how to handle disconects properly man. T_T
	for (int i=0; i<10; i++){
		while (ns<0){
			addr_sz = sizeof(addr_s);
			
				ns = accept(sd, (struct sockaddr*)&addr_s, (socklen_t*)&addr_sz);
			
			sleep(1);
		}

		pthread_create(&t[i], NULL, handler, (void*)&ns);
		sleep(1);
		ns=-1;
	}
	
	for (;;);
	pthread_mutex_destroy(&lock);

}
