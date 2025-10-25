#include "mesh.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<Triangle> &triangles) {
	this->vertices = vertices;
	this->triangles = triangles;
}

bool Mesh::toObj(std::string filename) {
	std::string ext = ".obj";
	if (!(filename.length() >= ext.length() &&
		  filename.substr(filename.length() - ext.length()) == ext)) {
		filename = filename + ext;
	}
	std::ofstream fileOut(filename);
	if (fileOut.is_open()) {
		fileOut << "# Obj file converted from proprietary mesh format";
		fileOut << "\n\n############\n# vertices #\n############\n\n";
		for (int i = 0; i < vertices.size(); i++) {
			fileOut << vertices[i].posToObj() << "\n";
		}
		fileOut << "\n\n\n############\n# vertices normals #\n############\n\n";
		for (int i = 0; i < vertices.size(); i++) {
			fileOut << vertices[i].normalToObj() << "\n";
		}
		fileOut << "\n\n\n############\n# triangles #\n############\n\n";
		for (int i = 0; i < triangles.size(); i++) {
			fileOut << triangles[i].toObj() << "\n";
		}
	} else {
		std::cerr << "Could not create obj file." << std::endl;
		return false;
	}
	fileOut.close();
	return true;
}

bool Mesh::toPly(std::string filename, std::string quality) {
	std::string ext = ".ply";
	std::vector<std::string> qualities = {"unipolar", "bipolar", "lat",
										  "eml",	  "exteml",	 "scar"};
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

	if (!(filename.length() >= ext.length() &&
		  filename.substr(filename.length() - ext.length()) == ext)) {
		filename = filename + "_" + quality + ext;
	}
	std::ofstream fileOut(filename);
	if (fileOut.is_open()) {

		fileOut << "ply\nformat ascii 1.0\n";
		fileOut << "comment Ply file converted from proprietary mesh format\n";
		fileOut << "element vertex " << vertices.size() << "\n";
		fileOut << "property float x\nproperty float y\nproperty float "
				   "z\nproperty float nx\nproperty float ny\nproperty "
				   "float nz\n";
		fileOut << "property float quality\n";
		fileOut << "element face " << triangles.size() << "\n";
		fileOut << "property list uchar int vertex_indices\n";
		fileOut << "end_header\n";

		for (int i = 0; i < vertices.size(); i++) {
			fileOut << vertices[i].toPly(quality) << "\n";
		}
		for (int i = 0; i < triangles.size(); i++) {
			fileOut << triangles[i].toPly() << "\n";
		}
		fileOut << "\n\n";
	} else {
		std::cerr << "Could not create ply file." << std::endl;
		return false;
	}
	fileOut.close();
	return true;
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
	oss << "Vertices:\n---------\n";
	for (int i = 0; i < vertices.size(); i++) {
		oss << i << ": " << vertices[i].toString() << std::endl;
	}
	return oss.str();
}

std::string Mesh::trianglesString() {
	std::ostringstream oss;
	oss << "Triangles:\n----------\n";
	for (int i = 0; i < vertices.size(); i++) {
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