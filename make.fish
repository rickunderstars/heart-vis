#!/usr/bin/env fish
# script to compile and execute all .cpp files in the same directory

cmake cbuild && cmake --build cbuild && ./cbuild/heart.out