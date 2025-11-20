#include "vertex.hpp"

#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <sstream>
#include <string>

Vertex::Vertex()
	: pos(glm::vec3(0, 0, 0)), normal(glm::vec3(0, 0, 0)), groupID(-1),
	  unipolar(0), bipolar(0), LAT(0), EML(-1), ExtEML(-1), SCAR(-1) {}
Vertex::Vertex(glm::vec3 &pos, glm::vec3 &normal, int id) {
	this->pos = pos;
	this->normal = normal;
	this->groupID = id;
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

std::string Vertex::toPly(std::string quality) {

	std::string q = "-1";

	if (quality == "unipolar") {
		q = std::to_string(this->unipolar);
	} else if (quality == "bipolar") {
		q = std::to_string(this->bipolar);
	} else if (quality == "lat") {
		q = std::to_string(this->LAT);
	} else if (quality == "eml") {
		q = std::to_string(this->EML);
	} else if (quality == "exteml") {
		q = std::to_string(this->ExtEML);
	} else if (quality == "scar") {
		q = std::to_string(this->SCAR);
	}

	std::ostringstream oss;
	oss << pos.x << " " << pos.y << " " << pos.z << " " << normal.x << " "
		<< normal.y << " " << normal.z << " " << q;
	return oss.str();
}
