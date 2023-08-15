#pragma once

#include "transform/transform.h"
#include "input/input.h"

struct camera_t {
	int transform_handle = -1;
	glm::vec3 look_at;
};

camera_t create_camera();
glm::mat4 get_view_matrix(camera_t& camera);
void update_camera(camera_t& camera, key_state_t& key_state);
