package shortestPathsFloyd_Warshall;

import java.util.concurrent.BrokenBarrierException;

public class MyThread extends Thread {
    private int startPoint;
    private int endPoint;
    private int[][] graph;
    private int N;

    MyThread(int startPoint, int endPoint, int[][] graph, int N) {
        this.startPoint = startPoint;
        this.endPoint = endPoint;
        this.graph = graph;
        this.N = N;
    }

    
    @Override
    public synchronized void run() {
        for (int i = startPoint; i < endPoint; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int k = 0; k < N; ++k) {
                    graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
                }
            }
            try {
                //Resincronizarea thread-urilor pentru urmatorul pas al algoritmului.
                Main.barrier.await();
            } catch (BrokenBarrierException | InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
