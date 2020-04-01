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
                final int max   = 100000;
                final int split = (int)(max * 0.51);
                for (int i = 0; i < max; i++) {
                    if (i > split) {
                        this.q.queue(i);
                        //System.out.println(Thread.currentThread().getName() + "- push():"+i);
                    }
                    else if (i == split) {
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

    private final static int N = 16;
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
    }
}
