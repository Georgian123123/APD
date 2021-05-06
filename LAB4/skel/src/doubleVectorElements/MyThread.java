package doubleVectorElements;

public class MyThread extends Thread{
    private int startPoint;
    private int endPoint;
    private int[] v;

    MyThread(int startPoint, int endPoint, int[] v) {
        this.startPoint = startPoint;
        this.endPoint = endPoint;
        this.v = v;
    }
    @Override
    public synchronized void run() {
        for (int i = startPoint; i < endPoint; ++i) {
            v[i] = v[i] * 2;
        }
    }
}
