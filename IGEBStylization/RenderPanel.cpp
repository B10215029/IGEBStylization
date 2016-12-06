#include "RenderPanel.h"
#include "ShaderUtility.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

int RenderPanel::fboSize = 99;

RenderPanel::RenderPanel()
{
	rotation = glm::vec3(glm::radians(15.0f), 0, 0);
	modelMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
	viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1));
	projectionMatrix = glm::perspective(glm::radians(45.0f), (float)fboSize / (float)fboSize, 0.1f, 100.0f);
	focusTexture = 0;
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
}

void RenderPanel::Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void RenderPanel::Display()
{
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
		BindGL();
		CalculateResult();
		ReleaseGL();
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

float ColorLength(unsigned char* d)
{
	return (d[0] / 255.0f) * (d[0] / 255.0f) + (d[1] / 255.0f) * (d[1] / 255.0f) + (d[2] / 255.0f) * (d[2] / 255.0f);
}

float VectorLength(float* v, int d)
{
	float r = 0;
	for (int i = 0; i < d; i++) {
		r += v[i] * v[i];
	}
	return r;
}

void RenderPanel::CalculateResult()
{
	unsigned char* ec[4];
	unsigned char* tc[4];
	unsigned char* e;
	unsigned char* r = new unsigned char[fboSize * fboSize * 3];
	float* exampleFeature = new float[fboSize * fboSize];
	ec[0] = writeTextureToArray(exaColTex[0]);
	ec[1] = writeTextureToArray(exaColTex[1]);
	ec[2] = writeTextureToArray(exaColTex[2]);
	ec[3] = writeTextureToArray(exaColTex[3]);
	tc[0] = writeTextureToArray(tarColTex[0]);
	tc[1] = writeTextureToArray(tarColTex[1]);
	tc[2] = writeTextureToArray(tarColTex[2]);
	tc[3] = writeTextureToArray(tarColTex[3]);
	e = writeTextureToArray(exampleTexture);
	for (int i = 0; i < fboSize * fboSize; i++) {
		float f[4];
		f[0] = ColorLength(&ec[0][i * 3]);
		f[1] = ColorLength(&ec[1][i * 3]);
		f[2] = ColorLength(&ec[2][i * 3]);
		f[3] = ColorLength(&ec[3][i * 3]);
		exampleFeature[i] = VectorLength(f, 4);
	}
	int process = 0, size = fboSize * fboSize;
	for (int i = 0; i < fboSize * fboSize; i++) {
		if (i * 100 / size > process) {
			process = i * 100 / size;
			//printf("%d%%\n", process);
			if (process % 10)
				putc('-', stdout);
			else
				putc('|', stdout);
			//putc('0' + process / 10, stdout);
		}
		float f[4], fl, minDis = FLT_MAX;
		int minIndex;
		f[0] = ColorLength(&tc[0][i * 3]);
		f[1] = ColorLength(&tc[1][i * 3]);
		f[2] = ColorLength(&tc[2][i * 3]);
		f[3] = ColorLength(&tc[3][i * 3]);
		fl = VectorLength(f, 4);
		for (int j = 0; j < fboSize * fboSize; j++) {
			if (abs(exampleFeature[j] - fl) < minDis) {
				minDis = abs(exampleFeature[j] - fl);
				minIndex = j;
			}
		}
		//if (i % 100 == 0) printf("%d, %lf\n", minIndex, minDis);
		r[i * 3 + 0] = e[minIndex * 3 + 0];
		r[i * 3 + 1] = e[minIndex * 3 + 1];
		r[i * 3 + 2] = e[minIndex * 3 + 2];
	}
	printf("|\nDone\n");
	glGenTextures(1, &resultTexture);
	glBindTexture(GL_TEXTURE_2D, resultTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fboSize, fboSize, 0, GL_RGB, GL_UNSIGNED_BYTE, r);
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] r;
	delete[] exampleFeature;
	free(ec[0]);
	free(ec[1]);
	free(ec[2]);
	free(ec[3]);
	free(tc[0]);
	free(tc[1]);
	free(tc[2]);
	free(tc[3]);
	free(e);
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
