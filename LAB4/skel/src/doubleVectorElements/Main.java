package doubleVectorElements;
/**
 * @author cristian.chilipirea
 *
 */
public class Main {

	public static int min(int a, int b) {
		return (a < b)?  a : b;
	}

	public static void main(String[] args) {
		int N = 100000013;
		int v[] = new int[N];
		int threads = 3;

		for(int i=0;i<N;i++)
			v[i]=i;
		
		// Parallelize me
		for (int i = 0; i < threads; i++) {
			int startPoint = (int)(i * (double)N / threads);
			int endPoint = min((int)((i + 1) * (double)N / threads), N);
			MyThread thread = new MyThread(startPoint, endPoint, v);
			thread.run();
		}

		for (int i = 0; i < N; i++) {
			if(v[i]!= i*2) {
				System.out.println("Wrong answer");
				System.exit(1);
			}
		}
		System.out.println("Correct");
	}

}
