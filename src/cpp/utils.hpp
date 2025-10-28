#pragma once
#include "mesh.hpp"
#include <iostream>
#include <vector>

bool isWhitespace(const std::string &str);

void printVector(std::vector<std::string> v);

std::istream &getCleanLine(std::ifstream &file, std::string &line);

std::string fileToString(std::string filepath);

Mesh sectionsHandler(std::ifstream &file);

void generalAttributesSection(std::ifstream &file, int &vertNum, int &triNum,
							  std::string &id);

void verticesSection(std::ifstream &file, std::vector<Vertex> &vertices);

void trianglesSection(std::ifstream &file, std::vector<Triangle> &triangle);

void verticesColorsSection(std::ifstream &file, std::vector<Vertex> &vertices);

void verticesAttributesSection(std::ifstream &file,
							   std::vector<Vertex> &vertices);

Mesh importMesh(std::string filepath);
