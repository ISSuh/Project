import java.io.*;
import java.net.*;
import java.util.Random;

public class Reciever {
	ServerSocket reciever; // ������ ����ϴ� Receiver���� ����
	Socket connection = null;
	ObjectOutputStream out; // ��ü��½�Ʈ��
	ObjectInputStream in; // ��ü�Է½�Ʈ��
	String packet, ack, data = ""; //��Ŷ,Ack,�����͸� ������ ����
	int i = 0, Ack = 0; // Ack �ʱ�ȭ
	Random rand = new Random(); // �����ϰ� ������ �߻���Ű�� ���� ����

	public void run() { // ��Ŷ�� �ް� Ack�� �����ϴ� �Լ� run �Լ�
		try {
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));  // �����Է�

			System.out.println("RECEIVER");
			
			reciever = new ServerSocket(1, 10); // ���� ����
			
			System.out.println("-------------------");
			System.out.println("Cenecting");
			
			connection = reciever.accept(); // Sender�� ����

			
			System.out.println("Cennect!");
			System.out.println("------------------");
			
			out = new ObjectOutputStream(connection.getOutputStream()); // ��ü ��½�Ʈ���� �޾ƿ� �����͸� ����
			out.flush(); // ���� ���ۿ� ����Ǿ� �ִ� ������ Ŭ���̾�Ʈ�� �����ϰ� ���۸� ���
			in = new ObjectInputStream(connection.getInputStream()); // ��ü �Է� ��Ʈ���� ���� �����͸� �Է�
			out.writeObject("Connected    .");
			
			System.out.println("\nSample > Frame : [Sequence|Data]  Ack : Ack[NUM]\n");
			
			do {
				try {
					//���۹��� ��Ŷ�� �ϰ� Ȯ���ϴ� �κ�
					packet = (String) in.readObject(); // ���۹��� ��Ŷ�� �д´�
					if (Integer.valueOf(packet.substring(0, 1)) == Ack) { // ���� Ack�������� Sender���� ���� ������ȣ�� ���ٸ� 
						data += packet.substring(1); // ������ȣ�� �����Ͱ� ������ packet���� �����͸� ������
						Ack = (Ack == 0) ? 1 : 0; // Ack�� 1�Ǵ� 0���� ����
						
						System.out.println("\nReceived Frame : ["  + packet.substring(0, 1)  + "|" +  packet.substring(1, 2) + "]");
					} 
					
					else  // ���� ���� ��Ŷ�� �޾Ҵٸ� �ߺ��̹Ƿ� ����
						System.out.println("Received Frame : ["  + packet.substring(0, 1)  + "|" +  packet.substring(1, 2) + "]" + " Duplation! Discard this Frame!");
					
					
					//Ack�� �����ϴ� �κ�
					if (rand.nextInt(10) < 5) { // 20%�� Ȯ���� ������ �߻���Ŵ (�߰��� Ack�� �Ҿ������ ���)
						out.writeObject(String.valueOf(Ack));
						System.out.println("Send Ack : Ack[" + Ack + "]");
					} 
					
					else {
						out.writeObject(String.valueOf((Ack + 1) % 2)); // ������ �ƴ� ���������� Ack�� ����, +1���ϰ� ��ⷯ 2������ �Ͽ� ������ �ʿ��� ��Ŷ�� ������ȣ�� ������ Ack�� ����
						System.out.println("Send Ack : Ack[" + Ack + "]");
					}
				} catch (Exception e) {
				}
			}while (!packet.equals("end")); // ��Ŷ�� ������'end'�� ���ö����� �ݺ���
			
			System.out.println("\nReceived Date  : " + data);
			System.out.println("Compelete! Connection ended    .\n");
			
		} catch (Exception e) {
		}finally {
			try {
				in.close();
				out.close(); // ���� �ݱ�
				reciever.close(); // ���� ����
			} catch (Exception e) {
			}
		}
	}

	public static void main(String args[]) {
		Reciever r = new Reciever(); // RecieverŬ���� ����
		r.run(); // �����ϴ� �Լ� run()
	}
}