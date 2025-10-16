#pragma once

#include <string>

class Vector3 {
  private:
	float x;
	float y;
	float z;

  public:
	Vector3();
	Vector3(float x, float y, float z);
	std::string toString();

	std::string getX();
	std::string getY();
	std::string getZ();
};