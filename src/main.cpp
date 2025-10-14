#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const string GENERAL_ATTRIBUTES = "[GeneralAttributes]";
const string GA_NUM_VERTEX = "NumVertex";
const string GA_NUM_TRIANGLES = "NumTriangle";
const string GA_MESH_ID = "MeshID";
const string VERTICES_SECTION = "[VerticesSection]";
const string TRIANGLES_SECTION = "[TrianglesSection]";
const string VERTICES_COLORS_SECTION = "[VerticesColorsSection]";
const string VERTICES_ATTRIBUTES_SECTION = "[VerticesAttributesSection]";
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
	boost::trim(line);
	return file;
}

class vector3 {
  private:
	float x;
	float y;
	float z;

  public:
	vector3() : x(0), y(0), z(0) {}
	vector3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	string toString() {
		ostringstream oss;
		oss << "(" << x << ", " << y << ", " << z << ")";
		return oss.str();
	}
	string getX() {
		ostringstream oss;
		oss << x;
		return oss.str();
	}
	string getY() {
		ostringstream oss;
		oss << y;
		return oss.str();
	}
	string getZ() {
		ostringstream oss;
		oss << z;
		return oss.str();
	}
};

class vertex {
  private:
	vector3 pos;
	vector3 normal;
	int groupID;

  public:
	vertex() : pos(vector3(0, 0, 0)), normal(vector3(0, 0, 0)), groupID(-1) {}
	vertex(vector3 &pos, vector3 &normal, int id) {
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

	string posToObj() {
		ostringstream oss;
		oss << "v  " << pos.getX() << " " << pos.getY() << " " << pos.getZ();
		return oss.str();
	}

	string normalToObj() {
		ostringstream oss;
		oss << "vn " << normal.getX() << " " << normal.getY() << " "
			<< normal.getZ();
		return oss.str();
	}

	string ToPly() {
		ostringstream oss;
		oss << pos.getX() << " " << pos.getY() << " " << pos.getZ() << " "
			<< normal.getX() << " " << normal.getY() << " " << normal.getZ();
		return oss.str();
	}
};

class triangle {
  private:
	int vertices[3];
	int groupID;

  public:
	triangle() : vertices{0, 0, 0}, groupID(-1) {}
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

	string toObj() {
		ostringstream oss;
		oss << "f  " << vertices[0] << "//" << vertices[0] << " " << vertices[1]
			<< "//" << vertices[1] << " " << vertices[2] << "//" << vertices[2];
		return oss.str();
	}

	string toPly() {
		ostringstream oss;
		oss << 3 << " " << vertices[0] << " " << vertices[1] << " "
			<< vertices[2];
		return oss.str();
	}
};

class Mesh {
  private:
	string meshID = "";
	vector<vertex> vertices;
	vector<triangle> triangles;

	Mesh(vector<vertex> &vertices, vector<triangle> &triangles, string meshID) {
		this->meshID = meshID;
		this->vertices = vertices;
		this->triangles = triangles;
	}

	static Mesh sectionsHandler(ifstream &file) {
		// general attributes
		int vertNum = 0;
		int triNum = 0;
		string id = "";
		generalAttributesSection(file, vertNum, triNum, id);

		// vertices
		vector<vertex> verts(vertNum);
		verticesSection(file, verts);

		// triangles
		vector<triangle> tris(triNum);
		trianglesSection(file, tris);

		return Mesh(verts, tris, id);
	}

	static void generalAttributesSection(ifstream &file, int &vertNum,
										 int &triNum, string &id) {
		string line = "";
		bool found = false;
		while (getCleanLine(file, line)) {
			if (line == GENERAL_ATTRIBUTES) {
				found = true;
				break;
			}
		}
		if (!found) {
			cerr << "Could not find section '" << VERTICES_COLORS_SECTION
				 << "'." << endl;
			exit(1);
		}
		while (getCleanLine(file, line) && !isWhitespace(line)) {
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
		}
	}

	static void verticesSection(ifstream &file, vector<vertex> &vertices) {
		string line = "";
		bool found = false;
		while (getCleanLine(file, line)) {
			if (line == VERTICES_SECTION) {
				found = true;
				break;
			}
		}
		if (!found) {
			cerr << "Could not find section '" << VERTICES_SECTION << "'."
				 << endl;
			exit(1);
		}
		getCleanLine(file, line);
		getCleanLine(file, line);
		int index = 0;
		while (getCleanLine(file, line) && !isWhitespace(line)) {
			vector<string> words;
			boost::split(words, line, boost::is_any_of(" ="),
						 boost::token_compress_on);
			if (index != stoi(words[0])) {
				cerr << "Vertex " << index << " not found.";
				exit(1);
			}
			vector3 pos(stof(words[1]), stof(words[2]), stof(words[3]));
			vector3 normal(stof(words[4]), stof(words[5]), stof(words[6]));
			vertices[index] = vertex(pos, normal, stoi(words[7]));
			index++;
		}
	}

