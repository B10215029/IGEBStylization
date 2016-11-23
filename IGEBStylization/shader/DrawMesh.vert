#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 in_normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 FragPos;
out vec3 Normal;

void main() {
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1);
	FragPos = position;
	Normal = in_normal;
}
