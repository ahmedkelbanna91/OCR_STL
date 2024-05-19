#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> 
#include <limits>
#include <cstring>


struct Vector3 {
	float x, y, z;
	void scale(float Xscale, float Yscale, float Zscale, float zThreshold, float Xtopscale, float Ytopscale) {
		if (z > zThreshold) {
			x *= Xtopscale;
			y *= Ytopscale;
		}
		else {
			x *= Xscale;
			y *= Yscale;
		}
		z *= Zscale;
	}
};

struct Triangle {
	Vector3 normal;
	Vector3 vertices[3];
	void scale(float Xscale, float Yscale, float Zscale, float zThreshold, float Xtopscale, float Ytopscale) {
		for (int i = 0; i < 3; ++i) {
			vertices[i].scale(Xscale, Yscale, Zscale, zThreshold, Xtopscale, Ytopscale);
		}
	}
};

std::vector<Triangle> readSTLfile(const std::string& filename, float& modelWidth, float& modelLength, float& modelHeight) {
	std::ifstream file(filename, std::ios::binary);
	std::vector<Triangle> triangles;
	modelWidth = 0.0f;
	modelLength = 0.0f;
	modelHeight = 0.0f;

	if (!file) {
		std::cerr << "Failed to open " << filename << std::endl;
		return triangles;
	}

	char header[80];
	file.read(header, 80);

	unsigned int numTriangles;
	file.read(reinterpret_cast<char*>(&numTriangles), 4);

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();

	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();

	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::lowest();

	for (unsigned int i = 0; i < numTriangles; ++i) {
		Triangle triangle;
		file.read(reinterpret_cast<char*>(&triangle.normal), sizeof(Vector3));
		for (int j = 0; j < 3; ++j) {
			file.read(reinterpret_cast<char*>(&triangle.vertices[j]), sizeof(Vector3));
			minX = std::min(minX, triangle.vertices[j].x);
			maxX = std::max(maxX, triangle.vertices[j].x);
			minY = std::min(minY, triangle.vertices[j].y);
			maxY = std::max(maxY, triangle.vertices[j].y);
			minZ = std::min(minZ, triangle.vertices[j].z);
			maxZ = std::max(maxZ, triangle.vertices[j].z);
		}
		file.ignore(2);
		triangles.push_back(triangle);
	}

	modelWidth = maxX - minX;
	modelLength = maxY - minY;
	modelHeight = maxZ - minZ;

	return triangles;
}

void writeSTL(const std::string& filename, std::vector<Triangle>& triangles) {

	std::ofstream file(filename, std::ios::binary);
	if (!file) {
		std::cerr << "Failed to open " << filename << " for writing." << std::endl;
		return;
	}

	char header[80] = {};
	std::string headerStr = "V1.0 CreatedByBanna";
	std::memcpy(header, headerStr.c_str(), std::min(headerStr.size(), sizeof(header)));
	file.write(header, sizeof(header));

	unsigned int totalTriangles = triangles.size();
	file.write(reinterpret_cast<char*>(&totalTriangles), sizeof(totalTriangles));

	for (const auto& triangle : triangles) {
		file.write(reinterpret_cast<const char*>(&triangle.normal), sizeof(Vector3));
		for (int i = 0; i < 3; ++i) {
			file.write(reinterpret_cast<const char*>(&triangle.vertices[i]), sizeof(Vector3));
		}
		unsigned short attributeByteCount = 0;
		file.write(reinterpret_cast<char*>(&attributeByteCount), sizeof(attributeByteCount));
	}
	file.close();
}

void translateModel(std::vector<Triangle>& model, float dx, float dy, float dz) {
	for (auto& triangle : model) {
		for (auto& vertex : triangle.vertices) {
			vertex.x += dx;
			vertex.y += dy;
			vertex.z += dz;
		}
	}
}

int main(int argc, char* argv[]) {
	std::vector<Triangle> allTriangles;
	std::vector<std::string> filenames;
	bool lastWasDigit = false;
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float offsetZ = 0.0f;


	float Xscale = 0.18f;
	float Xtopscale = 0.18f;
	float Yscale = Xscale;
	float Ytopscale = Xtopscale;
	float Zscale = 0.30f;
	float zThreshold = 0.1f;
	float Xspacing = 0.8f;
	float Yspacing = 2.9f;
	float Xtranslate = -6.5f;
	float Ytranslate = -7.5f;
	float Ztranslate = 4.0f; //2.6f



	if (argc < 2) {
		std::cerr << "Usage: OCR_STL.exe <ID> [Depth] [XYscale] [XYtopscale] [Xspacing] [Yspacing]     (V1.0 CreatedByBanna)" << std::endl;
		return 1;
	}

	std::string text = argv[1];
	if (argc > 2) Ztranslate = 4.0f + std::atof(argv[2]);
	if (argc > 3) Yscale = Xscale = std::atof(argv[3]);
	if (argc > 4) Ytopscale = Xtopscale = std::atof(argv[4]);
	if (argc > 5) Xspacing = std::atof(argv[5]);
	if (argc > 6) Yspacing = std::atof(argv[6]);

	std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return std::toupper(c); });

	for (char c : text) {
		float modelWidth = 0.0f, modelLength = 0.0f, modelHeight = 0.0f;

		//std::string filename = "model/font/" + std::string(1, c) + ".stl";
		std::string filename = "font/" + std::string(1, c) + ".stl";
		auto TagTriangles = readSTLfile(filename, modelWidth, modelLength, modelHeight);

		if (std::isdigit(c)) {
			lastWasDigit = true;
		}
		else if (lastWasDigit) {
			offsetY -= (modelLength * Xscale) + Yspacing;
			offsetX = 0.15f;
			lastWasDigit = false;
		}

		for (auto& triangle : TagTriangles) {
			triangle.scale(Xscale, Yscale, Zscale, zThreshold, Xtopscale, Ytopscale);
			for (auto& vertex : triangle.vertices) {
				vertex.x += offsetX;
				vertex.y += offsetY;
				vertex.z += offsetZ;
			}
		}
		offsetX += (modelWidth * Xscale) + Xspacing;

		allTriangles.insert(allTriangles.end(), TagTriangles.begin(), TagTriangles.end());
	}

	translateModel(allTriangles, Xtranslate, Ytranslate, Ztranslate);

	//float fixtureWidth, fixtureLength, fixtureHeight;
	//auto FixtureTriangles = readSTLfile("model/fixture/fixture.stl", fixtureWidth, fixtureLength, fixtureHeight);
	//allTriangles.insert(allTriangles.end(), FixtureTriangles.begin(), FixtureTriangles.end());

	//writeSTL("model/fixture/tag.stl", allTriangles);
	writeSTL("tag.stl", allTriangles);

	return 0;
}
