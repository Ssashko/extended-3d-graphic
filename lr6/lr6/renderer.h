#pragma once

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <glfw3.h>

#include "shader.hpp"

#include <exception>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include "figure.h"
struct MeshDeviceHandles {
	uint32_t VAO;
	uint32_t EBO;
	uint32_t VBO_vertex;
	uint32_t VBO_normals;

	uint32_t ShaderProgram;
};
class Renderer {
public:
	Renderer(size_t w, size_t h, const std::shared_ptr<Mesh>& _model);

	void prerender();

	void render() {
		static float angle = 45;
		//light_pos = glm::rotate(glm::radians(0.01f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(light_pos, 1.0f);
		//projectionMatrix = glm::perspective(glm::radians(angle-=0.01), static_cast<float>(width_w) / height_w, 2.0f, 50.0f);
		if (angle < 10)
			angle = 90;
		glUseProgram(handles.ShaderProgram);

		glm::mat4 VM = viewMatrix * modelMatrix;
		glm::mat4 PVM = projectionMatrix * VM;
		glm::mat3 NormalMatrix = glm::mat3(transpose(inverse(viewMatrix * modelMatrix)));
		glUniformMatrix4fv(glGetUniformLocation(handles.ShaderProgram, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
		glUniformMatrix4fv(glGetUniformLocation(handles.ShaderProgram, "VM"), 1, GL_FALSE, glm::value_ptr(VM));
		glUniformMatrix3fv(glGetUniformLocation(handles.ShaderProgram, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(NormalMatrix));

		glUniform3fv(glGetUniformLocation(handles.ShaderProgram, "lightPos"), 1, glm::value_ptr(light_pos));

		glBindVertexArray(handles.VAO);
		glDrawElements(GL_TRIANGLES, countElements, GL_UNSIGNED_INT, static_cast<void*>(0));
		glBindVertexArray(0);
		glUseProgram(0);
	}


	void run() {
		prerender();
		while (!glfwWindowShouldClose(window))
		{
			processInput(window);

			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			render();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	~Renderer() {
		glfwTerminate();
		glDeleteVertexArrays(1, &handles.VAO);
		glDeleteBuffers(1,&handles.EBO);
		glDeleteBuffers(1, &handles.VBO_normals);
		glDeleteBuffers(1, &handles.VBO_vertex);
	}
private:
	static void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	bool getShaderErrors(uint32_t shaderHandler) {
		int32_t success;
		std::vector<char> infoLog(1024);
		glGetShaderiv(shaderHandler, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			int32_t writed = 0;
			glGetShaderInfoLog(shaderHandler, 1023, &writed, infoLog.data());
			infoLog[writed] = '/0';
			std::cout << "ERROR::VERTEXSHADER::COMPILATION_FAILED\n" << infoLog.data() << std::endl;
			return true;
		}
		return false;
	}

	bool getShaderProgramLinkError(uint32_t shaderHandlerProgram) {
		int32_t success;
		std::vector<char> infoLog(1024);
		glGetProgramiv(shaderHandlerProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			int32_t writed = 0;
			glGetProgramInfoLog(shaderHandlerProgram, 1023, &writed, infoLog.data());
			infoLog[writed] = '/0';
			std::cout << "ERROR::SHADERPROGRAM::LINKING_FAILED\n" << infoLog.data() << std::endl;
			return true;
		}
		return false;
	}

	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	glm::vec3 light_pos = glm::vec3(10.f, 0.f, 10.f);

	GLFWwindow* window;
	size_t width_w;
	size_t height_w;
	size_t countElements;
	const std::shared_ptr<Mesh>& model;

	MeshDeviceHandles handles;
};

