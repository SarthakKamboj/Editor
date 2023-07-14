#version 410 core

out vec4 frag_color;
in vec2 tex;

uniform sampler2D fbo_texture;

void main() {
	frag_color = texture(fbo_texture, tex * 2);
}