#include "renderer.h"

Renderer::Renderer(size_t w, size_t h, const std::shared_ptr<Mesh>& _model) :
	model(_model),
	width_w(w),
	height_w(h)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(w, h, "LR 6", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		throw std::exception("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::exception("Failed to initialize GLAD");


}

void Renderer::prerender() {
	glGenVertexArrays(1, &handles.VAO);
	glGenBuffers(1, &handles.EBO);
	glGenBuffers(1, &handles.VBO_normals);
	glGenBuffers(1, &handles.VBO_vertex);

	glBindVertexArray(handles.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, handles.VBO_vertex);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model->getVertices().size(), model->getVertices().data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, handles.VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model->getNormals().size(), model->getNormals().data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handles.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(size_t) * model->getIndexes().size(), model->getIndexes().data(), GL_STATIC_DRAW);
	countElements = model->getIndexes().size();

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	modelMatrix = glm::mat4(1.0f);
	viewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, -5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	projectionMatrix = glm::perspective(glm::radians(45.0f), static_cast<float>(width_w)/ height_w, 2.0f, 50.0f);
	

	uint32_t VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &shaders_source::vertex_shader, NULL);
	glCompileShader(VertexShader);

	if (getShaderErrors(VertexShader))
		throw std::exception("shader compile error");

	uint32_t FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &shaders_source::fragment_shader, NULL);
	glCompileShader(FragmentShader);

	if (getShaderErrors(FragmentShader))
		throw std::exception("shader compile error");

	handles.ShaderProgram = glCreateProgram();
	glAttachShader(handles.ShaderProgram, VertexShader);
	glAttachShader(handles.ShaderProgram, FragmentShader);
	glLinkProgram(handles.ShaderProgram);

	if (getShaderProgramLinkError(handles.ShaderProgram))
		throw std::exception("shader program link error");

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	glUseProgram(handles.ShaderProgram);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glUseProgram(0);

}