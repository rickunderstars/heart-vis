#include "vertex.hpp"

#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>
#include <sstream>

Vertex::Vertex()
	: pos(glm::vec3(0, 0, 0)), normal(glm::vec3(0, 0, 0)), groupID(-1) {}
Vertex::Vertex(glm::vec3 &pos, glm::vec3 &normal, int id) {
	this->pos = pos;
	this->normal = normal;
	this->groupID = id;
}
std::string Vertex::toString() {
	std::ostringstream oss;
	oss << "{ pos: (" << pos.x << ", " << pos.y << ", " << pos.z
		<< ") | normal: (" << normal.x << ", " << normal.y << ", " << normal.z
		<< ") | groupID: " << groupID << " }";
	return oss.str();
}

std::string Vertex::posToObj() {
	std::ostringstream oss;
	oss << "v  " << pos.x << " " << pos.y << " " << pos.z;
	return oss.str();
}

std::string Vertex::normalToObj() {
	std::ostringstream oss;
	oss << "vn " << normal.x << " " << normal.y << " " << normal.z;
	return oss.str();
}

std::string Vertex::toPly() {
	std::ostringstream oss;
	oss << pos.x << " " << pos.y << " " << pos.z << " " << normal.x << " "
		<< normal.y << " " << normal.z;
	return oss.str();
}
