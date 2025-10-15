#include "utils.h"

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

bool isWhitespace(const string &str) {
	return all_of(str.begin(), str.end(),
				  [](unsigned char c) { return isspace(c); });
}

void printVector(vector<string> v) {
	for (int i = 0; i < v.size(); i++) {
		cout << v[i] << " // ";
	}
}

istream &getCleanLine(ifstream &file, string &line) {
	getline(file, line);
	line.erase(remove(line.begin(), line.end(), '\r'), line.end());
	boost::trim(line);
	return file;
}