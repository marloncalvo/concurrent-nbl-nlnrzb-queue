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
                for (int i = 0; i < 4; i++) {
                    if (i % 2== 0) {
                        this.q.queue(k);
                        System.out.println(Thread.currentThread().getName() + "- push():"+k);
                    }
                    else {
                        MutableInt v = new MutableInt();
                        this.q.dequeue(v);
                        System.out.println(Thread.currentThread().getName() + "- pop():"+v.value);
                    }
                }
            } catch (Exception e) {

            }
        }
    }

    private final static int N = 5;
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
