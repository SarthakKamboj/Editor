#version 410 core

out vec4 frag_color;

in float ratio_from_center;

uniform vec3 color;
uniform float ambient;

void main() {
	float remaining = pow((1 - ratio_from_center), 1.2);
	vec4 light_multiplier = vec4(remaining, remaining, remaining, 1);
	frag_color = vec4(color, 1) * light_multiplier;
	frag_color = vec4(max(frag_color.r, ambient), max(frag_color.g, ambient), max(frag_color.b, ambient), 1);
}