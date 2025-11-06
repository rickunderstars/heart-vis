#pragma once

#include <string>

class Triangle {
  public:
	int vertices[3] = {0, 0, 0};
	int groupID = -1;

	Triangle();
	Triangle(int vertices[3], int groupID);

	int v0();

	int v1();

	int v2();

	std::string toString();

	std::string toObj();

	std::string toPly();
};