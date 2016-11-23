#include "MyMesh.h"
#include <iostream>

MyMesh::MyMesh()
{
	vao = vbo = ebo = 0;
	invalid = true;
}

MyMesh::~MyMesh()
{

}

void MyMesh::Render()
{
	if (invalid) {
		UpdateMeshBuffer();
	}
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, n_faces() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MyMesh::UpdateMeshBuffer()
{
	if (vao)
		glDeleteVertexArrays(1, &vao);
	if (vbo)
		glDeleteBuffers(1, &vbo);
	if (ebo)
		glDeleteBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// add vertex position and normal to vbo
	GLdouble *vertexData = new GLdouble[n_vertices() * 6];
	for (MyMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it) {
		memcpy(vertexData + (v_it->idx() * 6), point(v_it.handle()).data(), sizeof(GLdouble) * 3);
		memcpy(vertexData + (v_it->idx() * 6 + 3), normal(v_it.handle()).data(), sizeof(GLdouble) * 3);
	}
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, n_vertices() * sizeof(GLdouble) * 6, vertexData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(GLdouble) * 6, 0);
	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, sizeof(GLdouble) * 6, (GLvoid*)(sizeof(GLdouble) * 3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	delete[] vertexData;
	// add face to ebo
	GLuint *vertexIndices = new GLuint[n_faces() * 3];
	for (MyMesh::FaceIter f_it = faces_begin(); f_it != faces_end(); ++f_it) {
		int i = 0;
		for (MyMesh::FaceVertexIter fv_it = fv_iter(f_it.handle()); fv_it.is_valid(); ++fv_it, ++i) {
			vertexIndices[f_it->idx() * 3 + i] = fv_it->idx();
		}
	}
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_faces() * sizeof(GLuint) * 3, vertexIndices, GL_STATIC_DRAW);
	delete[] vertexIndices;

	glBindVertexArray(0);
}
