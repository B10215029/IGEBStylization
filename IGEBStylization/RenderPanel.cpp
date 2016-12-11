#include "RenderPanel.h"
#include "ShaderUtility.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ctime>
#include <fstream>

#define MAX_LEVEL 6
#define OPTI_ITER 6
int RenderPanel::fboSize = 128;
RenderPanel* RenderPanel::singleton = NULL;

void RenderPanel::CalSingleton()
{
	singleton->CalculateResult();
}

RenderPanel::RenderPanel()
{
	rotation = glm::vec3(glm::radians(15.0f), 0, 0);
	modelMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
	viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1));
	projectionMatrix = glm::perspective(glm::radians(45.0f), (float)fboSize / (float)fboSize, 0.1f, 100.0f);
	focusTexture = 0;
	resultData = new GLubyte[fboSize * fboSize * 3];
}

RenderPanel::~RenderPanel()
{
}

void RenderPanel::Initialize()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	drawSolid.program = loadProgram("./shader/DrawMesh.vert", "./shader/DrawMesh.frag");
	drawSolid.modelMatrixLocation = glGetUniformLocation(drawSolid.program, "model_matrix");
	drawSolid.viewMatrixLocation = glGetUniformLocation(drawSolid.program, "view_matrix");
	drawSolid.projectionMatrixLocation = glGetUniformLocation(drawSolid.program, "projection_matrix");
	drawSolid.colorLocation = glGetUniformLocation(drawSolid.program, "color");
	drawSolid.AmbientLocation = glGetUniformLocation(drawSolid.program, "Ambient");
	drawSolid.LightColorLocation = glGetUniformLocation(drawSolid.program, "LightColor");
	drawSolid.lightPosLocation = glGetUniformLocation(drawSolid.program, "lightPos");
	drawSolid.viewPosLocation = glGetUniformLocation(drawSolid.program, "viewPos");
	drawSolid.ShininessLocation = glGetUniformLocation(drawSolid.program, "Shininess");
	drawSolid.StrengthLocation = glGetUniformLocation(drawSolid.program, "Strength");
	drawSolid.isBlinnLocation = glGetUniformLocation(drawSolid.program, "isBlinn");
	drawTexture.program = loadProgram("./shader/DrawTexture.vert", "./shader/DrawTexture.frag");
	drawTexture.textureLocation = glGetUniformLocation(drawTexture.program, "image");
	showResult.program = loadProgram("./shader/ShowResult.vert", "./shader/ShowResult.frag");
	showResult.texture0Location = glGetUniformLocation(showResult.program, "tex0");
	showResult.texture1Location = glGetUniformLocation(showResult.program, "tex1");
	showResult.texture2Location = glGetUniformLocation(showResult.program, "tex2");
	showResult.texture3Location = glGetUniformLocation(showResult.program, "tex3");
	showResult.texture4Location = glGetUniformLocation(showResult.program, "tex4");
	showResult.texture5Location = glGetUniformLocation(showResult.program, "tex5");
	showResult.texture6Location = glGetUniformLocation(showResult.program, "tex6");
	showResult.texture7Location = glGetUniformLocation(showResult.program, "tex7");
	showResult.texture8Location = glGetUniformLocation(showResult.program, "tex8");
	showResult.texture9Location = glGetUniformLocation(showResult.program, "tex9");

	GLenum drawbuff[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glGenFramebuffers(1, &exaFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, exaFBO);

	glGenTextures(4, exaColTex);
	glBindTexture(GL_TEXTURE_2D, exaColTex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fboSize, fboSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, exaColTex[0], 0);

	glBindTexture(GL_TEXTURE_2D, exaColTex[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fboSize, fboSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, exaColTex[1], 0);

	glBindTexture(GL_TEXTURE_2D, exaColTex[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fboSize, fboSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, exaColTex[2], 0);

	glBindTexture(GL_TEXTURE_2D, exaColTex[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fboSize, fboSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, exaColTex[3], 0);

	glGenRenderbuffers(1, &exaDephTex);
	glBindRenderbuffer(GL_RENDERBUFFER, exaDephTex);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fboSize, fboSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, exaDephTex);

	glDrawBuffers(4, drawbuff);
	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
		std::cout << "Framebuffer Error!" << std::endl;

	glGenFramebuffers(1, &tarFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, tarFBO);

	glGenTextures(4, tarColTex);
	glBindTexture(GL_TEXTURE_2D, tarColTex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fboSize, fboSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tarColTex[0], 0);

	glBindTexture(GL_TEXTURE_2D, tarColTex[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fboSize, fboSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tarColTex[1], 0);

	glBindTexture(GL_TEXTURE_2D, tarColTex[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fboSize, fboSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tarColTex[2], 0);

	glBindTexture(GL_TEXTURE_2D, tarColTex[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fboSize, fboSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, tarColTex[3], 0);

	glGenRenderbuffers(1, &tarDephTex);
	glBindRenderbuffer(GL_RENDERBUFFER, tarDephTex);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fboSize, fboSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, tarDephTex);

	glDrawBuffers(4, drawbuff);
	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
		std::cout << "Framebuffer Error!" << std::endl;

	glGenFramebuffers(1, &exaTexFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, exaTexFBO);

	glGenTextures(1, &exampleTexture);
	glBindTexture(GL_TEXTURE_2D, exampleTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fboSize, fboSize, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, exampleTexture, 0);

	glDrawBuffers(1, drawbuff);
	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
		std::cout << "Framebuffer Error!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	backgroundMesh.ReadFile("background.obj");
	exampleMesh.ReadFile("ball.obj");
	//targetMesh.ReadFile("mesh.obj");
	targetMesh.ReadFile("humanoid.fbx");
	//FBXMesh fbxmesh;
	//fbxmesh.ReadFile("humanoid.fbx");

	rawExampleTexture = loadTextureFromFilePNG("example.png");
	resultTexture = 0;
}

void RenderPanel::Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void RenderPanel::Display()
{
	glDeleteTextures(1, &resultTexture);
	glGenTextures(1, &resultTexture);
	glBindTexture(GL_TEXTURE_2D, resultTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fboSize, fboSize, 0, GL_RGB, GL_UNSIGNED_BYTE, resultData);
	glBindTexture(GL_TEXTURE_2D, 0);

	glViewport(0, 0, fboSize, fboSize);
	glUseProgram(drawSolid.program);
	glUniformMatrix4fv(drawSolid.modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(drawSolid.viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(drawSolid.projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniform3fv(drawSolid.LightColorLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glUniform3fv(drawSolid.lightPosLocation, 1, glm::value_ptr(glm::vec3(10, 20, 10)));
	glUniform3fv(drawSolid.viewPosLocation, 1, glm::value_ptr(glm::vec3(0, 0.20953750389, -0.97780061079)));
	glUniform1f(drawSolid.ShininessLocation, 64);
	glUniform1f(drawSolid.StrengthLocation, 1);
	glUniform1i(drawSolid.isBlinnLocation, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, exaFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniform3fv(drawSolid.AmbientLocation, 1, glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
	glUniform4fv(drawSolid.colorLocation, 1, glm::value_ptr(glm::vec4(1.0, 1.0, 1.0, 1.0)));
	backgroundMesh.Render();
	glUniform3fv(drawSolid.AmbientLocation, 1, glm::value_ptr(glm::vec3(0.2, 0.0, 0.0)));
	glUniform4fv(drawSolid.colorLocation, 1, glm::value_ptr(glm::vec4(1.0, 0.0, 0.0, 1.0)));
	exampleMesh.Render();

	glBindFramebuffer(GL_FRAMEBUFFER, tarFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniform3fv(drawSolid.AmbientLocation, 1, glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
	glUniform4fv(drawSolid.colorLocation, 1, glm::value_ptr(glm::vec4(1.0, 1.0, 1.0, 1.0)));
	backgroundMesh.Render();
	glUniform3fv(drawSolid.AmbientLocation, 1, glm::value_ptr(glm::vec3(0.2, 0.0, 0.0)));
	glUniform4fv(drawSolid.colorLocation, 1, glm::value_ptr(glm::vec4(1.0, 0.0, 0.0, 1.0)));
	targetMesh.Render();

	glBindFramebuffer(GL_FRAMEBUFFER, exaTexFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(drawTexture.program);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(drawTexture.textureLocation, 0);
	glBindTexture(GL_TEXTURE_2D, rawExampleTexture);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (focusTexture) {
		glUseProgram(drawTexture.program);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(drawTexture.textureLocation, 0);
		glBindTexture(GL_TEXTURE_2D, focusTexture);
	}
	else {
		glUseProgram(showResult.program);
		glUniform1i(showResult.texture0Location, 0);
		glUniform1i(showResult.texture1Location, 1);
		glUniform1i(showResult.texture2Location, 2);
		glUniform1i(showResult.texture3Location, 3);
		glUniform1i(showResult.texture4Location, 4);
		glUniform1i(showResult.texture5Location, 5);
		glUniform1i(showResult.texture6Location, 6);
		glUniform1i(showResult.texture7Location, 7);
		glUniform1i(showResult.texture8Location, 8);
		glUniform1i(showResult.texture9Location, 9);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, exaColTex[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, exaColTex[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, exaColTex[2]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, exaColTex[3]);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, tarColTex[0]);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, tarColTex[1]);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, tarColTex[2]);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, tarColTex[3]);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, exampleTexture);
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, resultTexture);
	}
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void RenderPanel::MouseDown(int x, int y, int button)
{
	if (button == 0) {
		float tx = ((float)x / width) * 4;
		float ty = (1 - (float)y / height) * 4;
		if (tx > 0 && tx < 1 && ty >3 && ty < 4)
			focusTexture = exaColTex[0];
		if (tx > 1 && tx < 2 && ty >3 && ty < 4)
			focusTexture = exaColTex[1];
		if (tx > 2 && tx < 3 && ty >3 && ty < 4)
			focusTexture = exaColTex[2];
		if (tx > 3 && tx < 4 && ty >3 && ty < 4)
			focusTexture = exaColTex[3];

		if (tx > 0 && tx < 1 && ty >0 && ty < 1)
			focusTexture = tarColTex[0];
		if (tx > 1 && tx < 2 && ty >0 && ty < 1)
			focusTexture = tarColTex[1];
		if (tx > 2 && tx < 3 && ty >0 && ty < 1)
			focusTexture = tarColTex[2];
		if (tx > 3 && tx < 4 && ty >0 && ty < 1)
			focusTexture = tarColTex[3];

		if (tx > 0 && tx < 2 && ty >1 && ty < 3)
			focusTexture = exampleTexture;
		if (tx > 2 && tx < 4 && ty >1 && ty < 3)
			focusTexture = resultTexture;
	}
	if (button == 1) {
		ResetView();
	}
	if (button == 2) {
		BindGL();
		writeTextureToFilePNG("exaColTex[0].png", exaColTex[0]);
		writeTextureToFilePNG("exaColTex[1].png", exaColTex[1]);
		writeTextureToFilePNG("exaColTex[2].png", exaColTex[2]);
		writeTextureToFilePNG("exaColTex[3].png", exaColTex[3]);
		writeTextureToFilePNG("tarColTex[0].png", tarColTex[0]);
		writeTextureToFilePNG("tarColTex[1].png", tarColTex[1]);
		writeTextureToFilePNG("tarColTex[2].png", tarColTex[2]);
		writeTextureToFilePNG("tarColTex[3].png", tarColTex[3]);
		writeTextureToFilePNG("exampleTexture.png", exampleTexture);
		writeTextureToFilePNG("resultTexture.png", resultTexture);
		ReleaseGL();
	}
	previousMousePosition.x = x;
	previousMousePosition.y = y;
}

void RenderPanel::MouseUp(int x, int y, int button)
{
	if (button == 0) {
		focusTexture = 0;
	}
}

void RenderPanel::MouseMove(int x, int y)
{
	if (isLMBDown) {
		rotation.x += glm::radians((float)(y - previousMousePosition.y));
		rotation.y += glm::radians((float)(x - previousMousePosition.x));
		modelMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
	}
	previousMousePosition.x = x;
	previousMousePosition.y = y;
}

void RenderPanel::MouseWheel(int x, int y, int delta)
{

}

inline double ColorLength(unsigned char* d)
{
	return (d[0] / 255.0f) * (d[0] / 255.0f) + (d[1] / 255.0f) * (d[1] / 255.0f) + (d[2] / 255.0f) * (d[2] / 255.0f);
}

inline double ColorDiff(unsigned char* a, unsigned char* b)
{
	unsigned char d[3];
	d[0] = a[0] > b[0] ? a[0] - b[0] : b[0] - a[0];
	d[1] = a[1] > b[1] ? a[1] - b[1] : b[1] - a[1];
	d[2] = a[2] > b[2] ? a[2] - b[2] : b[2] - a[2];
	return ColorLength(d);
}

double VectorLength(double* v, int d)
{
	double r = 0;
	for (int i = 0; i < d; i++) {
		r += v[i] * v[i];
	}
	return r;
}

template<typename T>
void ComputePyramids(T** data, int stride, int width, int height, int maxLevel)
{
	static const float weight[] = { 0.0453542, 0.0566406, 0.0453542, 0.0566406, 0.0707355, 0.0566406, 0.0453542, 0.0566406, 0.0453542 };
	static const int ox[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
	static const int oy[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
	static int tid = 0;
	int* levelWidth = new int[maxLevel];
	int* levelHeight = new int[maxLevel];
	levelWidth[0] = width;
	levelHeight[0] = height;
	for (int level = 1; level < maxLevel; level++) {
		levelWidth[level] = levelWidth[level - 1] / 2;
		levelHeight[level] = levelHeight[level - 1] / 2;
		data[level] = new T[levelWidth[level] * levelHeight[level] * stride];
		//printf("new %p\n", data[level]);
		for (int i = 0; i < levelWidth[level] * levelHeight[level]; i++) {
			float weightSum = 0;
			float* colorSum = new float[stride];
			for (int ch = 0; ch < stride; ch++) {
				colorSum[ch] = 0;
			}
			int mx = i % levelHeight[level] * 2;
			int my = i / levelHeight[level] * 2;
			for (int j = 0; j < 9; j++) {
				if ((mx + ox[j]) < 0 || (mx + ox[j]) >= levelWidth[level - 1] || (my + oy[j]) < 0 || (my + oy[j]) > levelHeight[level - 1])
					continue;
				for (int ch = 0; ch < stride; ch++) {
					colorSum[ch] += data[level - 1][((my + oy[j]) * levelWidth[level - 1] + (mx + ox[j])) * stride + ch] * weight[j];
				}
				weightSum += weight[j];
			}
			for (int ch = 0; ch < stride; ch++) {
				data[level][i * stride + ch] = colorSum[ch] / weightSum;
			}
			//memcpy(&data[level][i * stride], &data[level - 1][(i % levelHeight[level] * 2 + i / levelHeight[level] * 2 * levelHeight[level - 1]) * stride], sizeof(T) * stride);
		}
		char fn[100];
		sprintf(fn, "p%d.png", tid++);
		writeArrayToFilePNG(fn, data[level], levelWidth[level], levelHeight[level], stride);
	}
}

void RenderPanel::CalculateResult()
{
	//int pysize[6] = { fboSize, fboSize / 2, fboSize / 4 , fboSize / 8 , fboSize / 16 , fboSize / 32 };
	int* levelSize = new int[MAX_LEVEL];

	unsigned char* a[4][MAX_LEVEL];
	unsigned char* b[4][MAX_LEVEL];
	unsigned char* ap[MAX_LEVEL];
	unsigned char* bp[MAX_LEVEL];
	int* st = new int[fboSize * fboSize];
	//double* fv[MAX_LEVEL];

	// Get Image
	printf("Get Image\n");
	BindGL();
	a[0][0] = writeTextureToArray(exaColTex[0]);
	a[1][0] = writeTextureToArray(exaColTex[1]);
	a[2][0] = writeTextureToArray(exaColTex[2]);
	a[3][0] = writeTextureToArray(exaColTex[3]);
	b[0][0] = writeTextureToArray(tarColTex[0]);
	b[1][0] = writeTextureToArray(tarColTex[1]);
	b[2][0] = writeTextureToArray(tarColTex[2]);
	b[3][0] = writeTextureToArray(tarColTex[3]);
	ap[0] = writeTextureToArray(exampleTexture);
	bp[0] = (unsigned char*)malloc(sizeof(unsigned char) * fboSize * fboSize * 3);
	ReleaseGL();

	// Compute Features
	levelSize[0] = fboSize;
	for (int level = 1; level < MAX_LEVEL; level++) {
		levelSize[level] = levelSize[level - 1] / 2;
		//printf("levelSize[%d]: %d\n", level, levelSize[level]);
	//	fv[level] = new double[levelSize[level] * levelSize[level]];
	//	double f[4];
	//	for (int i = 0; i < levelSize[level] * levelSize[level]; i++) {
	//		f[0] = ColorLength(&a[0][level][i * 3]);
	//		f[1] = ColorLength(&a[1][level][i * 3]);
	//		f[2] = ColorLength(&a[2][level][i * 3]);
	//		f[3] = ColorLength(&a[3][level][i * 3]);
	//		fv[level][i] = VectorLength(f, 4);
	//	}
	}

	// Compute Gaussian pyramids
	//printf("Compute Gaussian pyramids%p, %p, %p, %p\n", a[0], a[1], a[2], a[3]);
	ComputePyramids<unsigned char>(a[0], 3, fboSize, fboSize, MAX_LEVEL);
	ComputePyramids<unsigned char>(a[1], 3, fboSize, fboSize, MAX_LEVEL);
	ComputePyramids<unsigned char>(a[2], 3, fboSize, fboSize, MAX_LEVEL);
	ComputePyramids<unsigned char>(a[3], 3, fboSize, fboSize, MAX_LEVEL);
	ComputePyramids<unsigned char>(b[0], 3, fboSize, fboSize, MAX_LEVEL);
	ComputePyramids<unsigned char>(b[1], 3, fboSize, fboSize, MAX_LEVEL);
	ComputePyramids<unsigned char>(b[2], 3, fboSize, fboSize, MAX_LEVEL);
	ComputePyramids<unsigned char>(b[3], 3, fboSize, fboSize, MAX_LEVEL);
	ComputePyramids<unsigned char>(ap, 3, fboSize, fboSize, MAX_LEVEL);
	ComputePyramids<unsigned char>(bp, 3, fboSize, fboSize, MAX_LEVEL);
	//ComputePyramids<double>(fv, 1, fboSize, fboSize, MAX_LEVEL);

	// Compute Result
	printf("Compute Result\n");
	//for (int level = MAX_LEVEL - 1; level >= MAX_LEVEL - 1; level--) {
	//	printf("level %d, size %d\n", level, levelSize[level]);
	//	for (int i = 0; i < levelSize[level] * levelSize[level]; i++) {
	//		int minIndex;
	//		double minDis = FLT_MAX;
	//		for (int j = 0; j < levelSize[level] * levelSize[level]; j++) {
	//			double f[4], fl;
	//			f[0] = ColorDiff(&a[0][level][j * 3], &b[0][level][i * 3]);
	//			f[1] = ColorDiff(&a[1][level][j * 3], &b[1][level][i * 3]);
	//			f[2] = ColorDiff(&a[2][level][j * 3], &b[2][level][i * 3]);
	//			f[3] = ColorDiff(&a[3][level][j * 3], &b[3][level][i * 3]);
	//			fl = VectorLength(f, 4);
	//			if (fl < minDis || (fl - minDis < 1e-100 && rand() > RAND_MAX / 2)) {
	//				minDis = fl;
	//				minIndex = j;
	//			}
	//		}
	//		memcpy(&bp[level][i * 3], &ap[level][minIndex * 3], sizeof(unsigned char) * 3);
	//		int x = i % levelSize[level];
	//		int y = i / levelSize[level];
	//		int minX = (double)x / levelSize[level] * fboSize;
	//		int minY = (double)y / levelSize[level] * fboSize;
	//		int maxX = (double)(x + 1) / levelSize[level] * fboSize;
	//		int maxY = (double)(y + 1) / levelSize[level] * fboSize;
	//		for (int xx = minX; xx < maxX; xx++) {
	//			for (int yy = minY; yy < maxY; yy++) {
	//				memcpy(&resultData[(yy * fboSize + xx) * 3], &bp[level][i * 3], sizeof(unsigned char) * 3);
	//			}
	//		}
	//	}
	//}
	std::ofstream debugFile("debug.txt");
	for (int level = MAX_LEVEL - 1; level >= 0; level--) {
		printf("level %d, size %d\n", level, levelSize[level]);
		// for each pixel in b
		for (int y = 0; y < levelSize[level]; ++y) {
			for (int x = 0; x < levelSize[level]; ++x) {
				int minIndex; // close pixel index in a
				double minDis = DBL_MAX; // close distance in a
				// for each pixel in a
				for (int ay = 0; ay < levelSize[level]; ++ay) {
					for (int ax = 0; ax < levelSize[level]; ++ax) {
						double f[4], fl = 0; // distance of pixel in b
						int pCount = 0; // valid pixel in range
						// evaluate at 5x5 range
						for (int offsetY = 0; offsetY <= 0; ++offsetY) {
							for (int offsetX = 0; offsetX <= 0; ++offsetX) {
						//for (int offsetY = -2; offsetY <= 2; ++offsetY) {
						//	for (int offsetX = -2; offsetX <= 2; ++offsetX) {
								// compuate pixel position in a and b
								int oax = offsetX + ax, oay = offsetY + ay;
								int obx = offsetX + x, oby = offsetY + y;
								//oax = (oax + levelSize[level]) % levelSize[level];
								//oay = (oay + levelSize[level]) % levelSize[level];
								//obx = (obx + levelSize[level]) % levelSize[level];
								//oby = (oby + levelSize[level]) % levelSize[level];
								// check pixel is in the range of image
								if (oax < 0 || oax >= levelSize[level] ||
									oay < 0 || oay >= levelSize[level] ||
									obx < 0 || obx >= levelSize[level] ||
									oby < 0 || oby >= levelSize[level])
									continue;
								// compute distance of image 0(full color), image 1(ambient), image 2(diffuse), image 3(specular)
								f[0] = ColorDiff(&a[0][level][(oay * levelSize[level] + oax) * 3], &b[0][level][(oby * levelSize[level] + obx) * 3]);
								f[1] = ColorDiff(&a[1][level][(oay * levelSize[level] + oax) * 3], &b[1][level][(oby * levelSize[level] + obx) * 3]);
								f[2] = ColorDiff(&a[2][level][(oay * levelSize[level] + oax) * 3], &b[2][level][(oby * levelSize[level] + obx) * 3]);
								f[3] = ColorDiff(&a[3][level][(oay * levelSize[level] + oax) * 3], &b[3][level][(oby * levelSize[level] + obx) * 3]);
								fl += VectorLength(f, 4);
								pCount++;
							}
						}
						fl /= pCount;
						if (fl < minDis || fl - minDis < 1e-10 && rand() < RAND_MAX / 100 ) {
							minDis = fl;
							minIndex = ay * levelSize[level] + ax;
						}
						//debugFile << "(" << ax << ", " << ay << "): " << fl << std::endl;
					}
				}
				//debugFile << minDis << std::endl;
				// write result to bp
				memcpy(&bp[level][(y * levelSize[level] + x) * 3], &ap[level][minIndex * 3], sizeof(unsigned char) * 3);
				// fill result to resultData for show
				int minX = (double)x / levelSize[level] * fboSize;
				int minY = (double)y / levelSize[level] * fboSize;
				int maxX = (double)(x + 1) / levelSize[level] * fboSize;
				int maxY = (double)(y + 1) / levelSize[level] * fboSize;
				for (int ry = minY; ry < maxY; ry++) {
					for (int rx = minX; rx < maxX; rx++) {
						memcpy(&resultData[(ry * fboSize + rx) * 3], &bp[level][(y * levelSize[level] + x) * 3], sizeof(unsigned char) * 3);
					}
				}
			}
		}

		for (int opti = 0; opti < OPTI_ITER; opti++) {
			printf("Optimization iterations %d\n", opti);
			// for each pixel in b
			for (int y = 0; y < levelSize[level]; ++y) {
				for (int x = 0; x < levelSize[level]; ++x) {
					int minIndex; // close pixel index in a
					double minDis = DBL_MAX; // close distance in a
					// for each pixel in a
					for (int ay = 0; ay < levelSize[level]; ++ay) {
						for (int ax = 0; ax < levelSize[level]; ++ax) {
							double f[5], fl = 0; // distance of pixel in b
							int pCount = 0; // valid pixel in range
							// evaluate at 5x5 range
							for (int offsetY = -2; offsetY <= 2; ++offsetY) {
								for (int offsetX = -2; offsetX <= 2; ++offsetX) {
									// compuate pixel position in a and b
									int oax = offsetX + ax, oay = offsetY + ay;
									int obx = offsetX + x, oby = offsetY + y;
									//oax = (oax + levelSize[level]) % levelSize[level];
									//oay = (oay + levelSize[level]) % levelSize[level];
									//obx = (obx + levelSize[level]) % levelSize[level];
									//oby = (oby + levelSize[level]) % levelSize[level];
									// check pixel is in the range of image
									if (oax < 0 || oax >= levelSize[level] ||
										oay < 0 || oay >= levelSize[level] ||
										obx < 0 || obx >= levelSize[level] ||
										oby < 0 || oby >= levelSize[level])
										continue;
									// compute distance of image 0(full color), image 1(ambient), image 2(diffuse), image 3(specular)
									f[0] = ColorDiff(&a[0][level][(oay * levelSize[level] + oax) * 3], &b[0][level][(oby * levelSize[level] + obx) * 3]);
									f[1] = ColorDiff(&a[1][level][(oay * levelSize[level] + oax) * 3], &b[1][level][(oby * levelSize[level] + obx) * 3]);
									f[2] = ColorDiff(&a[2][level][(oay * levelSize[level] + oax) * 3], &b[2][level][(oby * levelSize[level] + obx) * 3]);
									f[3] = ColorDiff(&a[3][level][(oay * levelSize[level] + oax) * 3], &b[3][level][(oby * levelSize[level] + obx) * 3]);
									f[4] = ColorDiff(&ap[level][(oay * levelSize[level] + oax) * 3], &bp[level][(oby * levelSize[level] + obx) * 3]);
									fl += VectorLength(f, 5);
									pCount++;
								}
							}
							fl /= pCount;
							if (fl < minDis || fl - minDis < 1e-10 && rand() < RAND_MAX / 100) {
								minDis = fl;
								minIndex = ay * levelSize[level] + ax;
							}
							//debugFile << "(" << ax << ", " << ay << "): " << fl << std::endl;
						}
					}
					//debugFile << minDis << std::endl;
					// write result to bp
					st[y * levelSize[level] + x] = minIndex;
					memcpy(&bp[level][(y * levelSize[level] + x) * 3], &ap[level][minIndex * 3], sizeof(unsigned char) * 3);
					// fill result to resultData for show
					int minX = (double)x / levelSize[level] * fboSize;
					int minY = (double)y / levelSize[level] * fboSize;
					int maxX = (double)(x + 1) / levelSize[level] * fboSize;
					int maxY = (double)(y + 1) / levelSize[level] * fboSize;
					for (int ry = minY; ry < maxY; ry++) {
						for (int rx = minX; rx < maxX; rx++) {
							memcpy(&resultData[(ry * fboSize + rx) * 3], &bp[level][(y * levelSize[level] + x) * 3], sizeof(unsigned char) * 3);
						}
					}
				}
			}

			// for each pixel in b
			for (int y = 0; y < levelSize[level]; ++y) {
				for (int x = 0; x < levelSize[level]; ++x) {
					int colorSumR = 0;
					int colorSumG = 0;
					int colorSumB = 0;
					int colorCount = 0;
					for (int offsetY = -2; offsetY <= 2; ++offsetY) {
						for (int offsetX = -2; offsetX <= 2; ++offsetX) {
							// compuate pixel position in a and b
							int obx = offsetX + x, oby = offsetY + y;
							if (obx < 0 || obx >= levelSize[level] || oby < 0 || oby >= levelSize[level])
								continue;

							int oax = st[obx + oby * levelSize[level]] % levelSize[level] - offsetX, oay = st[obx + oby * levelSize[level]] / levelSize[level] - offsetY;
							if (oax < 0 || oax >= levelSize[level] || oay < 0 || oay >= levelSize[level])
								continue;
							int oai = oax + oay * levelSize[level];
							colorSumR += ap[level][oai * 3 + 0];
							colorSumG += ap[level][oai * 3 + 1];
							colorSumB += ap[level][oai * 3 + 2];
							colorCount++;
						}
					}
					bp[level][(y * levelSize[level] + x) * 3 + 0] = colorSumR / colorCount;
					bp[level][(y * levelSize[level] + x) * 3 + 1] = colorSumG / colorCount;
					bp[level][(y * levelSize[level] + x) * 3 + 2] = colorSumB / colorCount;



					// fill result to resultData for show
					int minX = (double)x / levelSize[level] * fboSize;
					int minY = (double)y / levelSize[level] * fboSize;
					int maxX = (double)(x + 1) / levelSize[level] * fboSize;
					int maxY = (double)(y + 1) / levelSize[level] * fboSize;
					for (int ry = minY; ry < maxY; ry++) {
						for (int rx = minX; rx < maxX; rx++) {
							memcpy(&resultData[(ry * fboSize + rx) * 3], &bp[level][(y * levelSize[level] + x) * 3], sizeof(unsigned char) * 3);
						}
					}
				}


			}



		}

	}
	printf("OK\n");

	//for (int level = 0; level < MAX_LEVEL; level++) {
	//	int p, q;
	//	ColorDiff(&ap[level][p * 3], &bp[level][q * 3]);

	//	ColorDiff(&a[0][level][p * 3], &b[0][level][q * 3]);
	//	ColorDiff(&a[1][level][p * 3], &b[1][level][q * 3]);
	//	ColorDiff(&a[2][level][p * 3], &b[2][level][q * 3]);
	//	ColorDiff(&a[3][level][p * 3], &b[3][level][q * 3]);
	//}




	// Compute Result(knn)
	//int process = 0, size = fboSize * fboSize;
	//for (int i = 0; i < fboSize * fboSize; i++) {
	//	if (i * 100 / size > process) {
	//		process = i * 100 / size;
	//		//printf("%d%%\n", process);
	//		if (process % 10)
	//			putc('-', stdout);
	//		else
	//			putc('|', stdout);
	//		//putc('0' + process / 10, stdout);
	//	}
	//	double f[4], fl, minDis = FLT_MAX;
	//	int minIndex;
	//	f[0] = ColorLength(&tc[0][i * 3]);
	//	f[1] = ColorLength(&tc[1][i * 3]);
	//	f[2] = ColorLength(&tc[2][i * 3]);
	//	f[3] = ColorLength(&tc[3][i * 3]);
	//	fl = VectorLength(f, 4);
	//	for (int j = 0; j < fboSize * fboSize; j++) {
	//		if (abs(exampleFeature[j] - fl) < minDis) {
	//			minDis = abs(exampleFeature[j] - fl);
	//			minIndex = j;
	//		}
	//	}
	//	r[i * 3 + 0] = e[minIndex * 3 + 0];
	//	r[i * 3 + 1] = e[minIndex * 3 + 1];
	//	r[i * 3 + 2] = e[minIndex * 3 + 2];
	//	memcpy(&resultData[i * 3], &r[i * 3], sizeof(GLubyte) * 3);
	//}
	//printf("|\nDone\n");


	std::time_t time = std::time(NULL);
	char fileName[100], mbsrt[100];
	std::strftime(mbsrt, sizeof(mbsrt), "%Y%m%d%H%M%S", std::localtime(&time));
	sprintf(fileName, "result_%s.png", mbsrt);
	writeArrayToFilePNG(fileName, resultData, fboSize, fboSize, 3);
}

void RenderPanel::LoadExampleModel(const char* fileName)
{
	exampleMesh.ReadFile(fileName);
}

void RenderPanel::LoadTargetModel(const char* fileName)
{
	targetMesh.ReadFile(fileName);
}

void RenderPanel::LoadExampleImage(const char* fileName)
{
	rawExampleTexture = loadTextureFromFilePNG(fileName);
}

void RenderPanel::StartCalculate()
{
	//CalculateResult();
	singleton = this;
	System::Threading::Thread^ calThread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(CalSingleton));
	calThread->Start();
}

void RenderPanel::ResetView()
{
	rotation = glm::vec3(glm::radians(15.0f), 0, 0);
	modelMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
	viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1));
	projectionMatrix = glm::perspective(glm::radians(45.0f), (float)fboSize / (float)fboSize, 0.1f, 100.0f);
}
