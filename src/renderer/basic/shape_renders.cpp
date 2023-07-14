#include "shape_renders.h"
#include "utils/conversion.h"
#include "../renderer.h"
#include "transform/transform.h"
#include <vector>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "input/input.h"

opengl_object_data rectangle_render_t::obj_data{};

std::vector<rectangle_render_t> rectangles;

int create_rectangle_render(int transform_handle, glm::vec3& color, int tex_handle, float width, float height, bool wireframe, float tex_influence) {
    static int running_count = 0;
	rectangle_render_t rectangle;
	rectangle.transform_handle = transform_handle;
	rectangle.width = width;
	rectangle.height = height;
	rectangle.color = color;
	rectangle.wireframe_mode = wireframe;
    rectangle.tex_influence = tex_influence;
    // 2D ortho projection represented in pixels and original rec mesh is 1x1, so scale rectangle mesh by rec width and height
	rectangle._internal_transform.scale = glm::vec3(width, height, 1.f);
    rectangle.texture_handle = tex_handle;
    rectangle.handle = running_count;
	rectangles.push_back(rectangle);
    running_count++;
	return rectangle.handle;
}

rectangle_render_t* get_rectangle_render(int rec_render_handle) {
    for (rectangle_render_t& rec_render : rectangles) {
        if (rec_render.handle == rec_render_handle) {
            return &rec_render;
        }
    }
    return NULL;
}

void remove_rectangle_render(int rec_render_handle) {
    rectangle_render_t* rec_render_ptr = get_rectangle_render(rec_render_handle);
    assert(rec_render_ptr != NULL);
    rectangle_render_t& rec_render = *rec_render_ptr;
    remove_transform(rec_render.transform_handle);
    int idx_to_remove = -1;
    for (int i = 0; i < rectangles.size(); i++) {
        if (rec_render_handle == rectangles[i].handle) {
            idx_to_remove = i;
            break;
        }
    }
    if (idx_to_remove != -1) {
        rectangles.erase(rectangles.begin() + idx_to_remove, rectangles.begin() + idx_to_remove + 1);
    }
}

void draw_rectangle_render(const rectangle_render_t& rectangle) {
    transform_t* cur_transform_ptr = get_transform(rectangle.transform_handle);
    assert(cur_transform_ptr != NULL);
	transform_t cur_transform = *cur_transform_ptr;
	cur_transform.position += rectangle._internal_transform.position;
	cur_transform.rotation_deg += rectangle._internal_transform.rotation_deg;
	cur_transform.scale *= rectangle._internal_transform.scale;
	glm::mat4 model_matrix = get_model_matrix(cur_transform);
	shader_set_mat4(rectangle_render_t::obj_data.shader, "model", model_matrix);
	shader_set_vec3(rectangle_render_t::obj_data.shader, "color", rectangle.color);
	shader_set_float(rectangle_render_t::obj_data.shader, "tex_influence", rectangle.tex_influence);
    if (rectangle.texture_handle != -1) {
        bind_texture_by_handle(rectangle.texture_handle);
    }
	if (rectangle.wireframe_mode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	draw_obj(rectangle_render_t::obj_data);
}


void draw_rectangle_render(int rectangle_handle) {
    for (rectangle_render_t& rectangle : rectangles) {
        if (rectangle_handle == rectangle.handle) {
	        draw_rectangle_render(rectangle);
            break;
        }
    }
}

void draw_rectangle_renders(camera_t& camera) {
    glm::mat4 view_matrix = get_view_matrix(camera);
	shader_set_mat4(rectangle_render_t::obj_data.shader, "view", view_matrix);
	for (const rectangle_render_t& rectangle : rectangles) {
		draw_rectangle_render(rectangle);
	}
}