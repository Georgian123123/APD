package oneProducerOneConsumer;
/**
 * @author cristian.chilipirea
 *
 */
public class Buffer {
	int a;
	int size = 0;
	synchronized void put(int value) {
		while (size == 0) 
		{
				try {
					wait();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			

			a = value;
			size = size + 1;

			if (size == 1)
			{
				notifyAll();
			}	
		}
	}

	synchronized int get() {
		while (size != 0) 
        {
            try {
				wait();
			} catch (InterruptedException e) {
				e.printStackTrace();
			} 

			size = size - 1;

			if (size == 0)
			{
				notifyAll();
			}

			return a;
		}
		return -1;
	}
}
