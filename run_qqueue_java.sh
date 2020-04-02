#!/bin/bash

rm src/**/*.class
make qqueue
pushd src
java tests.QQueueTest
popd
