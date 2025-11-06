#include "mesh.hpp"
#include "triangle.hpp"
#include "utils.hpp"
#include "vertex.hpp"
#include <emscripten/bind.h>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <vector>

EMSCRIPTEN_BINDINGS(heart_module) {
  emscripten::register_vector<Vertex>("VectorVertex");
  emscripten::register_vector<Triangle>("VectorTriangle");

  emscripten::function("importMesh", &importMesh);
  emscripten::class_<Vertex>("Vertex")
      .constructor<>()
      .constructor<glm::vec3 &, glm::vec3 &, int>();

  emscripten::class_<Triangle>("Triangle");

  emscripten::class_<Mesh>("Mesh")
      .constructor<std::vector<Vertex> &, std::vector<Triangle> &>()
      .property("vertices", &Mesh::vertices)
      .property("triangles", &Mesh::triangles)
      .function("triangleFix", &Mesh::triangleFix)
      .function("fixNMEdges", &Mesh::fixNMEdges)
      .function("toPlyString", emscripten::optional_override(
                                   [](Mesh &self, std::string quality) {
                                     return self.toPlyString(quality);
                                   }))
      .function("toPlyStringDefault",
                emscripten::optional_override(
                    [](Mesh &self) { return self.toPlyString(); }))
      .function("toString", &Mesh::toString)
      .class_function("simpleShape", &Mesh::simpleShape);
}