#include "triangle.h"

#include <sstream>

triangle::triangle() : vertices{0, 0, 0}, groupID(-1) {}
triangle::triangle(int vertices[3], int groupID) {
	for (int i = 0; i < 3; i++) {
		this->vertices[i] = vertices[i];
	}
	this->groupID = groupID;
}

string triangle::toString() {
	ostringstream oss;
	oss << "{ triangle: [" << vertices[0] << ", " << vertices[1] << ", "
		<< vertices[2] << "] | groupID: " << groupID << " }";
	return oss.str();
}

string triangle::toObj() {
	ostringstream oss;
	oss << "f  " << vertices[0] << "//" << vertices[0] << " " << vertices[1]
		<< "//" << vertices[1] << " " << vertices[2] << "//" << vertices[2];
	return oss.str();
}

string triangle::toPly() {
	ostringstream oss;
	oss << 3 << " " << vertices[0] << " " << vertices[1] << " " << vertices[2];
	return oss.str();
}