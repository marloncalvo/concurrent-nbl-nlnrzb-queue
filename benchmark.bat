g++ -pthread -I src/headers -g tests/mrlock_qqueue.cc -o mrlock_test

mrlock_test.exe

javac src/qqueue/*.java
cd src
java qqueue/Benchmark
cd ..