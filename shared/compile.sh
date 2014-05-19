#!/bin/bash

g++ -std=c++11 -Wall -fPIC -c *.cpp
g++ -shared -o libiln.so *.o

mv libiln.so ../debug/
