#pragma once

#include <string>

class Triangle {
  public:
	int vertices[3] = {0, 0, 0};
	int groupID = -666;

	Triangle();
	Triangle(const int vertices[3]);
	Triangle(int v0, int v1, int v2);

	std::string toObj();

	std::string toPly();
};