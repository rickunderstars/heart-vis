#include "mesh.hpp"
#include "triangle.hpp"
#include "vertex.hpp"
#include <emscripten/bind.h>
#include <string>
#include <vector>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(heart_module) {
  // Registra i vector per poterli usare
  register_vector<Vertex>("VectorVertex");
  register_vector<Triangle>("VectorTriangle");

  class_<Vertex>("Vertex");

  class_<Triangle>("Triangle");

  class_<Mesh>("Mesh")
      .constructor<std::vector<Vertex> &, std::vector<Triangle> &>()
      .property("vertices", &Mesh::vertices)
      .property("triangles", &Mesh::triangles)
      .function("triangleFix", &Mesh::triangleFix)
      .function("fixNMEdges", &Mesh::fixNMEdges)
      .function("toObjString", &Mesh::toObjString)
      .function("toPlyString",
                optional_override([](Mesh &self, std::string quality) {
                  return self.toPlyString(quality);
                }))
      .function("toPlyStringDefault", optional_override([](Mesh &self) {
                  return self.toPlyString();
                }))
      .function("verticesString", &Mesh::verticesString)
      .function("trianglesString", &Mesh::trianglesString)
      .function("toString", &Mesh::toString)
      .class_function("simpleShape", &Mesh::simpleShape);
}