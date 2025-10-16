#pragma once

#include "triangle.hpp"
#include "vertex.hpp"
#include <vector>

class Mesh {
  public:
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;

	Mesh(std::vector<Vertex> &vertices, std::vector<Triangle> &triangles);

	std::string verticesString();

	std::string trianglesString();

	bool toObj(std::string filename);

	bool toPly(std::string filename);

	std::string toString();
};