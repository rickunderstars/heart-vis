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
		oss << vertices[i].posToObj() << "\n";
	}
	oss << "\n\n\n############\n# vertices normals #\n############\n\n";
	for (int i = 0; i < vertices.size(); i++) {
		oss << vertices[i].normalToObj() << "\n";
	}
	oss << "\n\n\n############\n# triangles #\n############\n\n";
	for (int i = 0; i < triangles.size(); i++) {
		oss << triangles[i].toObj() << "\n";
	}
	return oss.str();
}

std::string Mesh::toPlyString(std::string quality) {
	std::ostringstream oss;

	oss << "ply\nformat ascii 1.0\n";
	oss << "comment Ply file converted from proprietary mesh format\n";
	oss << "element vertex " << vertices.size() << "\n";
	oss << "property float x\nproperty float y\nproperty float "
		   "z\nproperty float nx\nproperty float ny\nproperty "
		   "float nz\n";
	oss << "property float quality\n";
	oss << "element face " << triangles.size() << "\n";
	oss << "property list uchar int vertex_indices\n";
	oss << "end_header\n";

	for (int i = 0; i < vertices.size(); i++) {
		oss << vertices[i].toPly(quality) << "\n";
	}
	for (int i = 0; i < triangles.size(); i++) {
		oss << triangles[i].toPly() << "\n";
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
		if (oldVertex == triangles[face].vertices[i]) {
			v = i;
		}
	}
	if (v < 0) {
		std::cout << "Vertex " << oldVertex << " is not in face " << face
				  << ": (" << triangles[face].vertices[0] << ", "
				  << triangles[face].vertices[1] << ", "
				  << triangles[face].vertices[2] << " )";
		return false;
	}
	if (newVertex < 0 || newVertex >= vertices.size()) {
		std::cout << "Vertex " << newVertex << " out of bounds (0-"
				  << vertices.size() - 1 << ")";
		return false;
	}

	triangles[face].vertices[v] = newVertex;

	return true;
}

void Mesh::fixNMEdges() {
	int healthy = 0;
	for (int i = 0; i < triangles.size(); i++) {
		if (triangles[i].groupID != -1000000) {
			healthy++;
		}
	}

	std::vector<Triangle> newTris(healthy);

	for (int i = 0, k = 0; i < triangles.size(); i++) {
		if (triangles[i].groupID != -1000000) {
			newTris[k] = Triangle(triangles[i].vertices, triangles[i].groupID);
			k++;
		}
	}

	triangles = newTris;
}

Mesh Mesh::simpleShape() {
	std::vector<Triangle> tri(2);
	int f0[3] = {0, 1, 2};
	int f1[3] = {0, 2, 3};
	tri[0] = Triangle(f0, -1);
	tri[1] = Triangle(f1, -1);

	std::vector<Vertex> vert(4);
	glm::vec3 nrm(0, 0, 1);
	glm::vec3 v0(0, 0, 0);
	glm::vec3 v1(0, 2, 0);
	glm::vec3 v2(2, 2, 0);
	glm::vec3 v3(2, 0, 0);
	vert[0] = Vertex(v0, nrm, -1);
	vert[1] = Vertex(v1, nrm, -1);
	vert[2] = Vertex(v2, nrm, -1);
	vert[3] = Vertex(v3, nrm, -1);

	return Mesh(vert, tri);
}

void Mesh::calcQualitiesMinMax() {
	maxUnipolar = vertices[0].unipolar;
	maxBipolar = vertices[0].bipolar;
	maxLAT = vertices[0].LAT;
	maxEML = vertices[0].EML;
	maxExtEML = vertices[0].ExtEML;
	maxSCAR = vertices[0].SCAR;

	minUnipolar = vertices[0].unipolar;
	minBipolar = vertices[0].bipolar;
	minLAT = vertices[0].LAT;
	minEML = vertices[0].EML;
	minExtEML = vertices[0].ExtEML;
	minSCAR = vertices[0].SCAR;
	for (int i = 1; i < vertices.size(); i++) {
		if (vertices[i].unipolar > maxUnipolar && vertices[i].unipolar != 0) {
			maxUnipolar = vertices[i].unipolar;
		}
		if (vertices[i].bipolar > maxBipolar && vertices[i].bipolar != 0) {
			maxBipolar = vertices[i].bipolar;
		}
		if (vertices[i].LAT > maxLAT && vertices[i].LAT != 0) {
			maxLAT = vertices[i].LAT;
		}
		if (vertices[i].EML > maxEML && vertices[i].EML != 0) {
			maxEML = vertices[i].EML;
		}
		if (vertices[i].ExtEML > maxExtEML && vertices[i].ExtEML != 0) {
			maxExtEML = vertices[i].ExtEML;
		}
		if (vertices[i].SCAR > maxSCAR && vertices[i].SCAR != 0) {
			maxSCAR = vertices[i].SCAR;
		}

		if (vertices[i].unipolar < minUnipolar && vertices[i].unipolar != 0) {
			minUnipolar = vertices[i].unipolar;
		}
		if (vertices[i].bipolar < minBipolar && vertices[i].bipolar != 0) {
			minBipolar = vertices[i].bipolar;
		}
		if (vertices[i].LAT < minLAT && vertices[i].LAT != 0) {
			minLAT = vertices[i].LAT;
		}
		if (vertices[i].EML < minEML && vertices[i].EML != 0) {
			minEML = vertices[i].EML;
		}
		if (vertices[i].ExtEML < minExtEML && vertices[i].ExtEML != 0) {
			minExtEML = vertices[i].ExtEML;
		}
		if (vertices[i].SCAR < minSCAR && vertices[i].SCAR != 0) {
			minSCAR = vertices[i].SCAR;
		}
	}
}

