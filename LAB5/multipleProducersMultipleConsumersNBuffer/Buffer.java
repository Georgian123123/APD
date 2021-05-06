package multipleProducersMultipleConsumersNBuffer;

import java.util.Queue;

/**
 * @author Gabriel Gutu <gabriel.gutu at upb.ro>
 *
 */
public class Buffer {
    
    Queue queue;
    int maxSize;
    
    public Buffer(int size) {
        queue = new LimitedQueue(size);
        int maxSize = size;
    }

	void put(int value) {
        if (queue.size() == maxSize) {
            queue.add(value);
        }        
	}

	int get() {
        if (!queue.isEmpty()) {
            return (int)queue.poll();
        }
        return -1;
	}
}
