import java.io.*;
import java.net.*;
import java.util.Random;

public class Reciever {
	ServerSocket reciever; // 서버를 담당하는 Receiver소켓 생성
	Socket connection = null;
	ObjectOutputStream out; // 객체출력스트림
	ObjectInputStream in; // 객체입력스트림
	String packet, ack, data = ""; //패킷,Ack,데이터를 저장할 변수
	int i = 0, Ack = 0; // Ack 초기화
	Random rand = new Random(); // 랜덤하게 오류를 발생시키기 위해 선언

	public void run() { // 패킷을 받고 Ack를 전송하는 함수 run 함수
		try {
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));  // 버퍼입력

			System.out.println("RECEIVER");
			
			reciever = new ServerSocket(1, 10); // 서버 설정
			
			System.out.println("-------------------");
			System.out.println("Cenecting");
			
			connection = reciever.accept(); // Sender와 연결

			
			System.out.println("Cennect!");
			System.out.println("------------------");
			
			out = new ObjectOutputStream(connection.getOutputStream()); // 객체 출력스트림에 받아온 데이터를 읽음
			out.flush(); // 현재 버퍼에 저장되어 있는 내용을 클라이언트로 전송하고 버퍼를 비움
			in = new ObjectInputStream(connection.getInputStream()); // 객체 입력 스트림에 받은 데이터를 입력
			out.writeObject("Connected    .");
			
			System.out.println("\nSample > Frame : [Sequence|Data]  Ack : Ack[NUM]\n");
			
			do {
				try {
					//전송받은 패킷을 일고 확인하는 부분
					packet = (String) in.readObject(); // 전송받은 패킷을 읽는다
					if (Integer.valueOf(packet.substring(0, 1)) == Ack) { // 현재 Ack설정값과 Sender에서 보낸 순서번호가 같다면 
						data += packet.substring(1); // 순서번호와 데이터가 합쳐진 packet에서 데이터만 추출함
						Ack = (Ack == 0) ? 1 : 0; // Ack를 1또는 0으로 설정
						
						System.out.println("\nReceived Frame : ["  + packet.substring(0, 1)  + "|" +  packet.substring(1, 2) + "]");
					} 
					
					else  // 전과 같은 패킷을 받았다면 중복이므로 버림
						System.out.println("Received Frame : ["  + packet.substring(0, 1)  + "|" +  packet.substring(1, 2) + "]" + " Duplation! Discard this Frame!");
					
					
					//Ack를 전송하는 부분
					if (rand.nextInt(10) < 5) { // 20%의 확률로 오류를 발생시킴 (중간에 Ack를 잃어버리는 경우)
						out.writeObject(String.valueOf(Ack));
						System.out.println("Send Ack : Ack[" + Ack + "]");
					} 
					
					else {
						out.writeObject(String.valueOf((Ack + 1) % 2)); // 오류가 아닌 정상적으로 Ack를 전송, +1을하고 모듈러 2연산을 하여 다음에 필요한 패킷의 순서번호를 가지는 Ack를 전송
						System.out.println("Send Ack : Ack[" + Ack + "]");
					}
				} catch (Exception e) {
				}
			}while (!packet.equals("end")); // 패킷의 마지막'end'가 나올때까지 반복함
			
			System.out.println("\nReceived Date  : " + data);
			System.out.println("Compelete! Connection ended    .\n");
			
		} catch (Exception e) {
		}finally {
			try {
				in.close();
				out.close(); // 버퍼 닫기
				reciever.close(); // 소켓 종료
			} catch (Exception e) {
			}
		}
	}

	public static void main(String args[]) {
		Reciever r = new Reciever(); // Reciever클래스 생성
		r.run(); // 동작하는 함수 run()
	}
}