	static void trianglesSection(ifstream &file, vector<triangle> &triangles) {
		string line = "";
		bool found = false;
		while (getCleanLine(file, line)) {
			if (line == TRIANGLES_SECTION) {
				found = true;
				break;
			}
		}
		if (!found) {
			cerr << "Could not find section '" << TRIANGLES_SECTION << "'."
				 << endl;
			exit(1);
		}
		getCleanLine(file, line);
		getCleanLine(file, line);
		int index = 0;
		while (getCleanLine(file, line) && !isWhitespace(line)) {
			vector<string> words;
			boost::split(words, line, boost::is_any_of(" ="),
						 boost::token_compress_on);
			if (index != stoi(words[0])) {
				cerr << "Triangle " << index << " not found.";
				exit(1);
			}
			int tri[3] = {stoi(words[1]), stoi(words[2]), stoi(words[3])};
			triangles[index] = triangle(tri, stoi(words[7]));
			index++;
		}
	}

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

	string verticesString() {
		ostringstream oss;
		oss << "Vertices:" << endl << "---------" << endl;
		for (int i = 0; i < vertices.size(); i++) {
			oss << i << ": " << vertices[i].toString() << endl;
		}
		return oss.str();
	}

	string trianglesString() {
		ostringstream oss;
		oss << "Triangles:" << endl << "----------" << endl;
		for (int i = 0; i < vertices.size(); i++) {
			oss << i << ": " << triangles[i].toString() << endl;
		}
		return oss.str();
	}

	bool toObj(string filename) {
		string ext = ".obj";
		if (!(filename.length() >= ext.length() &&
			  filename.substr(filename.length() - ext.length()) == ext)) {
			filename = filename + ext;
		}
		ofstream fileOut(filename);
		if (fileOut.is_open()) {
			fileOut << "# Obj file converted from proprietary mesh format";
			fileOut << "\n\n############\n# vertices #\n############\n\n";
			for (int i = 0; i < vertices.size(); i++) {
				fileOut << vertices[i].posToObj() << "\n";
			}
			fileOut
				<< "\n\n\n############\n# vertices normals #\n############\n\n";
			for (int i = 0; i < vertices.size(); i++) {
				fileOut << vertices[i].normalToObj() << "\n";
			}
			fileOut << "\n\n\n############\n# triangles #\n############\n\n";
			for (int i = 0; i < triangles.size(); i++) {
				fileOut << triangles[i].toObj() << "\n";
			}
		} else {
			cerr << "Could not create obj file." << endl;
			return false;
		}
		fileOut.close();
		return true;
	}

	bool toPly(string filename) {
		string ext = ".ply";
		if (!(filename.length() >= ext.length() &&
			  filename.substr(filename.length() - ext.length()) == ext)) {
			filename = filename + ext;
		}
		ofstream fileOut(filename);
		if (fileOut.is_open()) {

			fileOut << "ply\nformat ascii 1.0\n";
			fileOut
				<< "comment Ply file converted from proprietary mesh format\n";
			fileOut << "element vertex " << vertices.size() << "\n";
			fileOut << "property float x\nproperty float y\nproperty float "
					   "z\nproperty float nx\nproperty float ny\nproperty "
					   "float nz\n";
			fileOut << "element face " << triangles.size() << "\n";
			fileOut << "property list uchar int vertex_indices\n";
			fileOut << "end_header\n";

			for (int i = 0; i < vertices.size(); i++) {
				fileOut << vertices[i].ToPly() << "\n";
			}
			for (int i = 0; i < triangles.size(); i++) {
				fileOut << triangles[i].toPly() << "\n";
			}
			fileOut << "\n\n";
		} else {
			cerr << "Could not create ply file." << endl;
			return false;
		}
		fileOut.close();
		return true;
	}

	string toString() {
		ostringstream oss;
		oss << "MeshID: " << meshID
			<< "\n\n---------------------------------------------------------"
			   "\n\n"
			<< verticesString()
			<< "\n\n---------------------------------------------------------"
			   "\n\n"
			<< trianglesString()
			<< "\n\n---------------------------------------------------------"
			   "\n\n"
			<< "MeshID: " << meshID << endl;
		return oss.str();
	}
};

int main() {

	string file1 = "../assets/mesh/2-LA.mesh";
	Mesh msh1 = Mesh::importMesh(file1);
	msh1.toObj(file1);
	msh1.toPly(file1);

	string file2 = "../assets/mesh/2-LA-FA.mesh";
	Mesh msh2 = Mesh::importMesh(file1);
	msh2.toObj(file2);
	msh2.toPly(file2);

	return 0;
}