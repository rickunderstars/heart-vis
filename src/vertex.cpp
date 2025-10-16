#include "vertex.h"

#include <sstream>

vertex::vertex()
	: pos(vector3(0, 0, 0)), normal(vector3(0, 0, 0)), groupID(-1) {}
vertex::vertex(vector3 &pos, vector3 &normal, int id) {
	this->pos = pos;
	this->normal = normal;
	this->groupID = id;
}
std::string vertex::toString() {
	std::ostringstream oss;
	oss << "{ pos: " << pos.toString() << " | normal: " << normal.toString()
		<< " | groupID: " << groupID << " }";
	return oss.str();
}

std::string vertex::posToObj() {
	std::ostringstream oss;
	oss << "v  " << pos.getX() << " " << pos.getY() << " " << pos.getZ();
	return oss.str();
}

std::string vertex::normalToObj() {
	std::ostringstream oss;
	oss << "vn " << normal.getX() << " " << normal.getY() << " "
		<< normal.getZ();
	return oss.str();
}

std::string vertex::toPly() {
	std::ostringstream oss;
	oss << pos.getX() << " " << pos.getY() << " " << pos.getZ() << " "
		<< normal.getX() << " " << normal.getY() << " " << normal.getZ();
	return oss.str();
}
