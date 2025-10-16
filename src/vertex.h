#pragma once

#include "vector3.h"

class Vertex {
  private:
	Vector3 pos;
	Vector3 normal;
	int groupID;

  public:
	Vertex();
	Vertex(Vector3 &pos, Vector3 &normal, int id);
	std::string toString();

	std::string posToObj();

	std::string normalToObj();

	std::string toPly();
};