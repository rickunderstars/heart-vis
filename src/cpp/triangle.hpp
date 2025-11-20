#pragma once

#include <array>
#include <string>

class Triangle {
  public:
	std::array<int, 3> vertices = {0, 0, 0};
	int groupID = -666;

	Triangle();
	Triangle(std::array<int, 3> &vertices);
	Triangle(int v0, int v1, int v2);

	std::string toObj();

	std::string toPly();
};