package task2;

import java.io.File;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
	ExecutorService tpe;
    AtomicInteger inQueue;
    int step;
    int[] colors;

	public MyRunnable(ExecutorService tpe, AtomicInteger inQueue, int step, int[] colors) {
		this.tpe = tpe;
        this.inQueue = inQueue;
        this.step = step;
        this.colors = colors;
	}


    private static boolean verifyColors(int[] colors, int step) {
        for (int i = 0; i < step; i++) {
            if (colors[i] == colors[step] && isEdge(i, step))
                return false;
        }
        return true;
    }

    private static boolean isEdge(int a, int b) {
        for (int[] ints : Main.graph) {
            if (ints[0] == a && ints[1] == b)
                return true;
        }
        return false;
    }

    static void printColors(int[] colors) {
        StringBuilder aux = new StringBuilder();
        for (int color : colors) {
            aux.append(color).append(" ");
        }
        System.out.println(aux);
    }

	@Override
	public void run() {
        if (step == Main.N) {
            printColors(colors);
            tpe.shutdown();
            return;
        }

        // for the node at position step try all possible colors
        for (int i = 0; i < Main.COLORS; i++) {
            int[] newColors = colors.clone();
            newColors[step] = i;
            if (verifyColors(newColors, step)) {
                inQueue.incrementAndGet();
                tpe.submit(new MyRunnable(tpe, inQueue, step + 1, newColors));
            }
        }

        int left = inQueue.decrementAndGet();
		if (left == 0) {
			tpe.shutdown();
        }
        
	}
}
