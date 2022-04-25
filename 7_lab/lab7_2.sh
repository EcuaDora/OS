#!/bin/bash
g++ -pthread -o lab7_2 lab7_2.cpp -lrt
sudo setcap cap_sys_resource=eip lab7_2
./lab7_2