#pragma once

#include <iostream>

class triangle {
  private:
	int vertices[3];
	int groupID;

  public:
	triangle();
	triangle(int vertices[3], int groupID);

	std::string toString();

	std::string toObj();

	std::string toPly();
};