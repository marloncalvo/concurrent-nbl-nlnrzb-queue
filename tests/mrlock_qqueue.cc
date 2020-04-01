#include <iostream>
#include <utility>
#include <string>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <thread>

#include <time.h>
#include <stdio.h>
#include "mrlock_qqueue.h"

// ============================== DEBUGGING STUFF =====================

#define DEBUG 0

void debug(std::string s);
void print();

void debug(std::string s) {
    if(DEBUG)
        std::cout << s;
}

// ============================== TESTING STARTS HERE =====================

class test {
public:
    int op;
    int val;
};

class benchmark {
public:
    std::vector<test> tests;
};

/**
 * Runs all the preinitialized tests one by one, which optional debug statements.
 */
void* _perform_test(QQueue<int> *q, benchmark b) {

    while(!b.tests.empty()) {
        test test = b.tests.back();
        b.tests.pop_back();

        if (test.op == -1) {
            debug("Pop - " + std::to_string(test.val) + "\n");
            q->pop(&test.val);
        } else {
            debug("Push - " + std::to_string(test.val) + "\n");
            q->push(test.val);
        }
    }

    return NULL;
}

std::pair<double, double> perform_test(int n_threads, int n_ops, int perc_push) {

    QQueue<int> q;

    std::thread threads[n_threads];
    benchmark benchmarks[n_threads];

    /**
     * This generates all the tests
     */
    for (int i = 0;i < n_threads; i++) {
        for (int j = 0; j < n_ops; j++) {
            test t;
            if(rand() % 101 > perc_push) {
                t.op = -1;
                t.val = -1;
            } else {
                t.op = 1;
                t.val = rand() * 100000 + 1;
            }

            benchmarks[i].tests.push_back(t);
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < n_threads; i++) {
        threads[i] = std::thread(_perform_test, &q, benchmarks[i]);
    }

    for (int i = 0; i < n_threads; i++) {
        threads[i].join();
    }

    auto stop = std::chrono::high_resolution_clock::now();

    double total_time = std::chrono::duration<float>( stop - start ).count();
    double op_s       = n_ops / total_time;
    //std::cout << N_T << " Threads took " << (float) (N * N_T) / std::chrono::duration_cast<std::chrono::milliseconds>( stop - start ).count() << " ops / milliseconds\n";

    return std::make_pair(total_time, op_s);
}

int main(void) {
    srand( (unsigned)time(NULL) );

    std::ofstream res_file;
    res_file.open("mrlock_results.csv");

    res_file << "n_threads,n_ops,total_time,op/s\n";

    int N_THREADS = 32;
    int N_OPS     = 1000;
    int p_push    = 60;

    for (int i = 1; i <= N_THREADS; i++) {
        std::pair<double, double> results = perform_test(i, N_OPS, p_push);
        std::cout << "Finished " << i << " Threads\n";
        double total_time = results.first;
        double op_s       = results.second;
        res_file << std::fixed << i << "," << N_OPS << "," << std::setprecision(5) << total_time << "," << std::setprecision(2) << op_s << std::endl;
    }

    res_file.close();
}
