#include <iostream>
#include <string>
#include <time.h>
#include <chrono>
#include <stdio.h>
#include <pthread.h>
#include "mrlock_qqueue.h"

#define N 10000
//#define N_T 4

#define DEBUG 0

int arr[N];
QQueue<int> q;
std::atomic<int> counter;

void debug(std::string s);
void print();

class test {
public:
    int op;
    long val;
};

class benchmark {
public:
    std::vector<test> tests;
};

/**
 * Runs all the preinitialized tests one by one, which optional debug statements.
 */
void* foo(void* _benchmarks) {

    benchmark b = *reinterpret_cast<benchmark*>(_benchmarks);

    while(!b.tests.empty()) {
        test test = b.tests.back();
        b.tests.pop_back();

        if (test.op == -1) {
            debug("Pop - " + std::to_string(test.val) + "\n");
            q.pop(&arr[test.val]);
        } else {
            debug("Push - " + std::to_string(test.val) + "\n");
            q.push(test.val);
        }

        //print();
    }

    return NULL;
}

void debug(std::string s) {
    if(DEBUG)
        std::cout << s;
}

void print() {
    std::string out = "";
    for (int i = 0; i < N; i++) {
        out += std::to_string(arr[i]) + " ";
    }

    debug(out + "\n");
}

int main(void) {

    for( int N_T = 1; N_T <= 32; N_T++ ){
        srand( (unsigned)time(NULL) );
    
	    pthread_t threads[N_T];
        benchmark benchmarks[N_T];

        /**
         * This generates all the tests
         */
        for (int i = 0; i < N; i++) {
            test t;
            if((rand() % 100) < 50) {
                t.op = -1;
                t.val = i;
            } else {
                t.op = 1;
                t.val = rand() * 100000 + 1;
            }

            benchmarks[i%N_T].tests.push_back(t);
        }

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < N_T; i++) {
            pthread_create(&threads[i], NULL, foo, (void*)&benchmarks[i]);
        }

        for (int i = 0; i < N_T; i++) {
            pthread_join(threads[i], NULL);
        }

        auto stop = std::chrono::high_resolution_clock::now();

        std::cout << N_T << " Threads took " << (float) (N * N_T) / std::chrono::duration_cast<std::chrono::milliseconds>( stop - start ).count() << " ops / milliseconds\n"; 

    }

/*
    for (int i = 0; i < N; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
*/

    return 0;
}
