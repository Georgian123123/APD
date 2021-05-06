package synchronizedSortedList;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.Semaphore;

public class Sort extends Thread {
    private final List<Integer> list;
    public Sort(List<Integer> list) {
        super();
        this.list = Collections.synchronizedList(new ArrayList<Integer>());
    }

    @Override
    public void run() {
        try {
            Main.semaphore.acquire();
        } catch (Exception e) {
            e.printStackTrace();
        }
        Collections.sort(list);
        Main.semaphore.release();
    }
}
