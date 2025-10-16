#include "mesh.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <fstream>
#include <sstream>

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<Triangle> &triangles) {
	this->vertices = vertices;
	this->triangles = triangles;
}

Mesh Mesh::sectionsHandler(std::ifstream &file) {
	// general attributes
	int vertNum = 0;
	int triNum = 0;
	std::string id = "";
	generalAttributesSection(file, vertNum, triNum, id);

	// vertices
	std::vector<Vertex> verts(vertNum);
	verticesSection(file, verts);

	// triangles
	std::vector<Triangle> tris(triNum);
	trianglesSection(file, tris);

	return Mesh(verts, tris);
}

void Mesh::generalAttributesSection(std::ifstream &file, int &vertNum,
									int &triNum, std::string &id) {
	std::string line = "";
	bool found = false;
	while (getCleanLine(file, line)) {
		if (line == "[GeneralAttributes]") {
			found = true;
			break;
		}
	}
	if (!found) {
		std::cerr << "Could not find section '" << "[VerticesColorsSection]"
				  << "'." << std::endl;
		exit(1);
	}
	while (getCleanLine(file, line) && !isWhitespace(line)) {
		std::vector<std::string> words;
		boost::split(words, line, boost::is_any_of(" "),
					 boost::token_compress_on);
		if (words.size() == 3) {
			if (words[0] == "MeshID") {
				id = words[2];
			} else if (words[0] == "NumVertex") {
				vertNum = stoi(words[2]);
			} else if (words[0] == "NumTriangle") {
				triNum = stoi(words[2]);
			}
		}
	}
}

void Mesh::verticesSection(std::ifstream &file, std::vector<Vertex> &vertices) {
	std::string line = "";
	bool found = false;
	while (getCleanLine(file, line)) {
		if (line == "[VerticesSection]") {
			found = true;
			break;
		}
	}
	if (!found) {
		std::cerr << "Could not find section '" << "[VerticesSection]" << "'."
				  << std::endl;
		exit(1);
	}
	getCleanLine(file, line);
	getCleanLine(file, line);
	int index = 0;
	while (getCleanLine(file, line) && !isWhitespace(line)) {
		std::vector<std::string> words;
		boost::split(words, line, boost::is_any_of(" ="),
					 boost::token_compress_on);
		if (index != stoi(words[0])) {
			std::cerr << "Vertex " << index << " not found." << std::endl;
			exit(1);
		}
		glm::vec3 pos(stof(words[1]), stof(words[2]), stof(words[3]));
		glm::vec3 normal(stof(words[4]), stof(words[5]), stof(words[6]));
		vertices[index] = Vertex(pos, normal, stoi(words[7]));
		index++;
	}
}

void Mesh::trianglesSection(std::ifstream &file,
							std::vector<Triangle> &triangles) {
	std::string line = "";
	bool found = false;
	while (getCleanLine(file, line)) {
		if (line == "[TrianglesSection]") {
			found = true;
			break;
		}
	}
	if (!found) {
		std::cerr << "Could not find section '" << "[TrianglesSection]" << "'."
				  << std::endl;
		exit(1);
	}
	getCleanLine(file, line);
	getCleanLine(file, line);
	int index = 0;
	while (getCleanLine(file, line) && !isWhitespace(line)) {
		std::vector<std::string> words;
		boost::split(words, line, boost::is_any_of(" ="),
					 boost::token_compress_on);
		if (index != stoi(words[0])) {
			std::cerr << "Triangle " << index << " not found." << std::endl;
			exit(1);
		}
		int tri[3] = {stoi(words[1]), stoi(words[2]), stoi(words[3])};
		triangles[index] = Triangle(tri, stoi(words[7]));
		index++;
	}
}

Mesh Mesh::importMesh(std::string filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "error: could not open file '" << filepath << "'"
				  << std::endl;
		exit(1);
	}
	Mesh h = sectionsHandler(file);
	file.close();
	return h;
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

bool Mesh::toPly(std::string filename) {
	std::string ext = ".ply";
	if (!(filename.length() >= ext.length() &&
		  filename.substr(filename.length() - ext.length()) == ext)) {
		filename = filename + ext;
	}
	std::ofstream fileOut(filename);
	if (fileOut.is_open()) {

		fileOut << "ply\nformat ascii 1.0\n";
		fileOut << "comment Ply file converted from proprietary mesh format\n";
		fileOut << "element vertex " << vertices.size() << "\n";
		fileOut << "property float x\nproperty float y\nproperty float "
				   "z\nproperty float nx\nproperty float ny\nproperty "
				   "float nz\n";
		fileOut << "element face " << triangles.size() << "\n";
		fileOut << "property list uchar int vertex_indices\n";
		fileOut << "end_header\n";

		for (int i = 0; i < vertices.size(); i++) {
			fileOut << vertices[i].toPly() << "\n";
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