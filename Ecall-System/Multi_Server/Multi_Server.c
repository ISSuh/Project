#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "/usr/include/mysql/mysql.h"

#define PORT 9190

#define DBID "pi"
#define DBPW "1"
#define DBADR "localhost"
#define DBNAME "TEST_DB"

#define BUF_SIZE 2048
#define QUERY_BUF_SIZE 1024
#define MAX_CLNT 256

MYSQL *conn_ptr;
MYSQL_RES *res;

void *handle_clnt(void * arg);							// 쓰레드 함수
void send_msg();										// 확인 메세지 전송함수
void error_handling(char * msg);						// 오류처리

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

struct AcInfomation{
	char ID[45];
	char Name[45];
	char Phon[45];
	char Carinfor[45];
	char X[45];
	char Y[45];
	char Time[45];
};

int main(){
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;

	conn_ptr = mysql_init(NULL);

	if(!conn_ptr){
		fprintf(stderr,"mysql_init failed\n");
		exit(1);
	}

	conn_ptr = mysql_real_connect(conn_ptr,DBADR,DBID,DBPW,DBNAME,0,NULL,0);

	if(conn_ptr)
		printf("DB connection success\n");
	
	else
		printf("DB connection failed\n");

  
	pthread_mutex_init(&mutx, NULL);
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(PORT);
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	while(1){
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		
		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++]=clnt_sock;
		pthread_mutex_unlock(&mutx);
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_join(t_id,NULL);
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}

	mysql_close(conn_ptr);
	close(serv_sock);
	return 0;
}
	
void * handle_clnt(void * arg){
	int clnt_sock=*((int*)arg);
	int str_len=0, i;
	char msg[BUF_SIZE];

	char query[QUERY_BUF_SIZE];
	struct AcInfomation *Na;
	
	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)				// 전송받은 구조체 저장
		send_msg();
	
	pthread_mutex_lock(&mutx);											// 쓰레드 동기화 시작

	Na = (struct AcInfomation*)msg;										// 받은 구조체 저장
	sprintf(query,"INSERT INTO USER_CAR VALUES ('%s','%s','%s','%s','%s','%s','%s')",Na->ID,Na->Name,Na->Phon,Na->Carinfor,Na->X,Na->Y,Na->Time);
	mysql_query(conn_ptr,query);										// DB에 데이터 입력 쿼리전송
	res = mysql_use_result(conn_ptr);							

	for(i=0; i<clnt_cnt; i++){											 // 제거된 클라이언트 제거
		if(clnt_sock==clnt_socks[i]){
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}

	clnt_cnt--;
	mysql_free_result(res);

	pthread_mutex_unlock(&mutx);

	close(clnt_sock);
	return NULL;
}

void send_msg(){														// 확인문자
	int i;
	char ACK[25] = {"DATA Transfer Sucess!"};

	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], ACK, strlen(ACK));
	pthread_mutex_unlock(&mutx);
}

void error_handling(char * msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
