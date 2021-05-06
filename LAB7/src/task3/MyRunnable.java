package task3;

import java.io.File;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
	ExecutorService tpe;
    AtomicInteger inQueue;
    int step;
    int[] graph;

	public MyRunnable(ExecutorService tpe, AtomicInteger inQueue, int step, int[] graph) {
		this.tpe = tpe;
        this.inQueue = inQueue;
        this.step = step;
        this.graph = graph;
	}


    private static boolean check(int[] arr, int step) {
        for (int i = 0; i <= step; i++) {
            for (int j = i + 1; j <= step; j++) {
                if (arr[i] == arr[j] || arr[i] + i == arr[j] + j || arr[i] + j == arr[j] + i)
                    return false;
            }
        }
        return true;
    }

    private static void printQueens(int[] sol) {
        StringBuilder aux = new StringBuilder();
        for (int i = 0; i < sol.length; i++) {
            aux.append("(").append(sol[i] + 1).append(", ").append(i + 1).append("), ");
        }
        aux = new StringBuilder(aux.substring(0, aux.length() - 2));
        System.out.println("[" + aux + "]");
    }


	@Override
	public void run() {
        if (Main.N == step) {
            printQueens(graph);
            tpe.shutdown();
        }
        for (int i = 0; i < Main.N; ++i) {
            int[] newGraph = graph.clone();
            newGraph[step] = i;

            if (check(newGraph, step)) {
                inQueue.incrementAndGet();
                tpe.submit(new MyRunnable(tpe, inQueue, step + 1, newGraph));
            }
        }

        int left = inQueue.decrementAndGet();
		if (left == 0) {
			tpe.shutdown();
        }
        
	}
}
