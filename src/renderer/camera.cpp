#include "camera.h"
#include "glm/glm.hpp"
#include "constants.h"
#include "utils/time.h"
#include "glm/gtx/string_cast.hpp"
#include <iostream>

camera_t create_camera() {
	camera_t camera;
	camera.transform_handle = create_transform(glm::vec3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 5.f), glm::vec3(1.f), 0);
	camera.look_at = glm::vec3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0.f);
	return camera;
}

glm::mat4 get_view_matrix(camera_t& camera) {
	transform_t* transform = get_transform(camera.transform_handle);
    assert(transform != NULL);
	glm::mat4 view(1.0f);
	glm::vec3 translate(-transform->position.x + (WINDOW_WIDTH/2), -transform->position.y + (WINDOW_HEIGHT/2), 0.f);
	view = glm::translate(view, translate); 
	return view;
}

void update_camera(camera_t& camera, key_state_t& key_state) {
	transform_t* transform_ptr = get_transform(camera.transform_handle);
    assert(transform_ptr != NULL);
	transform_t& transform = *transform_ptr;
	if (key_state.key_being_pressed['d']) {
		transform.position.x += CAMERA_SCROLL_SPEED * engine_time_t::delta_time;
	} else if (key_state.key_being_pressed['a']) {
		transform.position.x -= CAMERA_SCROLL_SPEED * engine_time_t::delta_time;
	}
}