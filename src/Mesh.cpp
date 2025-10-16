#include "Mesh.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <fstream>
#include <sstream>

const std::string GENERAL_ATTRIBUTES = "[GeneralAttributes]";
const std::string GA_NUM_VERTEX = "NumVertex";
const std::string GA_NUM_TRIANGLES = "NumTriangle";
const std::string GA_MESH_ID = "MeshID";
const std::string VERTICES_SECTION = "[VerticesSection]";
const std::string TRIANGLES_SECTION = "[TrianglesSection]";
const std::string VERTICES_COLORS_SECTION = "[VerticesColorsSection]";
const std::string VERTICES_ATTRIBUTES_SECTION = "[VerticesAttributesSection]";
const int SECTIONS_NUM = 4;

Mesh::Mesh(std::vector<vertex> &vertices, std::vector<triangle> &triangles,
		   std::string meshID) {
	this->meshID = meshID;
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
	std::vector<vertex> verts(vertNum);
	verticesSection(file, verts);

	// triangles
	std::vector<triangle> tris(triNum);
	trianglesSection(file, tris);

	return Mesh(verts, tris, id);
}

void Mesh::generalAttributesSection(std::ifstream &file, int &vertNum,
									int &triNum, std::string &id) {
	std::string line = "";
	bool found = false;
	while (getCleanLine(file, line)) {
		if (line == GENERAL_ATTRIBUTES) {
			found = true;
			break;
		}
	}
	if (!found) {
		std::cerr << "Could not find section '" << VERTICES_COLORS_SECTION
				  << "'." << std::endl;
		exit(1);
	}
	while (getCleanLine(file, line) && !isWhitespace(line)) {
		std::vector<std::string> words;
		boost::split(words, line, boost::is_any_of(" "),
					 boost::token_compress_on);
		if (words.size() == 3) {
			if (words[0] == GA_MESH_ID) {
				id = words[2];
			} else if (words[0] == GA_NUM_VERTEX) {
				vertNum = stoi(words[2]);
			} else if (words[0] == GA_NUM_TRIANGLES) {
				triNum = stoi(words[2]);
			}
		}
	}
}

void Mesh::verticesSection(std::ifstream &file, std::vector<vertex> &vertices) {
	std::string line = "";
	bool found = false;
	while (getCleanLine(file, line)) {
		if (line == VERTICES_SECTION) {
			found = true;
			break;
		}
	}
	if (!found) {
		std::cerr << "Could not find section '" << VERTICES_SECTION << "'."
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
		vector3 pos(stof(words[1]), stof(words[2]), stof(words[3]));
		vector3 normal(stof(words[4]), stof(words[5]), stof(words[6]));
		vertices[index] = vertex(pos, normal, stoi(words[7]));
		index++;
	}
}

void Mesh::trianglesSection(std::ifstream &file,
							std::vector<triangle> &triangles) {
	std::string line = "";
	bool found = false;
	while (getCleanLine(file, line)) {
		if (line == TRIANGLES_SECTION) {
			found = true;
			break;
		}
	}
	if (!found) {
		std::cerr << "Could not find section '" << TRIANGLES_SECTION << "'."
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
		triangles[index] = triangle(tri, stoi(words[7]));
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

std::string Mesh::getMeshID() { return meshID; }

int Mesh::getVertexNum() { return vertices.size(); }

int Mesh::getTriangleNum() { return triangles.size(); }

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
	oss << "MeshID: " << meshID
		<< "\n\n---------------------------------------------------------"
		   "\n\n"
		<< verticesString()
		<< "\n\n---------------------------------------------------------"
		   "\n\n"
		<< trianglesString()
		<< "\n\n---------------------------------------------------------"
		   "\n\n"
		<< "MeshID: " << meshID << std::endl;
	return oss.str();
}