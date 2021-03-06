package bug3;
/**
 * @author cristian.chilipirea
 * 
 *         Why is value set correct even though we use different locks in
 *         different threads?
 */
public class MyThread implements Runnable {
	static String a = "LOCK";
	static String b = "LOCK";
	int id;
	static int value = 0;

	MyThread(int id) {
		this.id = id;
	}

	@Override
	public synchronized void run() {
		if (id == 0) {
				for (int i = 0; i < Main.N; i++)
					value = value + 3;
		} else {
				for (int i = 0; i < Main.N; i++)
					value = value + 3;
		}
	}
}
