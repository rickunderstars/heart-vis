#include "mesh.hpp"
#include "triangle.hpp"
#include "utils.hpp"
#include "vertex.hpp"
#include <emscripten/bind.h>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <vector>

EMSCRIPTEN_BINDINGS(heart_module) {

	emscripten::function("importMesh", &importMesh);

	emscripten::value_object<glm::vec3>("vec3")
		.field("x", &glm::vec3::x)
		.field("y", &glm::vec3::y)
		.field("z", &glm::vec3::z);

	emscripten::class_<Vertex>("Vertex")
		.property("pos", &Vertex::pos)
		.property("normal", &Vertex::normal)
		.property("groupID", &Vertex::groupID)
		.property("unipolar", &Vertex::unipolar)
		.property("bipolar", &Vertex::bipolar)
		.property("LAT", &Vertex::LAT)
		.property("EML", &Vertex::EML)
		.property("ExtEML", &Vertex::ExtEML)
		.property("SCAR", &Vertex::SCAR)
		.property("nUnipolar", &Vertex::nUnipolar)
		.property("nBipolar", &Vertex::nBipolar)
		.property("nLAT", &Vertex::nLAT)
		.property("nEML", &Vertex::nEML)
		.property("nExtEML", &Vertex::nExtEML)
		.property("nSCAR", &Vertex::nSCAR);

	emscripten::register_vector<Vertex>("VertexVector");

	emscripten::class_<Triangle>("Triangle")
		.constructor<int, int, int, int>()
		.property("groupID", &Triangle::groupID);

	emscripten::register_vector<Triangle>("TriangleVector");

	emscripten::class_<Mesh>("Mesh")
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
		.class_function("simpleShape", &Mesh::simpleShape)
		.function("Float32ArrayOfVertices", &Mesh::Float32ArrayOfVertices)
		.function("Uint32ArrayOfTriangles", &Mesh::Uint32ArrayOfTriangles)
		.function("Float32ArrayOfTurboColors", &Mesh::Float32ArrayOfTurboColors)
		.function("Float32ArrayOfUnipolar", &Mesh::Float32ArrayOfUnipolar)
		.function("Float32ArrayOfBipolar", &Mesh::Float32ArrayOfBipolar)
		.function("Float32ArrayOfLAT", &Mesh::Float32ArrayOfLAT)
		.function("Int32ArrayOfEML", &Mesh::Int32ArrayOfEML)
		.function("Int32ArrayOfExtEML", &Mesh::Int32ArrayOfExtEML)
		.function("Int32ArrayOfSCAR", &Mesh::Int32ArrayOfSCAR);
}