#version 410 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;
layout (location = 2) in vec2 in_tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float ratio_from_center;

void main() {
    ratio_from_center = in_tex.x;
	gl_Position = projection * view * model * vec4(in_pos, 1.0);
}