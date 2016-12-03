#pragma once
#include <glm/glm.hpp>
#include <fbxsdk/fbxsdk.h>
#include <GL/glew.h>
#include "MyMesh.h"

class FBXMesh
{
public:
	FBXMesh();
	~FBXMesh();

	void Render();
	void ReadFile(const char* fileName);
	void WriteFile(const char* fileName);

private:
	bool invalid;
	GLuint vao, vbo, ebo;
	FbxManager* lSdkManager;
	FbxNode* lRootNode;
	int lPolygonVertexCount;
	int lPolygonCount;
	int indicesCount;

	void LoadNodeMesh(FbxNode* node, FbxAMatrix modelMatrix);
	void UpdateMeshBuffer();

	MyMesh myMesh;
	double minSca;
	glm::dvec3 maxP, minP, tra, sca;
	std::vector<glm::dvec3> vertexData;
	std::vector<GLuint> vertexIndices;

};
