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
	bool invalid;

	MyMesh();
	~MyMesh();
	void Render();

private:
	GLuint vao, vbo, ebo;

	void UpdateMeshBuffer();

};