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
            for (int i = 0; i < 2; i++) {
                if (i % 2== 0)
                    this.q.queue(k);
                else {
                    System.out.println("TET");
                    MutableInt v = new MutableInt();
                    this.q.dequeue(v);
                    System.out.println(v.value);
                }
            }
        }
    }

    private final static int N = 3;
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
