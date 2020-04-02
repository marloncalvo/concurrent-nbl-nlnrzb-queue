# File Structure
Under `src/`, you can find the implementations for each of the data structures.
Under `src/tests/`, you can find the test for each implementation.  
We have two source languages for our implementations, where we utilize C++ for `naive_qqueue` and `mrlock_qqueue`, 
and `Java 11` for `qqueue` (the full implementation).  
All the implementations in C++ can be found under `src/headers`, and Java can be found in `src/qqueue`. 

# Environment
This was built, and tested, on `gcc (GCC) 9.2.1 20190827 (Red Hat 9.2.1-1)` in Fedora 31.
Should not require any special headers. Furthermore, `Java 11` was utilized for `qqueue`.

# Running
In this project, there are three implementatinos of the QQueue.
1. The `naive_qqueue` which can be found under src/headers/naive_qqueue.h.
	* Test: `src/tests/naive_qqueue.cc`, Impl: `src/headers/naive_qqueue.h`.
2. The `mrlock_qqueue` which can be found under `src/headers/mrlock_qqueue.h`.
	* Test: `src/tests/mrlock_qqueue.cc`, Impl: `src/headers/mrlock_qqueue.h`.
3. The "full" implementation `qqueue` which can be found under `src/qqueue/QQueue.java`.
	* Test: `src/tests/QQueueTest.java`, Impl: `src/qqueue/QQueue.java`

To compile, run `make`, and the mrlock and naive mplementation tests will be built. The java classes will also be generated.  
Run either `./naive_test` or `./mrlock_test`.  
For Java, either do `java tests.QQueueTest` under `src/`, or run `run_qqueue_java.sh` in the root directory.

In the root, you will also find a discussion about our implementation and data structure.
