#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

const string GENERAL_ATTRIBUTES = "[GeneralAttributes]";
const string GA_NUM_VERTEX = "NumVertex";
const string GA_NUM_TRIANGLES = "NumTriangle";
const string GA_MESH_ID = "MeshID";
const string VERTICES_START = "[VerticesSection]";
const string TRIANGLES_START = "[TrianglesSection]";
const string ATTRIBUTES_START = "[VerticesColorsSection]";
const int SECTIONS_NUM = 4;

bool isWhitespace(const string &str) {
	return all_of(str.begin(), str.end(),
				  [](unsigned char c) { return std::isspace(c); });
}

void printVector(vector<string> v) {
	for (int i = 0; i < v.size(); i++) {
		cout << v[i] << " // ";
	}
}

istream &getCleanLine(std::ifstream &file, std::string &line) {
	std::getline(file, line);
	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
	return file;
}

class vector3 {
  private:
	float x;
	float y;
	float z;

  public:
	vector3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	string toString() {
		ostringstream oss;
		oss << "[" << x << ", " << y << ", " << z << "]";
		return oss.str();
	}
};

class vertex {
  private:
	vector3 pos = vector3(0, 0, 0);
	vector3 normal = vector3(0, 0, 0);
	int groupID;

  public:
	vertex(vector3 pos, vector3 normal, int id) {
		this->pos = pos;
		this->normal = normal;
		this->groupID = id;
	}
	string toString() {
		ostringstream oss;
		oss << "{ pos: " << pos.toString() << " | normal: " << normal.toString()
			<< " | groupID: " << groupID << " }";
		return oss.str();
	}
};

class triangle {
  private:
	int vertices[3];
	int groupID;

  public:
	triangle(int vertices[3], int groupID) {
		for (int i = 0; i < 3; i++) {
			this->vertices[i] = vertices[i];
		}
		this->groupID = groupID;
	}

	string toString() {
		ostringstream oss;
		oss << "{ triangle: [" << vertices[0] << ", " << vertices[1] << ", "
			<< vertices[2] << "] | groupID: " << groupID << " }";
		return oss.str();
	}
};

class Mesh {
  private:
	string meshID = "";
	vector<vertex> vertices; // vertices(size, 0);
	vector<triangle> triangles;

	Mesh(vector<vertex> vertices, vector<triangle> triangles, string meshID) {}

	Mesh() {} // dummy constructor

	static Mesh sectionsHandler(ifstream &file) {
		int sections = 0;

		// general attributes
		int vertNum = 0;
		int triNum = 0;
		string id = "";

		// vertices
		vector<vertex> verts;

		string line = "";
		while (getCleanLine(file, line)) {
			boost::trim(line);
			if (line == VERTICES_START) {
				sections++;
			} else if (line == TRIANGLES_START) {
				sections++;
			} else if (line == ATTRIBUTES_START) {
				sections++;
			} else if (line == GENERAL_ATTRIBUTES) {
				sections++;
				generalAttributesSection(file, vertNum, triNum, id);
			}
		}
		if (sections < SECTIONS_NUM) {
			cerr << "Could not find all sections in mesh file\nExpected "
					"sections:\n- "
				 << ATTRIBUTES_START << "\n- " << VERTICES_START << "\n- "
				 << TRIANGLES_START << "\n- " << ATTRIBUTES_START << endl;
			exit(1);
		}
		return Mesh();
	}

	static void generalAttributesSection(ifstream &file, int &vertNum,
										 int &triNum, string &id) {
		string line = "";
		while (getCleanLine(file, line)) {
			vector<string> words;
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
			if (isWhitespace(line)) {
				break;
			}
		}
	}
	static void verticesSection(ifstream &file) { /*TODO*/ }
	static void trianglesSection(ifstream &file) { /*TODO*/ }
	static void verticesAttributesSection(ifstream &file) { /*TODO*/ }

  public:
	static Mesh importMesh(string filepath) {
		ifstream file(filepath);
		if (!file.is_open()) {
			cerr << "error: could not open file '" << filepath << "'" << endl;
			exit(1);
		}
		Mesh h = sectionsHandler(file);
		file.close();
		return h;
	}

	string getMeshID() { return meshID; }

	int getVertexNum() { return vertices.size(); }

	int getTriangleNum() { return triangles.size(); }

	string toString() { return "TODO"; }
};

int main() {
	Mesh msh = Mesh::importMesh("../assets/mesh/2-LA-FA.mesh");
	return 0;
}