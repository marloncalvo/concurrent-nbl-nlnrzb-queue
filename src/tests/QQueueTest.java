package tests;

import qqueue.MutableInt;
import qqueue.QQueue;

import java.io.File;
import java.io.PrintWriter;
import java.util.ArrayDeque;

enum Op {Push, Pop};

public class QQueueTest {

    static class Pair<T, U> {
        private final T head;
        private final U tail;
        public Pair(T head, U tail) {
            this.head = head;
            this.tail = tail;
        }
        public T getHead() {
            return head;
        }

        public U getTail() {
            return tail;
        }
    }

    static class Benchmark {

        static class Test {
            private final Op op;
            private final int value;
            private final MutableInt adr;
            public Test(int value) {
                this.op = Op.Push;
                this.adr = null;
                this.value = value;
            }
            public Test(MutableInt adr) {
                this.op = Op.Pop;
                this.adr = adr;
                this.value = -111;
            }
        }

        private final ArrayDeque<Test> tests;

        public Benchmark() {
            tests = new ArrayDeque<>();
        }

        public void addTest(Test test) {
            tests.add(test);
        }

        public Test popTest() {
            return tests.pop();
        }

        public boolean isEmpty() {
            return tests.isEmpty();
        }

        public void populateRandom(int numOps) {
            for (int i = 0;i < numOps; i++) {
                if (Math.random() > 0.3) {
                    addTest(new Test((int)Math.random() * 1000000 + 2));
                } else {
                    addTest(new Test(new MutableInt()));
                }
            }
        }
    }

    static class Thread extends java.lang.Thread {
        private final QQueue q;
        private final Benchmark benchmark;

        public Thread(QQueue q, Benchmark benchmark) {
            this.q = q;
            this.benchmark = benchmark;
        }

        @Override
        public void run() {
            while (!benchmark.isEmpty()) {
                Benchmark.Test test = benchmark.popTest();
                if (test.adr == null) {
                    q.queue(test.value);
                } else {
                    q.dequeue(test.adr);
                }
            }
        }
    }

    public static Pair<Double, Double> performTest(int numThreads, int numOps) throws Exception {
        QQueue q = new QQueue();
        Thread[] threads = new Thread[numThreads];
        for (int i = 0; i < numThreads; i++) {
            Benchmark benchmark = new Benchmark();
            benchmark.populateRandom(numOps);
            threads[i] = new Thread(q, benchmark);
        }

        long start = System.nanoTime();
        for (int i = 0; i < numThreads;i ++) {
            threads[i].start();
        }

        for (int i = 0; i < numThreads;i ++) {
            threads[i].join();
        }
        long end = System.nanoTime();

        double totalTime = (end-start)/1e9;
        double opPerSec  = (numOps*numThreads)/totalTime;

        // Attempt to gc.
        q = null;
        threads = null;

        System.gc();

        return new Pair<>(totalTime, opPerSec);
    }

    public static final int MAX_N_THREADS = 32;
    public static final int N_OPS = 1000;

    public static void main(String [] args) throws Exception {
        PrintWriter writer = new PrintWriter(new File("qqueue_results.csv"));
        writer.println("n_threads, n_ops, total_time, op/s");

        for (int i = 1; i <= MAX_N_THREADS; i++) {
            Pair<Double, Double> results = performTest(i, N_OPS);
            System.out.println(String.format("Results for %d threads:\n\tTotal Time: %f\n\tOp/s: %f\n\t", i, results.getHead(), results.getTail()));

            writer.println(String.format("%d,%d,%.6f,%.2f", i, N_OPS, results.getHead(), results.getTail()));
        }

        writer.close();
    }
}
