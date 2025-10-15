#pragma once

#include "triangle.h"
#include "utils.h"
#include "vector3.h"
#include "vertex.h"
#include <iostream>

using namespace std;

class Mesh {
  private:
	string meshID = "";
	vector<vertex> vertices;
	vector<triangle> triangles;

	Mesh(vector<vertex> &vertices, vector<triangle> &triangles, string meshID);

	static Mesh sectionsHandler(ifstream &file);

	static void generalAttributesSection(ifstream &file, int &vertNum,
										 int &triNum, string &id);

	static void verticesSection(ifstream &file, vector<vertex> &vertices);

	static void trianglesSection(ifstream &file, vector<triangle> &triangle);

  public:
	static Mesh importMesh(string filepath);

	string getMeshID();

	int getVertexNum();

	int getTriangleNum();

	string verticesString();

	string trianglesString();

	bool toObj(string filename);

	bool toPly(string filename);

	string toString();
};