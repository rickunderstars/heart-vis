#pragma once

#include "vector3.h"

class vertex {
  private:
	vector3 pos;
	vector3 normal;
	int groupID;

  public:
	vertex();
	vertex(vector3 &pos, vector3 &normal, int id);
	string toString();

	string posToObj();

	string normalToObj();

	string toPly();
};