#include "vertex.h"

#include <sstream>

Vertex::Vertex()
	: pos(Vector3(0, 0, 0)), normal(Vector3(0, 0, 0)), groupID(-1) {}
Vertex::Vertex(Vector3 &pos, Vector3 &normal, int id) {
	this->pos = pos;
	this->normal = normal;
	this->groupID = id;
}
std::string Vertex::toString() {
	std::ostringstream oss;
	oss << "{ pos: " << pos.toString() << " | normal: " << normal.toString()
		<< " | groupID: " << groupID << " }";
	return oss.str();
}

std::string Vertex::posToObj() {
	std::ostringstream oss;
	oss << "v  " << pos.getX() << " " << pos.getY() << " " << pos.getZ();
	return oss.str();
}

std::string Vertex::normalToObj() {
	std::ostringstream oss;
	oss << "vn " << normal.getX() << " " << normal.getY() << " "
		<< normal.getZ();
	return oss.str();
}

std::string Vertex::toPly() {
	std::ostringstream oss;
	oss << pos.getX() << " " << pos.getY() << " " << pos.getZ() << " "
		<< normal.getX() << " " << normal.getY() << " " << normal.getZ();
	return oss.str();
}
