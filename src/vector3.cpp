#include "vector3.h"

#include <sstream>

using namespace std;

vector3::vector3() : x(0), y(0), z(0) {}
vector3::vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}
string vector3::toString() {
	ostringstream oss;
	oss << "(" << x << ", " << y << ", " << z << ")";
	return oss.str();
}
string vector3::getX() {
	ostringstream oss;
	oss << x;
	return oss.str();
}
string vector3::getY() {
	ostringstream oss;
	oss << y;
	return oss.str();
}
string vector3::getZ() {
	ostringstream oss;
	oss << z;
	return oss.str();
}
