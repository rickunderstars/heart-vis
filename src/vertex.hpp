#pragma once

#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>
#include <string>

class Vertex {
  public:
	glm::vec3 pos;
	glm::vec3 normal;
	int groupID;

	Vertex();
	Vertex(glm::vec3 &pos, glm::vec3 &normal, int id);
	std::string toString();

	std::string posToObj();

	std::string normalToObj();

	std::string toPly();
};