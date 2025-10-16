#include "utils.h"

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <string>
#include <vector>

bool isWhitespace(const std::string &str) {
	return all_of(str.begin(), str.end(),
				  [](unsigned char c) { return isspace(c); });
}

void printVector(std::vector<std::string> v) {
	for (int i = 0; i < v.size(); i++) {
		std::cout << v[i] << " // ";
	}
}

std::istream &getCleanLine(std::ifstream &file, std::string &line) {
	getline(file, line);
	line.erase(remove(line.begin(), line.end(), '\r'), line.end());
	boost::trim(line);
	return file;
}