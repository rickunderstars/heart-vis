#include "vector3.h"

#include <sstream>

vector3::vector3() : x(0), y(0), z(0) {}
vector3::vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}
std::string vector3::toString() {
	std::ostringstream oss;
	oss << "(" << x << ", " << y << ", " << z << ")";
	return oss.str();
}
std::string vector3::getX() {
	std::ostringstream oss;
	oss << x;
	return oss.str();
}
std::string vector3::getY() {
	std::ostringstream oss;
	oss << y;
	return oss.str();
}
std::string vector3::getZ() {
	std::ostringstream oss;
	oss << z;
	return oss.str();
}
