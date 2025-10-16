#pragma once

#include <iostream>
#include <vector>

bool isWhitespace(const std::string &str);

void printVector(std::vector<std::string> v);

std::istream &getCleanLine(std::ifstream &file, std::string &line);