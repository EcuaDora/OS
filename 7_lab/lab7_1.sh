#!/bin/bash
g++ -pthread -o lab7_1 lab7_1.cpp -lrt
sudo setcap cap_sys_resource=eip lab7_1
./lab7_1