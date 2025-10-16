#pragma once

#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>
#include <string>

class Vertex {
  public:
	glm::vec3 pos = glm::vec3();
	glm::vec3 normal = glm::vec3();
	int groupID = -1;
	float unipolar = -1;
	float bipolar = -1;
	float LAT = -1;
	int EML = -1;
	int ExtEML = -1;
	int SCAR = -1;

	Vertex();
	Vertex(glm::vec3 &pos, glm::vec3 &normal, int id);
	std::string toString();

	std::string posToObj();

	std::string normalToObj();

	std::string toPly(std::string quality);
};