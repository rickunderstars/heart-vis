#include "triangle.hpp"

#include <sstream>

Triangle::Triangle() : vertices{0, 0, 0}, groupID(-666) {}
Triangle::Triangle(const int vertices[3]) {
	for (int i = 0; i < 3; i++) {
		this->vertices[i] = vertices[i];
	}
}

Triangle::Triangle(int v0, int v1, int v2) {
	this->vertices[0] = v0;
	this->vertices[1] = v1;
	this->vertices[2] = v2;
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