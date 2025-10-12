#include <fstream>
#include <iostream>

using namespace std;

const string VERTICES_START = "[VerticesSection]";
const string FACES_START = "[TrianglesSection]";
const string ATTRIBUTES_START = "[VerticesColorsSection]";
const string GENERAL_ATTRIBUTES = "[GeneralAttributes]";

struct vector3 {
	float x;
	float y;
	float z;
};

class Mesh {
  private:
	string content = "";

	Mesh(string text) { content = text; }

  public:
	static Mesh importMesh(string filepath) {
		ifstream file(filepath);
		if (!file.is_open()) {
			cerr << "error: could not open file '" << filepath << "'" << endl;
			exit(1);
		}
		string line = "";
		string data = "";
		while (getline(file, line)) {
			data += line;
		}
		return Mesh(data);
	}

	string toString() { return content; }
};

int main() {
	Mesh msh = Mesh::importMesh("../assets/mesh/2-LA-FA.mesh");
	cout << msh.toString() << endl;
	return 0;
}