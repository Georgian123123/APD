public class Task extends Thread {
 
    private int id;
 
    public Task(int id) {
        this.id = id;
    }
 
    public void run() {
        System.out.println("Hello from thread " + id);
    }
}