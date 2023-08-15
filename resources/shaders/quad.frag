#version 410 core

out vec4 frag_color;

in vec2 tex_coord;
in vec2 light_uv;

uniform vec3 color;
uniform float tex_influence;

uniform sampler2D tex;
uniform sampler2D light_map;

void main() {
    vec4 light_color = texture(light_map, light_uv);

    vec4 tex_part = tex_influence * texture(tex, tex_coord);
    vec4 color_part = (1 - tex_influence) * vec4(color, 1.0);
 
	frag_color = tex_part + color_part;
    if (tex_influence > 0) {
        frag_color = frag_color * light_color;
    }
}