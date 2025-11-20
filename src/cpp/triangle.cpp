#include "triangle.hpp"

#include <sstream>

Triangle::Triangle() : vertices{0, 0, 0}, groupID(-1) {}
Triangle::Triangle(const int vertices[3], int groupID) {
	for (int i = 0; i < 3; i++) {
		this->vertices[i] = vertices[i];
	}
	this->groupID = groupID;
}

Triangle::Triangle(int v0, int v1, int v2, int groupID) {
	this->vertices[0] = v0;
	this->vertices[1] = v1;
	this->vertices[2] = v2;
	this->groupID = groupID;
}

std::string Triangle::toObj() {
	std::ostringstream oss;
	oss << "f  " << vertices[0] + 1 << "//" << vertices[0] + 1 << " "
		<< vertices[1] + 1 << "//" << vertices[1] + 1 << " " << vertices[2] + 1
		<< "//" << vertices[2] + 1;
	return oss.str();
}

std::string Triangle::toPly() {
	std::ostringstream oss;
	oss << 3 << " " << vertices[0] << " " << vertices[1] << " " << vertices[2];
	return oss.str();
}