#pragma once

#include "triangle.h"
#include "utils.h"
#include "vector3.h"
#include "vertex.h"

class Mesh {
  private:
	std::string meshID = "";
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;

	Mesh(std::vector<Vertex> &vertices, std::vector<Triangle> &triangles,
		 std::string meshID);

	static Mesh sectionsHandler(std::ifstream &file);

	static void generalAttributesSection(std::ifstream &file, int &vertNum,
										 int &triNum, std::string &id);

	static void verticesSection(std::ifstream &file,
								std::vector<Vertex> &vertices);

	static void trianglesSection(std::ifstream &file,
								 std::vector<Triangle> &triangle);

  public:
	static Mesh importMesh(std::string filepath);

	std::string getMeshID();

	int getVertexNum();

	int getTriangleNum();

	std::string verticesString();

	std::string trianglesString();

	bool toObj(std::string filename);

	bool toPly(std::string filename);

	std::string toString();
};