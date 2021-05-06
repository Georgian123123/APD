package shortestPathsFloyd_Warshall;

import java.util.concurrent.CyclicBarrier;

/**
 * @author cristian.chilipirea
 *
 */
public class Main {

	public static CyclicBarrier barrier;
	
	public static int min(int a, int b) {
		return (a < b)?  a : b;
	}

	public static void main(String[] args) {
		int M = 9;
		int graph[][] = { { 0, 1, M, M, M }, 
				          { 1, 0, 1, M, M }, 
				          { M, 1, 0, 1, 1 }, 
				          { M, M, 1, 0, M },
				          { M, M, 1, M, 0 } };
		int threads = 3;
		int N  = 5;
		// Parallelize me (You might want to keep the original code in order to compare)

		for (int i = 0; i < 3; ++i) {
			int startPoint = (int)(i * (double)N / threads);
			int endPoint = min((int)((i + 1) * (double)N / threads), N);
			MyThread thread = new MyThread(startPoint, endPoint, graph, N);
			thread.run();
		}

		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				System.out.print(graph[i][j] + " ");
			}
			System.out.println();
		}
	}
}
