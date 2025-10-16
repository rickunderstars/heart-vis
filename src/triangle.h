#pragma once

#include <string>

class Triangle {
  private:
	int vertices[3];
	int groupID;

  public:
	Triangle();
	Triangle(int vertices[3], int groupID);

	std::string toString();

	std::string toObj();

	std::string toPly();
};