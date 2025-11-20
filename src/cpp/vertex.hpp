#pragma once

#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>
#include <string>

class Vertex {
  public:
	glm::vec3 pos = glm::vec3();

	int groupID = -666;

	float unipolar = -666;
	float bipolar = -666;
	float LAT = -666;
	int EML = -666;
	int ExtEML = -666;
	int SCAR = -666;

	Vertex();
	Vertex(glm::vec3 &pos);

	std::string toObj();

	std::string toPly(std::string quality);
};