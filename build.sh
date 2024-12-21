#!/bin/sh

set -xe

cc main.cpp -o main -I/usr/local/include -L/usr/local/lib -lraylib -lstdc++
