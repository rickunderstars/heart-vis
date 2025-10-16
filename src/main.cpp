#include "mesh.h"
#include <glm/ext/vector_float3.hpp>

int main() {
	std::string file1 = "../assets/mesh/2-LA.mesh";
	Mesh msh1 = Mesh::importMesh(file1);
	msh1.toObj(file1);
	msh1.toPly(file1);

	std::string file2 = "../assets/mesh/2-LA-FA.mesh";
	Mesh msh2 = Mesh::importMesh(file1);
	msh2.toObj(file2);
	msh2.toPly(file2);
	return 0;
}