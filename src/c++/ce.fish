#!/usr/bin/env fish
# script to compile and execute all .cpp files in the same directory

g++ *.cpp -o converter.out && ./converter.out