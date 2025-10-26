#pragma once

#include "triangle.hpp"
#include "vertex.hpp"
#include <vector>

class Mesh {
  public:
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;

	Mesh(std::vector<Vertex> &vertices, std::vector<Triangle> &triangles);

	bool triangleFix(int face, int oldVertex, int newVertex);

	bool toObj(std::string filename);

	bool toPly(std::string filename, std::string quality);

	static Mesh simpleShape();

	std::string verticesString();

	std::string trianglesString();

	std::string toString();
};