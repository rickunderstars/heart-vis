#include "mesh.hpp"
#include "triangle.hpp"
#include "utils.hpp"
#include "vertex.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <cstdint>
#include <emscripten/val.h>
#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <sstream>
#include <vector>

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<Triangle> &triangles) {
	this->vertices = vertices;
	this->triangles = triangles;
}

std::string Mesh::toObjString() {
	std::ostringstream oss;
	oss << "# Obj file converted from proprietary mesh format";
	oss << "\n\n############\n# vertices #\n############\n\n";
	for (int i = 0; i < vertices.size(); i++) {
		oss << vertices.at(i).toObj() << "\n";
	}
	oss << "\n\n\n############\n# triangles #\n############\n\n";
	for (int i = 0; i < triangles.size(); i++) {
		oss << triangles.at(i).toObj() << "\n";
	}
	return oss.str();
}

std::string Mesh::toPlyString(std::string quality) {
	std::ostringstream oss;

	oss << "ply\nformat ascii 1.0\n";
	oss << "comment Ply file converted from proprietary mesh format\n";
	oss << "element vertex " << vertices.size() << "\n";
	oss << "property float x\nproperty float y\nproperty float "
		   "z\n";
	oss << "property float quality\n";
	oss << "element face " << triangles.size() << "\n";
	oss << "property list uchar int vertex_indices\n";
	oss << "end_header\n";

	for (int i = 0; i < vertices.size(); i++) {
		oss << vertices.at(i).toPly(quality) << "\n";
	}
	for (int i = 0; i < triangles.size(); i++) {
		oss << triangles.at(i).toPly() << "\n";
	}

	return oss.str();
}

bool Mesh::triangleFix(int face, int oldVertex, int newVertex) {
	if (face < 0 || face >= triangles.size()) {
		std::cout << "Face " << face << " out of bounds (0-"
				  << triangles.size() - 1 << ")";
		return false;
	}
	int v = -1;
	for (int i = 0; i < 3; i++) {
		if (oldVertex == triangles.at(face).vertices.at(i)) {
			v = i;
		}
	}
	if (v < 0) {
		std::cout << "Vertex " << oldVertex << " is not in face " << face
				  << ": (" << triangles.at(face).vertices.at(0) << ", "
				  << triangles.at(face).vertices.at(1) << ", "
				  << triangles.at(face).vertices.at(2) << " )";
		return false;
	}
	if (newVertex < 0 || newVertex >= vertices.size()) {
		std::cout << "Vertex " << newVertex << " out of bounds (0-"
				  << vertices.size() - 1 << ")";
		return false;
	}

	triangles.at(face).vertices.at(v) = newVertex;

	return true;
}

void Mesh::fixNMEdges() {
	int healthy = 0;
	for (int i = 0; i < triangles.size(); i++) {
		if (triangles.at(i).groupID != -1000000) {
			healthy++;
		}
	}

	std::vector<Triangle> newTris(healthy);

	for (int i = 0, k = 0; i < triangles.size(); i++) {
		if (triangles.at(i).groupID != -1000000) {
			newTris.at(k) = Triangle(triangles.at(i).vertices);
			newTris.at(k).groupID = triangles.at(i).groupID;
			k++;
		}
	}

	triangles = newTris;
}

Mesh Mesh::simpleShape() {
	std::vector<Triangle> tri(2);
	tri.at(0) = Triangle(0, 1, 2);
	tri.at(1) = Triangle(0, 2, 3);

	std::vector<Vertex> vert(4);
	glm::vec3 v0(0, 0, 0);
	glm::vec3 v1(0, 2, 0);
	glm::vec3 v2(2, 2, 0);
	glm::vec3 v3(2, 0, 0);
	vert.at(0) = Vertex(v0);
	vert.at(1) = Vertex(v1);
	vert.at(2) = Vertex(v2);
	vert.at(3) = Vertex(v3);

	return Mesh(vert, tri);
}

