#pragma once
#undef min
#undef max
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <GL/glew.h>

struct MyTraits : public OpenMesh::DefaultTraits
{
	VertexAttributes(OpenMesh::Attributes::Normal);
	FaceAttributes(OpenMesh::Attributes::Normal);
	typedef OpenMesh::Vec3d Point;
	typedef OpenMesh::Vec3d Normal;
};

class MyMesh : public OpenMesh::TriMesh_ArrayKernelT<MyTraits>
{
public:

	MyMesh();
	~MyMesh();
	void Render();
	void ReadFile(const char* fileName);
	void WriteFile(const char* fileName);

private:
	bool invalid;
	GLuint vao, vbo, ebo;

	void UpdateMeshBuffer();

};