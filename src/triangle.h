#pragma once

#include <iostream>

using namespace std;

class triangle {
  private:
	int vertices[3];
	int groupID;

  public:
	triangle();
	triangle(int vertices[3], int groupID);

	string toString();

	string toObj();

	string toPly();
};