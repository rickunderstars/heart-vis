#include "mesh.hpp"
#include "triangle.hpp"
#include "utils.hpp"

int main() {
	std::string file1 = "assets/mesh/2-LA.mesh";
	std::string file2 = "assets/mesh/2-LA-FA.mesh";
	std::vector<std::string> qualities = {"unipolar", "bipolar", "lat",
										  "eml",	  "exteml",	 "scar"};

	std::string buffer1 = fileToString(file1);
	Mesh msh1 = importMesh(buffer1);
	msh1.triangleFix(8703, 4559, 4538);
	for (std::string q : qualities) {
		msh1.toPly(file1, q);
	}
	std::string buffer2 = fileToString(file2);
	Mesh msh2 = importMesh(buffer2);
	msh2.triangleFix(25180, 12810, 12813);
	msh2.triangleFix(29108, 9930, 14703);
	msh2.triangleFix(21420, 10857, 10941);
	msh2.triangleFix(56, 38, 29);
	msh2.triangleFix(30812, 15492, 15447);
	msh2.triangleFix(30578, 14384, 14398);
	int face[3] = {15417, 14398, 14381};
	Triangle newTri(face, -1);
	msh2.triangles.push_back(newTri);

	for (std::string q : qualities) {
		msh2.toPly(file2, q);
	}

	Mesh smsh = Mesh::simpleShape();
	smsh.toPly("assets/mesh/simple");

	return 0;
}