void Mesh::calcQualitiesMinMax() {
	maxUnipolar = vertices.at(0).unipolar;
	maxBipolar = vertices.at(0).bipolar;
	maxLAT = vertices.at(0).LAT;
	maxEML = vertices.at(0).EML;
	maxExtEML = vertices.at(0).ExtEML;
	maxSCAR = vertices.at(0).SCAR;

	minUnipolar = vertices.at(0).unipolar;
	minBipolar = vertices.at(0).bipolar;
	minLAT = vertices.at(0).LAT;
	minEML = vertices.at(0).EML;
	minExtEML = vertices.at(0).ExtEML;
	minSCAR = vertices.at(0).SCAR;
	for (int i = 1; i < vertices.size(); i++) {
		if (vertices.at(i).unipolar > maxUnipolar &&
			vertices.at(i).unipolar != 0) {
			maxUnipolar = vertices.at(i).unipolar;
		}
		if (vertices.at(i).bipolar > maxBipolar &&
			vertices.at(i).bipolar != 0) {
			maxBipolar = vertices.at(i).bipolar;
		}
		if (vertices.at(i).LAT > maxLAT && vertices.at(i).LAT != 0) {
			maxLAT = vertices.at(i).LAT;
		}
		if (vertices.at(i).EML > maxEML && vertices.at(i).EML != 0) {
			maxEML = vertices.at(i).EML;
		}
		if (vertices.at(i).ExtEML > maxExtEML && vertices.at(i).ExtEML != 0) {
			maxExtEML = vertices.at(i).ExtEML;
		}
		if (vertices.at(i).SCAR > maxSCAR && vertices.at(i).SCAR != 0) {
			maxSCAR = vertices.at(i).SCAR;
		}

		if (vertices.at(i).unipolar < minUnipolar &&
			vertices.at(i).unipolar != 0) {
			minUnipolar = vertices.at(i).unipolar;
		}
		if (vertices.at(i).bipolar < minBipolar &&
			vertices.at(i).bipolar != 0) {
			minBipolar = vertices.at(i).bipolar;
		}
		if (vertices.at(i).LAT < minLAT && vertices.at(i).LAT != 0) {
			minLAT = vertices.at(i).LAT;
		}
		if (vertices.at(i).EML < minEML && vertices.at(i).EML != 0) {
			minEML = vertices.at(i).EML;
		}
		if (vertices.at(i).ExtEML < minExtEML && vertices.at(i).ExtEML != 0) {
			minExtEML = vertices.at(i).ExtEML;
		}
		if (vertices.at(i).SCAR < minSCAR && vertices.at(i).SCAR != 0) {
			minSCAR = vertices.at(i).SCAR;
		}
	}
}

float Mesh::normalizedVertexQuality(int vIndex, std::string quality) const {
	if (!checkQuality(quality)) {
		throw std::runtime_error("'" + quality + "' is not an attribute");
	}
	float delta;
	float min;
	float max;

	if (quality == "unipolar") {
		max = maxUnipolar;
		min = minUnipolar;
		delta = max - min != 0 ? max - min : 1;
		return (vertices.at(vIndex).unipolar - min) / delta;
	} else if (quality == "bipolar") {
		max = maxBipolar;
		min = minBipolar;
		delta = max - min != 0 ? max - min : 1;
		return (vertices.at(vIndex).bipolar - min) / delta;
	} else if (quality == "lat") {
		max = maxLAT;
		min = minLAT;
		delta = max - min != 0 ? max - min : 1;
		return (vertices.at(vIndex).LAT - min) / delta;
	} else if (quality == "eml") {
		max = maxEML;
		min = minEML;
		delta = max - min != 0 ? max - min : 1;
		return (vertices.at(vIndex).EML - min) / delta;
	} else if (quality == "exteml") {
		max = maxExtEML;
		min = minExtEML;
		delta = max - min != 0 ? max - min : 1;
		return (vertices.at(vIndex).ExtEML - min) / delta;
	} else if (quality == "scar") {
		max = maxSCAR;
		min = minSCAR;
		delta = max - min != 0 ? max - min : 1;
		return (vertices.at(vIndex).SCAR - min) / delta;
	}

	return 0;
}

emscripten::val Mesh::Float32ArrayOfVertices() const {
	std::vector<float> positions;
	positions.reserve(vertices.size() * 3);

	for (const auto &v : vertices) {
		positions.push_back(v.pos.x);
		positions.push_back(v.pos.y);
		positions.push_back(v.pos.z);
	}

	emscripten::val float32Array =
		emscripten::val::global("Float32Array").new_(positions.size());
	emscripten::val memory = emscripten::val::module_property("HEAPF32");

	float32Array.call<void>("set",
							emscripten::val(emscripten::typed_memory_view(
								positions.size(), positions.data())));

	return float32Array;
}

emscripten::val Mesh::Uint32ArrayOfTriangles() const {
	std::vector<uint32_t> indices;
	indices.reserve(triangles.size() * 3);

	for (const auto &t : triangles) {
		indices.push_back(static_cast<uint32_t>(t.vertices.at(0)));
		indices.push_back(static_cast<uint32_t>(t.vertices.at(1)));
		indices.push_back(static_cast<uint32_t>(t.vertices.at(2)));
	}

	emscripten::val uint32Array =
		emscripten::val::global("Uint32Array").new_(indices.size());

	uint32Array.call<void>("set", emscripten::val(emscripten::typed_memory_view(
									  indices.size(), indices.data())));

	return uint32Array;
}

