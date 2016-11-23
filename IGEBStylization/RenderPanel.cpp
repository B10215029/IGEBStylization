#include "RenderPanel.h"
#include "ShaderUtility.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

RenderPanel::RenderPanel()
{
	modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(15.0f), glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(0, 1, 0));
	viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1));
	projectionMatrix = glm::perspective(glm::radians(45.0f), (float)FBO_WIDTH / (float)FBO_HEIGHT, 0.1f, 100.0f);
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
	drawSolid.AmbientLocation = glGetUniformLocation(drawSolid.program, "Ambient");
	drawSolid.LightColorLocation = glGetUniformLocation(drawSolid.program, "LightColor");
	drawSolid.lightPosLocation = glGetUniformLocation(drawSolid.program, "lightPos");
	drawSolid.viewPosLocation = glGetUniformLocation(drawSolid.program, "viewPos");
	drawSolid.ShininessLocation = glGetUniformLocation(drawSolid.program, "Shininess");
	drawSolid.StrengthLocation = glGetUniformLocation(drawSolid.program, "Strength");
	drawSolid.isBlinnLocation = glGetUniformLocation(drawSolid.program, "isBlinn");
	drawTexture.program = loadProgram("./shader/DrawTexture.vert", "./shader/DrawTexture.frag");
	drawTexture.textureLocation = glGetUniformLocation(drawTexture.program, "image");

	GLenum drawbuff[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glGenFramebuffers(1, &exaFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, exaFBO);

	glGenTextures(4, exaColTex);
	glBindTexture(GL_TEXTURE_2D, exaColTex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, exaColTex[0], 0);

	glBindTexture(GL_TEXTURE_2D, exaColTex[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, exaColTex[1], 0);

	glBindTexture(GL_TEXTURE_2D, exaColTex[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, exaColTex[2], 0);

	glBindTexture(GL_TEXTURE_2D, exaColTex[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, exaColTex[3], 0);

	glGenRenderbuffers(1, &exaDephTex);
	glBindRenderbuffer(GL_RENDERBUFFER, exaDephTex);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, FBO_WIDTH, FBO_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, exaDephTex);

	glDrawBuffers(4, drawbuff);
	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
		std::cout << "Framebuffer Error!" << std::endl;

	glGenFramebuffers(1, &tarFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, tarFBO);

	glGenTextures(4, tarColTex);
	glBindTexture(GL_TEXTURE_2D, tarColTex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tarColTex[0], 0);

	glBindTexture(GL_TEXTURE_2D, tarColTex[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tarColTex[1], 0);

	glBindTexture(GL_TEXTURE_2D, tarColTex[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tarColTex[2], 0);

	glBindTexture(GL_TEXTURE_2D, tarColTex[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, tarColTex[3], 0);

	glGenRenderbuffers(1, &tarDephTex);
	glBindRenderbuffer(GL_RENDERBUFFER, tarDephTex);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, FBO_WIDTH, FBO_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, tarDephTex);

	glDrawBuffers(4, drawbuff);
	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
		std::cout << "Framebuffer Error!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	OpenMesh::IO::read_mesh(backgroundMesh, "background.obj");
	backgroundMesh.update_normals();
	backgroundMesh.invalid = true;
	OpenMesh::IO::read_mesh(exampleMesh, "ball.obj");
	exampleMesh.update_normals();
	exampleMesh.invalid = true;
	OpenMesh::IO::read_mesh(targetMesh, "mesh.obj");
	targetMesh.update_normals();
	targetMesh.invalid = true;
}

void RenderPanel::Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}
int imgi = 0;
void RenderPanel::Display()
{
	glViewport(0, 0, FBO_WIDTH, FBO_HEIGHT);
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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(drawTexture.program);
	glUseProgram(drawTexture.program);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(drawTexture.textureLocation, 0);
	if (imgi < 4)
		glBindTexture(GL_TEXTURE_2D, exaColTex[imgi]);
	else
		glBindTexture(GL_TEXTURE_2D, tarColTex[imgi - 4]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void RenderPanel::MouseDown(int x, int y, int button)
{
	imgi = (imgi + 1) % 8;
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
