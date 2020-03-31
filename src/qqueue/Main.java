package qqueue;

import java.util.ArrayList;

public class Main {

    static class Thread extends java.lang.Thread {
        QQueue q;
        int k;
        Thread(QQueue q, int k) {
            this.q =q;
            this.k = k;
        }

        @Override
        public void run() {
            try {
                for (int i = 0; i < 100000; i++) {
                    if (i < 90000) {
                        this.q.queue(i);
                        //System.out.println(Thread.currentThread().getName() + "- push():"+i);
                    }
                    else if(i == 90000) {
                        System.out.println(Thread.currentThread().getName() + " - Done Queueing");
                    }
                    else {
                        MutableInt v = new MutableInt();
                        this.q.dequeue(v);
                        //System.out.println(Thread.currentThread().getName() + "- pop():"+i);
                    }
                }
            } catch (Exception e) {

            }
        }
    }

    private final static int N = 32;
    public static void main(String [] args) throws Exception {
        QQueue q = new QQueue();
        ArrayList<Thread> threads = new ArrayList<>();
        for (int i = 0; i < N; i++) {
            threads.add(new Thread(q, i+1));
        }
        for (int i = 0; i < N; i++) {
            threads.get(i).start();
        }
        for (int i = 0; i < N; i++) {
            System.out.println("Waiting for " + threads.get(i).getName());
            threads.get(i).join();
            System.out.println("Done -> " + threads.get(i).getName());
        }

        MutableInt v = new MutableInt();
        q.dequeue(v);
        System.out.print(v.value);
    }
}
