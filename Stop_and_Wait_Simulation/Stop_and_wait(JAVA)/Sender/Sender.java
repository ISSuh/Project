import java.io.*;
import java.net.*;
import java.util.Random;

public class Sender {
	Socket sender; // Ŭ���̾�Ʈ�� ����ϴ� Sender���� ����
	ObjectOutputStream out; // ��ü ��� ��Ʈ��
	ObjectInputStream in; // ��ü �Է� ��Ʈ��
	String packet, ack, str, msg;  //��Ŷ,Ack,������,msg �� ������ ����
	int n, i = 0, seq = 0; // ������ȣ �ʱ�ȭ
	Random rand = new Random();

	public void run() {
		try {
			BufferedReader buffer = new BufferedReader(new InputStreamReader(System.in));  // �����Է�
			
			System.out.println("SENDER");
			
			sender = new Socket("localhost", 1); // ���� ip�� ��Ʈ��ȣ ����
			
			System.out.println("-------------------");
			System.out.println("Cenecting");

			out = new ObjectOutputStream(sender.getOutputStream()); // ��ü ��½�Ʈ���� �޾ƿ� �����͸� ����
			out.flush();
			in = new ObjectInputStream(sender.getInputStream()); // ��ü �Է� ��Ʈ���� ���� �����͸� �Է�
			str = (String) in.readObject();
		
			System.out.println("connect!");
			System.out.println("-------------------");
			System.out.println("Write Data : ");
			packet = buffer.readLine();  // Ű����� �Է¹��� ������ �����͸� packet�� ����
			n = packet.length();
			
			System.out.println("\nSample > Frame : [Sequence|Data]  Ack : Ack[NUM]");
			
			do {
				try {
					if (i < n) {
						msg = String.valueOf(seq); // ������ ��ı�� ������ȣ�� �� ���� �����Ͱ� 
						msg = msg.concat(packet.substring( i , i + 1));
					} 
					
					else if (i == n) { // ���� �����Ͱ� �������� ���� �˸��� 'end'�� ����
						msg = "end";
						out.writeObject(msg);
						break;
					}
					
					
					System.out.println("\nSend Frame : ["  + msg.substring(0, 1)  + "|" +  msg.substring(1, 2) + "]");
					
					if(rand.nextInt(10) < 5) // 20%�� Ȯ���� ������ �߻���Ŵ (�߰��� Frame�� �Ҿ������ ���)
						out.writeObject(msg);
					
					else{
						System.out.println("Time Out! ReSending! [Frame loss]");
						continue;
					}
					
					seq = (seq == 0) ? 1 : 0;
					seq = seq % 2; // ������ȣ�� ��ⷯ 2�����Ͽ� ����
					out.flush();
								
					ack = (String) in.readObject(); // Receiver���� Ack�� ����
					
					if (ack.equals(String.valueOf(seq))) { // ���� Ack�� ���� Seq���� ���ٸ� �������� ���
						i++;
						System.out.println("Received Ack[" + ack + "]");
					} 
										
					else { // �ƴ϶�� Ack�� �Ҿ���ȴٰ� �����Ͽ� ������ ��
						System.out.println("Time Out! ReSending! [Ack loss]");
						seq = (seq == 0) ? 1 : 0;
						seq = seq % 2;
					}
				} catch (Exception e) {
				}
			} while (i < n + 1); // ���� �������� ������ +1 ��ŭ �ݺ��Ѵ�
			System.out.println("Compelete! Connection ended    .\n");
		} catch (Exception e) {
		} finally {
			try {
				in.close();
				out.close(); // ���� �ݱ�
				sender.close(); // ���� ����
			} catch (Exception e) {
			}
		}
	}

	public static void main(String args[]) {
		Sender s = new Sender(); // Sender �𷡽� ����
		s.run(); // �����ϴ� �Լ� run()
	}
}
