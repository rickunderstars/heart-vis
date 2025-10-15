#pragma once

#include <iostream>
#include <vector>

using namespace std;

bool isWhitespace(const string &str);

void printVector(vector<string> v);

istream &getCleanLine(ifstream &file, string &line);