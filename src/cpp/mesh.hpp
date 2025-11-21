#pragma once

#include "triangle.hpp"
#include "vertex.hpp"
#include <emscripten/val.h>
#include <vector>

class Mesh {
  public:
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;

	float maxUnipolar = -666;
	float maxBipolar = -666;
	float maxLAT = -666;
	int maxEML = -666;
	int maxExtEML = -666;
	int maxSCAR = -666;

	float minUnipolar = -666;
	float minBipolar = -666;
	float minLAT = -666;
	int minEML = -666;
	int minExtEML = -666;
	int minSCAR = -666;

	Mesh(std::vector<Vertex> &vertices, std::vector<Triangle> &triangles);

	bool triangleFix(int face, int oldVertex, int newVertex);

	void fixNMEdges();

	void calcQualitiesMinMax();

	float normalizedVertexQuality(int vIndex, std::string quality) const;

	std::string toObjString();

	std::string toPlyString(std::string quality = "");

	static Mesh simpleShape();

	emscripten::val Float32ArrayOfVertices() const;

	emscripten::val Uint32ArrayOfTriangles() const;

	emscripten::val Int32ArrayOfGroupID() const;

	emscripten::val Float32ArrayOfUnipolar() const;
	emscripten::val Float32ArrayOfBipolar() const;
	emscripten::val Float32ArrayOfLAT() const;
	emscripten::val Int32ArrayOfEML() const;
	emscripten::val Int32ArrayOfExtEML() const;
	emscripten::val Int32ArrayOfSCAR() const;

	emscripten::val Float32ArrayOfGroupIDTurboColors() const;
	emscripten::val Float32ArrayOfTurboColors(std::string quality) const;
};