#pragma once
#include "OpenGLPanel.h"
#include "MyMesh.h"
#include "FBXMesh.h"
#include <GL\glew.h>
#include <glm\glm.hpp>

class RenderPanel : public OpenGLPanel
{
public:
	static int fboSize;

	RenderPanel();
	~RenderPanel();
	void Initialize();
	void Reshape(int width, int height);
	void Display();
	void MouseDown(int x, int y, int button);
	void MouseUp(int x, int y, int button);
	void MouseMove(int x, int y);
	void MouseWheel(int x, int y, int delta);
	void LoadExampleModel(const char* fileName);
	void LoadTargetModel(const char* fileName);
	void LoadExampleImage(const char* fileName);

private:
	struct DrawSolidProgram {
		GLuint program;
		GLuint modelMatrixLocation;
		GLuint viewMatrixLocation;
		GLuint projectionMatrixLocation;
		GLuint colorLocation;
		GLuint AmbientLocation;
		GLuint LightColorLocation;
		GLuint lightPosLocation;
		GLuint viewPosLocation;
		GLuint ShininessLocation;
		GLuint StrengthLocation;
		GLuint isBlinnLocation;
	} drawSolid;
	struct DrawTextureProgram {
		GLuint program;
		GLuint textureLocation;
	} drawTexture;
	struct ShowResultProgram {
		GLuint program;
		GLuint texture0Location;
		GLuint texture1Location;
		GLuint texture2Location;
		GLuint texture3Location;
		GLuint texture4Location;
		GLuint texture5Location;
		GLuint texture6Location;
		GLuint texture7Location;
		GLuint texture8Location;
		GLuint texture9Location;
	} showResult;
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	MyMesh backgroundMesh;
	MyMesh exampleMesh;
	FBXMesh targetMesh;
	GLuint exaFBO;
	GLuint exaColTex[4];
	GLuint exaDephTex;
	GLuint tarFBO;
	GLuint tarColTex[4];
	GLuint tarDephTex;
	GLuint exampleTexture;
	GLuint resultTexture;
	GLuint focusTexture;
	glm::vec3 rotation;
	glm::ivec2 previousMousePosition;

	void CalculateResult();

};