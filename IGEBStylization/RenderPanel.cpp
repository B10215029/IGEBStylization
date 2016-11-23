#include "RenderPanel.h"
#include "ShaderUtility.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

RenderPanel::RenderPanel()
{
	modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
	viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1));
	projectionMatrix = glm::perspective(glm::radians(45.0f), (float)TEXTURE_WIDTH / (float)TEXTURE_HEIGHT, 0.1f, 1000.0f);
}

RenderPanel::~RenderPanel()
{
}

void RenderPanel::Initialize()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	drawSolid.program = loadProgram("./shader/DrawMesh.vert", "./shader/DrawMesh.frag");
	drawSolid.modelMatrixLocation = glGetUniformLocation(drawSolid.program, "model_matrix");
	drawSolid.viewMatrixLocation = glGetUniformLocation(drawSolid.program, "view_matrix");
	drawSolid.projectionMatrixLocation = glGetUniformLocation(drawSolid.program, "projection_matrix");
	drawSolid.colorLocation = glGetUniformLocation(drawSolid.program, "color");
	OpenMesh::IO::read_mesh(backgroundMesh, "meshA.obj");
	backgroundMesh.update_normals();
	backgroundMesh.invalid = true;
	OpenMesh::IO::read_mesh(exampleMesh, "meshB.obj");
	exampleMesh.update_normals();
	exampleMesh.invalid = true;
	OpenMesh::IO::read_mesh(targetMesh, "meshC.obj");
	targetMesh.update_normals();
	targetMesh.invalid = true;
}

void RenderPanel::Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void RenderPanel::Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(drawSolid.program);
	glUniformMatrix4fv(drawSolid.modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(drawSolid.viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(drawSolid.projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniform4fv(drawSolid.colorLocation, 1, glm::value_ptr(glm::vec4(1.0, 0.0, 0.0, 1.0)));
	backgroundMesh.Render();
	exampleMesh.Render();
	targetMesh.Render();
}

void RenderPanel::MouseDown(int x, int y, int button)
{

}

void RenderPanel::MouseUp(int x, int y, int button)
{

}

void RenderPanel::MouseMove(int x, int y)
{

}

void RenderPanel::MouseWheel(int x, int y, int delta)
{

}
