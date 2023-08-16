#version 410 core

out vec4 frag_color;

in float ratio_from_center;

uniform vec3 color;

void main() {
	float remaining = pow((1 - ratio_from_center), 1.8);
	vec4 light_multiplier = vec4(remaining, remaining, remaining, 1);
	frag_color = vec4(color * vec3(light_multiplier), 1);
}