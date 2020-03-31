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
                MutableInt v = new MutableInt();
                for (int i = 0; i < 10000; i++) {
                    if (i < 5000) {
                        this.q.queue(i);
                        System.out.println(Thread.currentThread().getName() + "- push():"+v.value);
                    }
                    else {

                        this.q.dequeue(v);
                        System.out.println(Thread.currentThread().getName() + "- pop():"+i);
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
            threads.get(i).join();
        }

    }
}
