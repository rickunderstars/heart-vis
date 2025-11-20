#pragma once

#include <string>

class Triangle {
  public:
	int vertices[3] = {0, 0, 0};
	int groupID = -1;

	Triangle();
	Triangle(const int vertices[3], int groupID);
	Triangle(int v0, int v1, int v2, int groupID);

	std::string toObj();

	std::string toPly();
};