
#include "mesh.hpp"
#include "triangle.hpp"
#include "utils.hpp"

int main() {

  // filepaths
  std::string base = "assets/mesh/";
  std::string filepath1 = base + "2-LA.mesh";
  std::string filepath2 = base + "2-LA-FA.mesh";
  std::string simplepath = base + "simple";

  // vector of qualities
  std::vector<std::string> qualities = {"unipolar", "bipolar", "lat",
                                        "eml",      "exteml",  "scar"};

  // from file to string
  std::string rawString1 = fileToString(filepath1);

  // from string to Mesh object
  Mesh msh1 = importMesh(rawString1);

  // mesh fixes (first vertices, faces and then edges, order is important!)
  msh1.triangleFix(8703, 4559, 4538);

  msh1.fixNMEdges();

  for (std::string q : qualities) {
    // from Mesh object to string, then from string to file
    // stringToFile(msh1.toPlyString(q), filepath1, "ply", q);
  }
  // stringToFile(msh1.toObjString(), filepath1, "obj");

  /// --- second mesh --- ///

  // from file to string
  std::string rawString2 = fileToString(filepath2);

  // from string to Mesh object
  Mesh msh2 = importMesh(rawString2);

  // mesh fixes (first vertices, faces and then edges, order is important!)
  msh2.triangleFix(25180, 12810, 12813);
  msh2.triangleFix(29108, 9930, 14703);
  msh2.triangleFix(21420, 10857, 10941);
  msh2.triangleFix(56, 38, 29);
  msh2.triangleFix(30812, 15492, 15447);
  msh2.triangleFix(30578, 14384, 14398);

  int face[3] = {15417, 14398, 14381};
  Triangle newTri(face, -1);
  msh2.triangles.push_back(newTri);

  msh2.fixNMEdges();

  for (std::string q : qualities) {
    // from Mesh object to string, then from string to file
    // stringToFile(msh2.toPlyString(q), filepath2, "ply", q);
  }
  // stringToFile(msh2.toObjString(), filepath2, "obj");

  /// --- simple mesh (square) --- ///

  Mesh smsh = Mesh::simpleShape();
  // stringToFile(smsh.toPlyString(), simplepath, "ply");

  std::cout << smsh.toPlyString() << std::endl;

  return 0;
}