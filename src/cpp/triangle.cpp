#include "triangle.hpp"
#include <sstream>

Triangle::Triangle() : vertices{0, 0, 0}, groupID(NULL_VALUE) {}
Triangle::Triangle(std::array<int, 3> &vertices) { this->vertices = vertices; }
Triangle::Triangle(int v0, int v1, int v2) {
	vertices.at(0) = v0;
	vertices.at(1) = v1;
	vertices.at(2) = v2;
}

std::string Triangle::toObj() {
	std::ostringstream oss;
	oss << "f  " << vertices.at(0) + 1 << "//" << vertices.at(0) + 1 << " "
		<< vertices.at(1) + 1 << "//" << vertices.at(1) + 1 << " "
		<< vertices.at(2) + 1 << "//" << vertices.at(2) + 1;
	return oss.str();
}

std::string Triangle::toPly() {
	std::ostringstream oss;
	oss << 3 << " " << vertices.at(0) << " " << vertices.at(1) << " "
		<< vertices.at(2);
	return oss.str();
}