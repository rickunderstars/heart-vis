// #include <fstream>
#include <iostream>

using namespace std;

class Mesh {
  private:
	string content = "";

	Mesh(string text) { content = text; }

  public:
	static Mesh importMesh(string path) { return Mesh(path); }

	string toString() { return content; }
};

int main() {
	Mesh msh = Mesh::importMesh("../assets/mesh/2-LA-FA.mesh");
	cout << msh.toString() << endl;
	return 0;
}