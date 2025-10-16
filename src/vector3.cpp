#include "vector3.h"

#include <sstream>

Vector3::Vector3() : x(0), y(0), z(0) {}
Vector3::Vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}
std::string Vector3::toString() {
	std::ostringstream oss;
	oss << "(" << x << ", " << y << ", " << z << ")";
	return oss.str();
}
std::string Vector3::getX() {
	std::ostringstream oss;
	oss << x;
	return oss.str();
}
std::string Vector3::getY() {
	std::ostringstream oss;
	oss << y;
	return oss.str();
}
std::string Vector3::getZ() {
	std::ostringstream oss;
	oss << z;
	return oss.str();
}
