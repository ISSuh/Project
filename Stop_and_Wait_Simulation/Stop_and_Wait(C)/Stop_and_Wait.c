#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define PACKET_NUM 5 // 프레임 길이 설정

typedef struct {
	int data;
}packet; // 패킷구조체 생성
typedef struct {
	int seq; // Sender에서의 순서번호(seq)
	int ack; // Receiver에서의 ack
	packet info; // 프레임내의 데이터
	int err; // 에러 발생을 보이기 위한 에러
}frame; // 프레임 구조체 생성

typedef enum{ ARRIVAL, ERR, TIMEOUT } event_type; // 프레임도착, Time out, 에러 이벤트 정의 

void FromNetworkLayer(packet *); // 네트워크층에서 데이터를 받음
void ToPhysicalLayer(frame *); // 프레임구성후, 물리층으로 전달
void FromPhysicalLayer(frame *); // 전송받은 데이터를 물리층에서 데이터링크층으로 전달
void ToNetworkLayer(packet *); // 프레임을 네트워크층으로 전달
void WaitSender(event_type *); // Sender에서의 Timer,에러,도착을 확인
void WaitReciever(event_type *); // Receiver에서의 도착및 에러 확인
void sender(); // Sender 정의
void reciever(); // Receiver 정의
int Random(int n); // 랜덤으로 오류를 발생하기위한 함수
int MakeSeq(packet *, int*); // Sender에서의 순서번호(seq)를 만들기위한 함수

int i = 0; // 전송하는 패킷의 번호
char turn = 's'; // Sender,Receiver의 순서 
int Finish = 0; // 종료상태
frame DATA; // 전송되는 데이터

void main(){
	srand((unsigned)time(NULL));

	printf("Sample  > SENDER : Data[] Seq[]        RECEIVER : Data[] Ack[] \n\n");

	while (!Finish){
		sender();
		Sleep(300);
		reciever();
	}
	_getch();
}

void sender(){
	static int seq = 0; // 순서번호 
	static frame s; // Sender에서의 프레이밍 
	packet buffer; // 네트워크층에서 받은 패킷
	event_type event; // 위에서 정의한 3가지 이벤트
	static int flag = 0;

	if (flag == 0){ // 최초의 데이터 전송 한번 수행
		FromNetworkLayer(&buffer); // 네트워크층에서 패킷을 받음
		s.info = buffer; // 받은 데이터를 저장
		s.seq = seq; // 순서번호
		printf("SENDER :  Data[%d]   Seq[%d]        ", s.info, s.seq);
		turn = 'r'; // Receiver의 차례로 넘어가게 함
		ToPhysicalLayer(&s); // 물리층으로 데이터를 넘김
		flag = 1;
	}

	WaitSender(&event); // 어떤 이벤트가 발생하는지 정의

	if (turn == 's'){ // Sender의 차례
		if (event == ARRIVAL){ // 프레임이 도착. 이벤트 ARRIVAL발생
			FromNetworkLayer(&buffer); // 네트워크층에서 패킷을 받음
			MakeSeq(&buffer, &seq); // 받은 패킷을 이용하여 순서번호(seq)를 생성
			s.info = buffer; // 받은 데이터를 저장
			s.seq = seq; // 순서번호(seq) 저장
			printf("SENDER :  Data[%d]   Seq[%d]	  ", s.info, s.seq);
			turn = 'r'; // Receiver의 차례로 넘어가게 함
			ToPhysicalLayer(&s); // 물리층으로 데이터를 넘김
		}

		else if (event == TIMEOUT){ // 이벤트 Timeout 발생
			printf("SENDER : Time Out! Resending Frame");
			turn = 'r'; // Receiver의 차례로 넘어가게 함
			ToPhysicalLayer(&s); // 물리층으로 다시 데이터를 넘김
		}
	}
}

void reciever(){
	static int ack = 1; // Receiver에서 보내는 ack
	frame r, s; // 받은 프레임, 다시 보낼 프레임 생성
	event_type event; // 이벤트

	WaitReciever(&event); // 어떤이벤트가 발생하는지 정의

	if (turn == 'r'){ // Receiver의 차례
		if (event == ARRIVAL){ // 프레임이 도착. 이벤트 ARRIVAL 발생
			FromPhysicalLayer(&r); // 물리층에서 데이터를 받음

			if (r.seq + 1 == ack || r.seq - 1 == ack){ // 받은 패킷의 순서번호가 전에 보낸 ack와 같은지 판단
				printf("   RECIEVER : Data[%d] Received Ack[%d] Send\n", r.info, ack);
				ToNetworkLayer(&r.info); // 네트워크층으로 데이터를 전송

				if (ack < 1)
					ack++;
				else
					ack = 0; // ack가 0또는 1이 되도록 함.
			}

			else
				printf("   RECIEVER : Ack Resent(Discard Frame)\n");

			turn = 's'; // Sender 차례로 바꿈
			ToPhysicalLayer(&s); // ack를 전송하기위해 물리층으로 데이터를 보냄
		}

		if (event == ERR){ // 에러가 발생했을때
			printf("   RECIEVER : Frame Loss!\n");
			turn = 's'; // 프레임이 중간에 잃어버렸다면 차례를 Sender로 바꿔줘서 다시 보내도록 함
		}
	}
}



void FromNetworkLayer(packet *buffer){ // 네트워크층에서 데이터링크층으로 패킷을 주기위해 데이터를 생성
	(*buffer).data = i; // 데이터는 1,2,3...n
	i++;
}

void ToPhysicalLayer(frame *s){ // 데이터링크층에서 물리층으로 데이터를 보냄
	s->err = Random(5); // 20%의 확룰로 에러가 생성된다. 0일때 에러라고 가정함
	DATA = *s;
}

void FromPhysicalLayer(frame *buffer){ // Receiver의 물리층에서 데이터를 전송받음
	*buffer = DATA;
}

void ToNetworkLayer(packet *buffer){ // Receiver의 데이터링크층에서 네트워크층으로 데이터를 보냄
	if (i == PACKET_NUM){ // 전송받은 i가 프레임의 길이만큼이면 전송 완료이므로 종료
		Finish = 1;
		printf("\nDISCONNECTED");
	}
}

void WaitSender(event_type * e){ // Sender측의 이벤트 정의
	static int timer = 0; // 타이머 생성

	if (turn == 's'){ // Sender의 차례일때
		timer++; // 타이머를 증가 시킴

		if (timer == 5){ // 타이머가 증가될동안 ack가 오지않는다면 이벤트를 TIMEOUT으로 설정
			*e = TIMEOUT;
			timer = 0;
			return;
		}

		if (DATA.err == 0) // 데이터전송에 에러가 있다면 이벤트를 ERR로 설정
			*e = ERR;
		else{ // 정상적으로 프레임이 도착했다면 ARRIVAL으로 설정
			timer = 0;
			*e = ARRIVAL;
		}
	}
}

void WaitReciever(event_type * e){ // Receiver에서의 이벤트 설정
	if (turn == 'r'){ // Receiver차례일때
		if (DATA.err == 0) // 전송중 에러일때
			*e = ERR; // 이벤트를 ERR로 설정
		else // 아닐경우 정상도착이므로 ARRIVAL로 설정
			*e = ARRIVAL;
	}
}

int Random(int n){
	return rand() % n; // 오류를 랜덤하게 생성
}
int MakeSeq(packet *buffer, int *seq){
	*seq = buffer->data % 2; // 보내는 데이터(여기서는 데이터값 ex)1,2,3...n)를 이용해 모듈러 2연산하여 순서번호(seq)를 생성함
	return *seq;
}
