#pragma once

#include <iostream>

class vector3 {
  private:
	float x;
	float y;
	float z;

  public:
	vector3();
	vector3(float x, float y, float z);
	std::string toString();

	std::string getX();
	std::string getY();
	std::string getZ();
};