float standardize(int vIndex, std::string quality) { return 0; }

// to remove
void Mesh::calcQualitiesNorm() {
	float unipolarDelta =
		maxUnipolar - minUnipolar != 0 ? maxUnipolar - minUnipolar : 1;
	float bipolarDelta =
		maxBipolar - minBipolar != 0 ? maxBipolar - minBipolar : 1;
	float latDelta = maxLAT - minLAT != 0 ? maxLAT - minLAT : 1;
	float emlDelta = maxEML - minEML != 0 ? maxEML - minEML : 1;
	float extemlDelta = maxExtEML - minExtEML != 0 ? maxExtEML - minExtEML : 1;
	float scarDelta = maxSCAR - minSCAR != 0 ? maxSCAR - minSCAR : 1;
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].nUnipolar =
			(vertices[i].unipolar - minUnipolar) / unipolarDelta;
		vertices[i].nBipolar =
			(vertices[i].bipolar - minBipolar) / bipolarDelta;
		vertices[i].nLAT = (vertices[i].LAT - minLAT) / latDelta;
		vertices[i].nEML = (vertices[i].EML - minEML) / emlDelta;
		vertices[i].nExtEML = (vertices[i].ExtEML - minExtEML) / extemlDelta;
		vertices[i].nSCAR = (vertices[i].SCAR - minSCAR) / scarDelta;
	}
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
		indices.push_back(static_cast<uint32_t>(t.vertices[0]));
		indices.push_back(static_cast<uint32_t>(t.vertices[1]));
		indices.push_back(static_cast<uint32_t>(t.vertices[2]));
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
	std::vector<float> valuesArray;
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
	std::vector<float> valuesArray;
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
	std::vector<float> valuesArray;
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

emscripten::val Mesh::Float32ArrayOfTurboColors(std::string quality) const {
	if (!checkQuality(quality)) {
		throw std::runtime_error("'" + quality + "' is not an attribute");
	}

	std::vector<float> turboColors;
	turboColors.reserve(vertices.size() * 3);

	for (const auto &v : vertices) {
		if (quality == "unipolar") {
			std::array<float, 3> u = scalarToTurbo(v.nUnipolar);
			turboColors.push_back(u[0]);
			turboColors.push_back(u[1]);
			turboColors.push_back(u[2]);
		} else if (quality == "bipolar") {
			std::array<float, 3> b = scalarToTurbo(v.nBipolar);
			turboColors.push_back(b[0]);
			turboColors.push_back(b[1]);
			turboColors.push_back(b[2]);
		} else if (quality == "lat") {
			std::array<float, 3> l = scalarToTurbo(v.nLAT);
			turboColors.push_back(l[0]);
			turboColors.push_back(l[1]);
			turboColors.push_back(l[2]);
		} else if (quality == "eml") {
			std::array<float, 3> e = scalarToTurbo(v.nEML);
			turboColors.push_back(e[0]);
			turboColors.push_back(e[1]);
			turboColors.push_back(e[2]);
		} else if (quality == "exteml") {
			std::array<float, 3> ee = scalarToTurbo(v.nExtEML);
			turboColors.push_back(ee[0]);
			turboColors.push_back(ee[1]);
			turboColors.push_back(ee[2]);
		} else if (quality == "scar") {
			std::array<float, 3> s = scalarToTurbo(v.nSCAR);
			turboColors.push_back(s[0]);
			turboColors.push_back(s[1]);
			turboColors.push_back(s[2]);
		}
	}

	emscripten::val float32Array =
		emscripten::val::global("Float32Array").new_(turboColors.size());
	emscripten::val memory = emscripten::val::module_property("HEAPF32");

	float32Array.call<void>("set",
							emscripten::val(emscripten::typed_memory_view(
								turboColors.size(), turboColors.data())));

	return float32Array;
}