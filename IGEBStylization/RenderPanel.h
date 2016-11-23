#pragma once
#include "OpenGLPanel.h"
#include "MyMesh.h"
#include <GL\glew.h>
#include <glm\glm.hpp>
#define TEXTURE_WIDTH 300
#define TEXTURE_HEIGHT 300

class RenderPanel : public OpenGLPanel
{
public:
	RenderPanel();
	~RenderPanel();
	void Initialize();
	void Reshape(int width, int height);
	void Display();
	void MouseDown(int x, int y, int button);
	void MouseUp(int x, int y, int button);
	void MouseMove(int x, int y);
	void MouseWheel(int x, int y, int delta);
	void UpdateMeshBuffer();

private:
	struct DrawSolidProgram {
		GLuint program;
		GLuint modelMatrixLocation;
		GLuint viewMatrixLocation;
		GLuint projectionMatrixLocation;
		GLuint colorLocation;
	} drawSolid;
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	MyMesh backgroundMesh;
	MyMesh exampleMesh;
	MyMesh targetMesh;

};
