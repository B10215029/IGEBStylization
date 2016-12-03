#pragma once
#include <glm/glm.hpp>
#include <fbxsdk/fbxsdk.h>
#include <GL/glew.h>

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
	FbxMesh* mesh;
	int lPolygonVertexCount;
	int lPolygonCount;
	int indicesCount;


	void UpdateMeshBuffer();

};
