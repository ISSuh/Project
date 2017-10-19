#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "/usr/include/mysql/mysql.h"

#define PORT 9777

#define DBID "pi"
#define DBPW "1"
#define DBADR "localhost"
#define DBNAME "EZLock"

MYSQL *conn_ptr;
MYSQL_RES *res;
MYSQL_ROW row;

#define BUF_SIZE 45
#define QUERY_BUF_SIZE 1024
#define MAX_CLNT 256

void *handle_clnt(void * arg);							// 쓰레드 함수
void send_msg();										// 확인 메세지 전송함수
void error_handling(char * msg);						// 오류처리

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(){
	int serv_sock, clnt_sock;							// 자신이 서버일때
	struct sockaddr_in serv_adr, clnt_adr;
	int  clnt_adr_sz;
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

	while(1){																			// 클라이언트의 다중접속
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++]=clnt_sock;
		pthread_mutex_unlock(&mutx);
		printf("*************************************\n");
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_join(t_id,NULL);
		printf("DisConnected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
		printf("*************************************\n");
	}

	mysql_close(conn_ptr);
	close(serv_sock);
	return 0;
}
	
void * handle_clnt(void * arg){
	int clnt_sock=*((int*)arg);
	int i;
	char HD[2],ST[2],H_S[3];
	char Id[BUF_SIZE],Pw[BUF_SIZE],Pcnum[BUF_SIZE];
	char Error_msg[2];
	char query[QUERY_BUF_SIZE];
	char query_unlock[QUERY_BUF_SIZE];
	
	pthread_mutex_lock(&mutx);	

	memset(H_S,NULL,sizeof(H_S));
	memset(query,NULL,sizeof(query));
	memset(query_unlock,NULL,sizeof(query_unlock));

	read(clnt_sock, H_S, sizeof(H_S));
	HD[0] = H_S[0];
	printf("head : %d\n",atoi(HD));
	
	switch(atoi(HD)){
		case 1:
			ST[0] = H_S[1];
			printf("state : %d\n",atoi(ST));

			sprintf(query_unlock,"UPDATE `EZLock`.`PC` SET `State`='1' WHERE `idPC`='0'");
			mysql_query(conn_ptr,query_unlock);
			res = mysql_use_result(conn_ptr);
			mysql_free_result(res);
			
			break;

		case 2:
			memset(Pcnum,NULL,sizeof(Pcnum));
			memset(Error_msg,NULL,sizeof(Error_msg));

			read(clnt_sock, Pcnum, sizeof(Pcnum));
			printf("Pcnum : %d\n",atoi(Pcnum));

			sprintf(query,"select State from PC where idPC = '%d'",atoi(Pcnum));

			if(mysql_query(conn_ptr,query)){																						// DB에 데이터 입력 쿼리전송
				printf("Query Error!\n");
				break;
			}

			else{
				if(res= mysql_store_result(conn_ptr)){
					if(row = mysql_fetch_row(res)){
						
						printf("state : %s\n",row[0]);

						Error_msg[0] = '0';
						write(clnt_sock,Error_msg, sizeof(Error_msg));

						switch(atoi(row[0])){
							case 0:																									// 언락인 상황일때
								break;
							case 1:																									// 락인 상황일때
								/*printf("*************************************\n");
								Connect_pc();																						// 언락시킴
								printf("*************************************\n");*/

								sprintf(query_unlock,"UPDATE `EZLock`.`PC` SET `State`='0' WHERE `idPC`='%d'",atoi(Pcnum));
								mysql_query(conn_ptr,query_unlock);																	// DB에 데이터 입력 쿼리전송
								res = mysql_use_result(conn_ptr);									
								mysql_free_result(res);
								break;
						}
					}
					else{
						printf("Error : Invaild PC NUM!\n");
						Error_msg[0] = '1';
						write(clnt_sock,Error_msg, sizeof(Error_msg));
					}
				}
				else{
					printf("NOT Row!\n");
				}
			}
			break;
	}
	
	for(i=0; i<clnt_cnt; i++){											 // 제거된 클라이언트 제거
		if(clnt_sock==clnt_socks[i]){
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;

	pthread_mutex_unlock(&mutx);

	close(clnt_sock);
	return NULL;
}

void Connect_pc(void){
	int s;
	struct sockaddr_in c_serv_adr;						//자신이 클라리언트일때
	char buf[BUF_SIZE];

	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
                printf("can't create socket\n");
                exit(0);
     }

     c_serv_adr.sin_family = AF_INET;
     c_serv_adr.sin_addr.s_addr = inet_addr("192.168.0.5");
     c_serv_adr.sin_port = htons(9988);
	  
	 if(connect(s, (struct sockaddr *)&c_serv_adr, sizeof(c_serv_adr)) < 0){      
		 printf("can't connect.\n");
         exit(0);
     }
	 
	 printf("PC Connect!\n");

	 write(s, "0",2);
	 printf("send to pc '0' \n");

	 close(s);
}

void send_msg(char *ACK){														// 확인문자
	int i;

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
