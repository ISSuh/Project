#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
	
#define BUF_SIZE 2048

#define IP "192.168.0.22"
#define PORT 9190

struct InfomationAC{
	char ID[45];
	char Name[45];
	char Phon[45];
	char Carinfor[45];
	char X[45];
	char Y[45];
	char Time[45];
};

void *send_msg(void *arg);
void *recv_msg(void *arg);
void error_handling(char *msg);
	
char msg[BUF_SIZE];

int main(){
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void * thread_return;
	
	sock=socket(PF_INET, SOCK_STREAM, 0);
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(IP); //IP
	serv_addr.sin_port=htons(PORT); //PORT
	  
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("connect() error");
	
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);

	close(sock);  
	return 0;
}
	
void *send_msg(void *arg){										// 데이터 전송 스레드
	int sock=*((int*)arg);
	struct InfomationAC Na = {"100","Packmin","01088888888","4E233DF2","238.18989","32.18945646","201703151729"};
	char Sendmsg[BUF_SIZE];

	write(sock, (char*)&Na,sizeof(Na));
	return NULL;
}
	
void *recv_msg(void *arg){										 // 확인문자 받을 스레드
	int sock=*((int*)arg);
	char RecvMsg[BUF_SIZE];
	int str_len;
	
	str_len=read(sock, RecvMsg, BUF_SIZE-1);
	printf("%s\n",RecvMsg);

	return NULL;
}
	
void error_handling(char *msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
