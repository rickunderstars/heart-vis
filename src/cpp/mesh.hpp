#pragma once

#include "triangle.hpp"
#include "vertex.hpp"
#include <emscripten/val.h>
#include <vector>

class Mesh {
  public:
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;

	Mesh(std::vector<Vertex> &vertices, std::vector<Triangle> &triangles);

	bool triangleFix(int face, int oldVertex, int newVertex);

	void fixNMEdges();

	std::string toObjString();

	std::string toPlyString(std::string quality = "");

	static Mesh simpleShape();

	std::string verticesString();

	std::string trianglesString();

	std::string toString();

	emscripten::val Float32ArrayOfVertices() const;

	emscripten::val Uint32ArrayOfTriangles() const;
};