#include "mesh.hpp"
#include "triangle.hpp"
#include "vertex.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <sstream>
#include <vector>

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<Triangle> &triangles) {
	this->vertices = vertices;
	this->triangles = triangles;
}

std::string Mesh::toObjString() {
	std::ostringstream oss;
	oss << "# Obj file converted from proprietary mesh format";
	oss << "\n\n############\n# vertices #\n############\n\n";
	for (int i = 0; i < vertices.size(); i++) {
		oss << vertices[i].posToObj() << "\n";
	}
	oss << "\n\n\n############\n# vertices normals #\n############\n\n";
	for (int i = 0; i < vertices.size(); i++) {
		oss << vertices[i].normalToObj() << "\n";
	}
	oss << "\n\n\n############\n# triangles #\n############\n\n";
	for (int i = 0; i < triangles.size(); i++) {
		oss << triangles[i].toObj() << "\n";
	}
	return oss.str();
}

std::string Mesh::toPlyString(std::string quality) {
	std::vector<std::string> qualities = {"",	 "unipolar", "bipolar", "lat",
										  "eml", "exteml",	 "scar"};
	for (char &c : quality) {
		c = std::tolower(static_cast<unsigned char>(c));
	}
	bool quality_found =
		std::any_of(qualities.begin(), qualities.end(),
					[&](const std::string &s) { return s == quality; });

	if (!quality_found) {
		std::cerr << "'" << quality << "' is not an attribute.\n";
		exit(1);
	}

	std::ostringstream oss;

	oss << "ply\nformat ascii 1.0\n";
	oss << "comment Ply file converted from proprietary mesh format\n";
	oss << "element vertex " << vertices.size() << "\n";
	oss << "property float x\nproperty float y\nproperty float "
		   "z\nproperty float nx\nproperty float ny\nproperty "
		   "float nz\n";
	oss << "property float quality\n";
	oss << "element face " << triangles.size() << "\n";
	oss << "property list uchar int vertex_indices\n";
	oss << "end_header\n";

	for (int i = 0; i < vertices.size(); i++) {
		oss << vertices[i].toPly(quality) << "\n";
	}
	for (int i = 0; i < triangles.size(); i++) {
		oss << triangles[i].toPly() << "\n";
	}
	oss << "\n\n";

	return oss.str();
}

bool Mesh::triangleFix(int face, int oldVertex, int newVertex) {
	if (face < 0 || face >= triangles.size()) {
		std::cerr << "Face " << face << " out of bounds (0-"
				  << triangles.size() - 1 << ")." << std::endl;
		return false;
	}
	int v = -1;
	for (int i = 0; i < 3; i++) {
		if (oldVertex == triangles[face].vertices[i]) {
			v = i;
		}
	}
	if (v < 0) {
		std::cerr << "Vertex " << oldVertex << " is not in face " << face
				  << ": (" << triangles[face].vertices[0] << ", "
				  << triangles[face].vertices[1] << ", "
				  << triangles[face].vertices[2] << " )" << std::endl;
		return false;
	}
	if (newVertex < 0 || newVertex >= vertices.size()) {
		std::cerr << "Vertex " << newVertex << " out of bounds (0-"
				  << vertices.size() - 1 << ")." << std::endl;
		return false;
	}

	triangles[face].vertices[v] = newVertex;

	return true;
}

std::string Mesh::verticesString() {
	std::ostringstream oss;
	for (int i = 0; i < vertices.size(); i++) {
		oss << i << ": " << vertices[i].toString() << std::endl;
	}
	return oss.str();
}

std::string Mesh::trianglesString() {
	std::ostringstream oss;
	for (int i = 0; i < triangles.size(); i++) {
		oss << i << ": " << triangles[i].toString() << std::endl;
	}
	return oss.str();
}

std::string Mesh::toString() {
	std::ostringstream oss;
	oss << "- Vertices -\n"
		<< "----------------------------------------------------\n"
		<< verticesString()
		<< "\n\n- Triangles -\n"
		   "----------------------------------------------------\n"
		<< trianglesString() << "\n";
	return oss.str();
}

Mesh Mesh::simpleShape() {
	std::vector<Triangle> tri(2);
	int f0[3] = {0, 1, 2};
	int f1[3] = {0, 2, 3};
	tri[0] = Triangle(f0, -1);
	tri[1] = Triangle(f1, -1);

	std::vector<Vertex> vert(4);
	glm::vec3 nrm(0, 0, 1);
	glm::vec3 v0(0, 0, 0);
	glm::vec3 v1(0, 2, 0);
	glm::vec3 v2(2, 2, 0);
	glm::vec3 v3(2, 0, 0);
	vert[0] = Vertex(v0, nrm, -1);
	vert[1] = Vertex(v1, nrm, -1);
	vert[2] = Vertex(v2, nrm, -1);
	vert[3] = Vertex(v3, nrm, -1);

	return Mesh(vert, tri);
};
