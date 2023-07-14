#version 410 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 tex;

void main() {
	gl_Position = projection * view * model * vec4(in_pos, 1.0);
	gl_Position = vec4(in_pos * .5, 1.0);
	tex = in_tex;
}