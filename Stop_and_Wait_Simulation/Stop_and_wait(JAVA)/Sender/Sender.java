import java.io.*;
import java.net.*;
import java.util.Random;

public class Sender {
	Socket sender; // 클라이언트를 담당하는 Sender소켓 생성
	ObjectOutputStream out; // 객체 출력 스트림
	ObjectInputStream in; // 객체 입력 스트림
	String packet, ack, str, msg;  //패킷,Ack,데이터,msg 를 저장할 변수
	int n, i = 0, seq = 0; // 순서번호 초기화
	Random rand = new Random();

	public void run() {
		try {
			BufferedReader buffer = new BufferedReader(new InputStreamReader(System.in));  // 버퍼입력
			
			System.out.println("SENDER");
			
			sender = new Socket("localhost", 1); // 소켓 ip및 포트번호 설정
			
			System.out.println("-------------------");
			System.out.println("Cenecting");

			out = new ObjectOutputStream(sender.getOutputStream()); // 객체 출력스트림에 받아온 데이터를 읽음
			out.flush();
			in = new ObjectInputStream(sender.getInputStream()); // 객체 입력 스트림에 받은 데이터를 입력
			str = (String) in.readObject();
		
			System.out.println("connect!");
			System.out.println("-------------------");
			System.out.println("Write Data : ");
			packet = buffer.readLine();  // 키보드로 입력벋은 전송할 데이터를 packet에 저장
			n = packet.length();
			
			System.out.println("\nSample > Frame : [Sequence|Data]  Ack : Ack[NUM]");
			
			do {
				try {
					if (i < n) {
						msg = String.valueOf(seq); // 전송할 퍄캇중 순서번호를 뺀 실제 데이터값 
						msg = msg.concat(packet.substring( i , i + 1));
					} 
					
					else if (i == n) { // 실제 데이터값 마지막에 끝을 알리는 'end'를 저장
						msg = "end";
						out.writeObject(msg);
						break;
					}
					
					
					System.out.println("\nSend Frame : ["  + msg.substring(0, 1)  + "|" +  msg.substring(1, 2) + "]");
					
					if(rand.nextInt(10) < 5) // 20%의 확률로 오류를 발생시킴 (중간에 Frame를 잃어버리는 경우)
						out.writeObject(msg);
					
					else{
						System.out.println("Time Out! ReSending! [Frame loss]");
						continue;
					}
					
					seq = (seq == 0) ? 1 : 0;
					seq = seq % 2; // 순서번호를 모듈러 2연산하여 설정
					out.flush();
								
					ack = (String) in.readObject(); // Receiver에게 Ack를 받음
					
					if (ack.equals(String.valueOf(seq))) { // 받은 Ack와 현재 Seq값이 같다면 정상적인 통신
						i++;
						System.out.println("Received Ack[" + ack + "]");
					} 
										
					else { // 아니라면 Ack를 잃어버렸다고 가정하여 오류로 봄
						System.out.println("Time Out! ReSending! [Ack loss]");
						seq = (seq == 0) ? 1 : 0;
						seq = seq % 2;
					}
				} catch (Exception e) {
				}
			} while (i < n + 1); // 보낸 데이터의 길이의 +1 만큼 반복한다
			System.out.println("Compelete! Connection ended    .\n");
		} catch (Exception e) {
		} finally {
			try {
				in.close();
				out.close(); // 버퍼 닫기
				sender.close(); // 소켓 종료
			} catch (Exception e) {
			}
		}
	}

	public static void main(String args[]) {
		Sender s = new Sender(); // Sender 쿨래스 생성
		s.run(); // 동작하는 함수 run()
	}
}
