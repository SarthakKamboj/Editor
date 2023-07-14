#pragma once

#include "transform/transform.h"
#include "glm/glm.hpp"
#include "../opengl/object_data.h"
#include "renderer/camera.h"

struct rectangle_render_t {
    int handle = -1;
	int transform_handle = -1;
    // _internal_transform responsible for scaling based on actual width/height of the rectangle
	transform_t _internal_transform;
	glm::vec3 color = glm::vec3(0, 0, 0);
	float width = -1.f, height = -1.f;
	bool wireframe_mode = false;
    float tex_influence = 0.f;
    int texture_handle = -1;

	static opengl_object_data obj_data;
};

int create_rectangle_render(int transform_handle, glm::vec3& color, int tex_handle, float width, float height, bool wireframe, float tex_influence);
rectangle_render_t* get_rectangle_render(int rec_render_handle);
void remove_rectangle_render(int rec_render_handle);
void draw_rectangle_render(const rectangle_render_t& rectangle);
void draw_rectangle_render(int rectangle_handle);
void draw_rectangle_renders(camera_t& camera);
