package qqueue;

import java.util.ArrayList;
import java.util.Random;

public class Benchmark {
    private final static int NUM_OPS = 10000;

    static class Thread extends java.lang.Thread {
        QQueue q;
        int k;
        Thread( QQueue q, int k ){
            this.q = q;
            this.k = k;
        }

        @Override
        public void run() {
            Random rand = new Random();

            try {
                for( int i = 0; i < NUM_OPS; i++ ){
                    
                    int val = rand.nextInt(10);
                    //50-50 split
                    if( val >= 5  ){
                    //30-70 split
                    //if( val >= 3 ){
                    //70-30 split
                    //if( val >= 7 ){
                        this.q.queue(k);
                        //System.out.println(Thread.currentThread().getName() + "- push():"+k);
                    } else {
                        MutableInt v = new MutableInt();
                        this.q.dequeue(v);
                        //System.out.println(Thread.currentThread().getName() + "- pop():"+v.value);
                    }
                }
            } catch ( Exception e ) {

            }
        }
    }

    public static void main( String[] args ) throws Exception {
        QQueue q = new QQueue();
        System.out.println("Each thread performs " + NUM_OPS + " operations" );
        for( int i = 1; i <= 32; i++ ){
            long total_duration = 0;
            for( int k = 0; k < 5; k++ ){
                ArrayList<Thread> threads = new ArrayList<>();
                //Create Threads
                for( int j = 0; j < i; j++ ){
                    threads.add( new Thread( q, j + 1 ) );
                }
                long startTime = System.nanoTime();
                //Start Threads
                for( int j = 0; j < i; j++ ){
                    threads.get(j).start();
                }
                //Join Threads
                for( int j = 0; j < i; j++ ){
                    //System.out.println("Waiting for " + threads.get(j).getName());
                    threads.get(j).join();
                    //System.out.println("Done -> " + threads.get(j).getName());    
                }
                long endTime = System.nanoTime();

                total_duration += (endTime - startTime);
            }
            //Report Time
            System.out.println( i + " Threads took " + (float) (NUM_OPS * i)  / ( ( total_duration / 5 ) / 1000000 ) + " ops / millisecond on average" );
        }
    }
}