#include "mesh.hpp"
#include "utils.hpp"

int main() {
	std::vector<std::string> qualities = {"unipolar", "bipolar", "lat",
										  "eml",	  "exteml",	 "scar"};

	std::string file1 = "../assets/mesh/2-LA.mesh";
	Mesh msh1 = importMesh(file1);
	msh1.toObj(file1);
	for (std::string q : qualities) {
		msh1.toPly(file1, q);
	}

	std::string file2 = "../assets/mesh/2-LA-FA.mesh";
	Mesh msh2 = importMesh(file1);
	msh2.toObj(file2);

	for (std::string q : qualities) {
		msh2.toPly(file2, q);
	}

	return 0;
}