#include "transform.h"
#include <vector>

std::vector<transform_t> transforms;

int create_transform(glm::vec3 position, glm::vec3 scale, float rot_deg) {
    static int running_count = 0;
	transform_t transform;
	transform.position = position;
	transform.scale = scale;
	transform.rotation_deg = rot_deg;
    transform.handle = running_count;
	transforms.push_back(transform);
    running_count++;
	return transform.handle;
}

glm::mat4 get_model_matrix(const transform_t& transform) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, glm::radians(transform.rotation_deg), glm::vec3(0.f, 0.f, 1.0f));
	const glm::vec3& scale = transform.scale;
	model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));
	return model;
}

transform_t* get_transform(int transform_handle) {
    for (transform_t& transform : transforms) {
        if (transform.handle == transform_handle) {
            return &transform;
        }
    }
	return NULL;
}

void remove_transform(int transform_handle) {
    int idx_to_remove = -1;
    for (int i = 0; i < transforms.size(); i++) {
        if (transform_handle == transforms[i].handle) {
            idx_to_remove = i;
            break;
        }
    }
    if (idx_to_remove != -1) {
        transforms.erase(transforms.begin() + idx_to_remove, transforms.begin() + idx_to_remove + 1);
    }
}