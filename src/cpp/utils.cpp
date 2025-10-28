#include "utils.hpp"

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <ostream>

bool isWhitespace(const std::string &str) {
	return all_of(str.begin(), str.end(),
				  [](unsigned char c) { return isspace(c); });
}

void printVector(std::vector<std::string> v) {
	for (int i = 0; i < v.size(); i++) {
		std::cout << v[i] << " // ";
	}
}

std::istream &getCleanLine(std::ifstream &file, std::string &line) {
	getline(file, line);
	line.erase(remove(line.begin(), line.end(), '\r'), line.end());
	boost::trim(line);
	if (!line.empty() && line[0] == ';') {
		return getCleanLine(file, line);
	} else {
		return file;
	}
}

Mesh sectionsHandler(std::ifstream &file) {
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

	// vertices properties 1
	verticesColorsSection(file, verts);

	// vertices properties 2
	verticesAttributesSection(file, verts);

	return Mesh(verts, tris);
}

void generalAttributesSection(std::ifstream &file, int &vertNum, int &triNum,
							  std::string &id) {
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

void verticesSection(std::ifstream &file, std::vector<Vertex> &vertices) {
	std::string line = "";
	bool found = false;
	while (getCleanLine(file, line)) {
		if (line == "[VerticesSection]") {
			found = true;
			break;
		}
	}
	if (!found) {
		std::cerr << "Could not find section '" << "[VerticesSection]"
				  << "'." << std::endl;
		exit(1);
	}
	bool found_first = false;
	int index = 0;
	while (getCleanLine(file, line)) {

		std::vector<std::string> words;
		boost::split(words, line, boost::is_any_of(" ="),
					 boost::token_compress_on);

		if (!found_first && words[0] != "0") {
			continue;
		} else {
			found_first = true;
		}
		if (isWhitespace(line)) {
			break;
		}

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

void trianglesSection(std::ifstream &file, std::vector<Triangle> &triangles) {
	std::string line = "";
	bool found = false;
	while (getCleanLine(file, line)) {
		if (line == "[TrianglesSection]") {
			found = true;
			break;
		}
	}
	if (!found) {
		std::cerr << "Could not find section '" << "[TrianglesSection]"
				  << "'." << std::endl;
		exit(1);
	}

	bool found_first = false;
	int index = 0;
	while (getCleanLine(file, line)) {

		std::vector<std::string> words;
		boost::split(words, line, boost::is_any_of(" ="),
					 boost::token_compress_on);

		if (!found_first && words[0] != "0") {
			continue;
		} else {
			found_first = true;
		}
		if (isWhitespace(line)) {
			break;
		}

		if (index != stoi(words[0])) {
			std::cerr << "Triangle " << index << " not found." << std::endl;
			exit(1);
		}
		int tri[3] = {stoi(words[1]), stoi(words[2]), stoi(words[3])};
		triangles[index] = Triangle(tri, stoi(words[7]));
		index++;
	}
}

void verticesColorsSection(std::ifstream &file, std::vector<Vertex> &vertices) {
	std::string line = "";
	bool found = false;
	while (getCleanLine(file, line)) {
		if (line == "[VerticesColorsSection]") {
			found = true;
			break;
		}
	}
	if (!found) {
		std::cerr << "Could not find section '" << "[VerticesColorsSection]"
				  << "'." << std::endl;
		exit(1);
	}

	bool found_first = false;
	int index = 0;
	while (getCleanLine(file, line)) {

		std::vector<std::string> words;
		boost::split(words, line, boost::is_any_of(" ="),
					 boost::token_compress_on);

		if (!found_first && words[0] != "0") {
			continue;
		} else {
			found_first = true;
		}

		if (isWhitespace(line)) {
			break;
		}

		if (index != stoi(words[0])) {
			std::cerr << "Vertex colors at index " << index << " not found."
					  << std::endl;
			exit(1);
		}

		vertices[index].unipolar = stof(words[1]);
		vertices[index].bipolar = stof(words[2]);
		vertices[index].LAT = stof(words[3]);
		index++;
	}
}

void verticesAttributesSection(std::ifstream &file,
							   std::vector<Vertex> &vertices) {
	std::string line = "";
	bool found = false;
	while (getCleanLine(file, line)) {
		if (line == "[VerticesAttributesSection]") {
			found = true;
			break;
		}
	}
	if (!found) {
		std::cerr << "Could not find section '"
				  << "[VerticesAttributesSection]"
				  << "'." << std::endl;
		exit(1);
	}

	bool found_first = false;
	int index = 0;
	while (getCleanLine(file, line)) {

		std::vector<std::string> words;
		boost::split(words, line, boost::is_any_of(" ="),
					 boost::token_compress_on);

		if (!found_first && words[0] != "0") {
			continue;
		} else {
			found_first = true;
		}
		if (isWhitespace(line)) {
			break;
		}

		if (index != stoi(words[0])) {
			std::cerr << "Vertex attributes at index " << index << " not found."
					  << std::endl;
			exit(1);
		}

		vertices[index].EML = stoi(words[1]);
		vertices[index].ExtEML = stoi(words[2]);
		vertices[index].SCAR = stoi(words[3]);
		index++;
	}
}

Mesh importMesh(std::string filepath) {
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