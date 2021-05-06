package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingDeque;
import java.util.Queue;
public class Buffer {
	private ArrayBlockingQueue<Integer> queue;
	private int n = 100;
	Buffer() {
		queue = new ArrayBlockingQueue<>(n);
	}
	
	void put(int value) {
		try {
			queue.put(value);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	int get() {
		try {
			return queue.take();
		} catch (Exception e) {
			e.printStackTrace();
			return -1;
		}
	}
}
