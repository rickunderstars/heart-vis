#pragma once

#include <iostream>

using namespace std;

class vector3 {
  private:
	float x;
	float y;
	float z;

  public:
	vector3();
	vector3(float x, float y, float z);
	string toString();

	string getX();
	string getY();
	string getZ();
};