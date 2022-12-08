#pragma once

#define glsl(s) _glsl(s)
#define _glsl(s) #s


namespace shaders_source {
	static const char* vertex_shader = glsl(

		\#version 440 core\n
		layout(location = 0) in vec3 vertex;
		layout(location = 1) in vec3 normal;


		uniform mat4 PVM;
		uniform mat4 VM;
		uniform mat3 NormalMatrix;

		out vec3 v_normal;
		out vec3 FragPos;

		void main() {
			gl_Position = PVM * vec4(vertex,1.0f);
			v_normal = normalize(NormalMatrix * normal);
			FragPos = vec3(VM * vec4(vertex, 1.0f));
		}

	);

	static const char* fragment_shader = glsl(

		\#version 440 core\n

		out vec4 FragColor;

		in vec3 v_normal;
		in vec3 FragPos;

		uniform vec3 lightPos;

		void main() {

			vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
			vec3 objectColor = vec3(1.0f, 0.84f, 0.0f);
			vec3 viewPos = vec3(0.0f, 0.0f, 0.0f);

			float ambientStrength = 0.1;
			vec3 ambient = ambientStrength * lightColor;

			vec3 lightDir = normalize(lightPos - FragPos);
			float diff = max(dot(v_normal, lightDir), 0.0);
			vec3 diffuse = diff * lightColor;

			float specularStrength = 0.5;
			vec3 viewDir = normalize(viewPos - FragPos);
			vec3 reflectDir = reflect(-lightDir, v_normal);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
			vec3 specular = specularStrength * spec * lightColor;

			FragColor = vec4((ambient + diffuse + specular) * objectColor,1.0f);
		}

	);
}