emscripten::val Mesh::Float32ArrayOfUnipolar() const {
	std::vector<float> valuesArray;
	valuesArray.reserve(vertices.size());

	for (const auto &v : vertices) {
		valuesArray.push_back(v.unipolar);
	}

	emscripten::val float32Array =
		emscripten::val::global("Float32Array").new_(valuesArray.size());
	emscripten::val memory = emscripten::val::module_property("HEAPF32");

	float32Array.call<void>("set",
							emscripten::val(emscripten::typed_memory_view(
								valuesArray.size(), valuesArray.data())));

	return float32Array;
}

emscripten::val Mesh::Float32ArrayOfBipolar() const {
	std::vector<float> valuesArray;
	valuesArray.reserve(vertices.size());

	for (const auto &v : vertices) {
		valuesArray.push_back(v.bipolar);
	}

	emscripten::val float32Array =
		emscripten::val::global("Float32Array").new_(valuesArray.size());
	emscripten::val memory = emscripten::val::module_property("HEAPF32");

	float32Array.call<void>("set",
							emscripten::val(emscripten::typed_memory_view(
								valuesArray.size(), valuesArray.data())));

	return float32Array;
}

emscripten::val Mesh::Float32ArrayOfLAT() const {
	std::vector<float> valuesArray;
	valuesArray.reserve(vertices.size());

	for (const auto &v : vertices) {
		valuesArray.push_back(v.LAT);
	}

	emscripten::val float32Array =
		emscripten::val::global("Float32Array").new_(valuesArray.size());
	emscripten::val memory = emscripten::val::module_property("HEAPF32");

	float32Array.call<void>("set",
							emscripten::val(emscripten::typed_memory_view(
								valuesArray.size(), valuesArray.data())));

	return float32Array;
}

emscripten::val Mesh::Int32ArrayOfEML() const {
	std::vector<int> valuesArray;
	valuesArray.reserve(vertices.size());

	for (const auto &v : vertices) {
		valuesArray.push_back(v.EML);
	}

	emscripten::val int32Array =
		emscripten::val::global("Int32Array").new_(valuesArray.size());
	emscripten::val memory = emscripten::val::module_property("HEAPF32");

	int32Array.call<void>("set", emscripten::val(emscripten::typed_memory_view(
									 valuesArray.size(), valuesArray.data())));

	return int32Array;
}

emscripten::val Mesh::Int32ArrayOfExtEML() const {
	std::vector<int> valuesArray;
	valuesArray.reserve(vertices.size());

	for (const auto &v : vertices) {
		valuesArray.push_back(v.ExtEML);
	}

	emscripten::val int32Array =
		emscripten::val::global("Int32Array").new_(valuesArray.size());
	emscripten::val memory = emscripten::val::module_property("HEAPF32");

	int32Array.call<void>("set", emscripten::val(emscripten::typed_memory_view(
									 valuesArray.size(), valuesArray.data())));

	return int32Array;
}

emscripten::val Mesh::Int32ArrayOfSCAR() const {
	std::vector<int> valuesArray;
	valuesArray.reserve(vertices.size());

	for (const auto &v : vertices) {
		valuesArray.push_back(v.SCAR);
	}

	emscripten::val int32Array =
		emscripten::val::global("Int32Array").new_(valuesArray.size());
	emscripten::val memory = emscripten::val::module_property("HEAPF32");

	int32Array.call<void>("set", emscripten::val(emscripten::typed_memory_view(
									 valuesArray.size(), valuesArray.data())));

	return int32Array;
}
emscripten::val Mesh::Int32ArrayOfGroupID() const {
	std::vector<int> valuesArray;
	valuesArray.reserve(vertices.size());

	for (const auto &v : vertices) {
		valuesArray.push_back(v.groupID);
	}

	emscripten::val int32Array =
		emscripten::val::global("Int32Array").new_(valuesArray.size());
	emscripten::val memory = emscripten::val::module_property("HEAPF32");

	int32Array.call<void>("set", emscripten::val(emscripten::typed_memory_view(
									 valuesArray.size(), valuesArray.data())));

	return int32Array;
}

emscripten::val Mesh::Float32ArrayOfTurboColors(std::string quality) const {
	if (!checkQuality(quality)) {
		throw std::runtime_error("'" + quality + "' is not an attribute");
	}

	std::vector<float> turboColors;
	turboColors.reserve(vertices.size() * 3);

	for (int i = 0; i < vertices.size(); i++) {
		std::array<float, 3> n =
			scalarToTurbo(normalizedVertexQuality(i, quality));
		turboColors.push_back(n.at(0));
		turboColors.push_back(n.at(1));
		turboColors.push_back(n.at(2));
	}

	emscripten::val float32Array =
		emscripten::val::global("Float32Array").new_(turboColors.size());
	emscripten::val memory = emscripten::val::module_property("HEAPF32");

	float32Array.call<void>("set",
							emscripten::val(emscripten::typed_memory_view(
								turboColors.size(), turboColors.data())));

	return float32Array;
}