#pragma once

#include "triangle.h"
#include "utils.h"
#include "vector3.h"
#include "vertex.h"
#include <iostream>

class Mesh {
  private:
	std::string meshID = "";
	std::vector<vertex> vertices;
	std::vector<triangle> triangles;

	Mesh(std::vector<vertex> &vertices, std::vector<triangle> &triangles,
		 std::string meshID);

	static Mesh sectionsHandler(std::ifstream &file);

	static void generalAttributesSection(std::ifstream &file, int &vertNum,
										 int &triNum, std::string &id);

	static void verticesSection(std::ifstream &file,
								std::vector<vertex> &vertices);

	static void trianglesSection(std::ifstream &file,
								 std::vector<triangle> &triangle);

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