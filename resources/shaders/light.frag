#version 410 core

out vec4 frag_color;

in float ratio_from_center;

uniform vec3 color;
uniform float intensity;

void main() {
	float remaining = pow((1 - ratio_from_center), 1.8);
	float val = remaining * intensity;
	vec4 light_multiplier = vec4(val, val, val, 1);
	frag_color = vec4(color * vec3(light_multiplier), 1);
}