#include "triangle.hpp"

#include <sstream>

Triangle::Triangle() : vertices{0, 0, 0}, groupID(-1) {}
Triangle::Triangle(int vertices[3], int groupID) {
	for (int i = 0; i < 3; i++) {
		this->vertices[i] = vertices[i];
	}
	this->groupID = groupID;
}

std::string Triangle::toString() {
	std::ostringstream oss;
	oss << "tri[" << vertices[0] << ", " << vertices[1] << ", " << vertices[2]
		<< "] ---- groupID(" << groupID << ")";
	return oss.str();
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