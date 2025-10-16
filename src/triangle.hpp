#pragma once

#include <string>

class Triangle {
  public:
	int vertices[3];
	int groupID;

	Triangle();
	Triangle(int vertices[3], int groupID);

	std::string toString();

	std::string toObj();

	std::string toPly();
};