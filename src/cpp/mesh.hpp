#pragma once

#include "triangle.hpp"
#include "vertex.hpp"
#include <emscripten/val.h>
#include <vector>

class Mesh {
  public:
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;

	float maxUnipolar = -1;
	float maxBipolar = -1;
	float maxLAT = -1;
	int maxEML = -1;
	int maxExtEML = -1;
	int maxSCAR = -1;

	float minUnipolar = -1;
	float minBipolar = -1;
	float minLAT = -1;
	int minEML = -1;
	int minExtEML = -1;
	int minSCAR = -1;

	Mesh(std::vector<Vertex> &vertices, std::vector<Triangle> &triangles);

	bool triangleFix(int face, int oldVertex, int newVertex);

	void fixNMEdges();

	void calcQualitiesMinMax();

	void calcQualitiesNorm();

	std::string toObjString();

	std::string toPlyString(std::string quality = "");

	static Mesh simpleShape();

	std::string verticesString();

	std::string trianglesString();

	std::string toString();

	emscripten::val Float32ArrayOfVertices() const;

	emscripten::val Uint32ArrayOfTriangles() const;

	emscripten::val Float32ArrayOfTurboColors(std::string quality) const;
};