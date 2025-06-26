#!/bin/bash
g++ -std=c++23 main.cpp NFL_Players.cpp -lsqlite3 -o main
./main
