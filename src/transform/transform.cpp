#include "transform.h"
#include <vector>
#include <algorithm>

std::vector<transform_t> transforms;

bool transform_idx_less(const transform_t& transform1, const transform_t& transform2) {
    return transform1.handle < transform2.handle;
}

struct {
    bool operator()(const transform_t& t1, const transform_t& t2) const {
        return transform_idx_less(t1, t2);
    }
} transform_handle_less;

int create_transform(glm::vec3 position, glm::vec3 scale, float rot_deg) {
    static int running_count = 0;
	transform_t transform;
	transform.position = position;
	transform.scale = scale;
	transform.rotation_deg = rot_deg;
    transform.handle = running_count;
	transforms.push_back(transform);
    // std::sort(transforms.data(), transforms.data() + transforms.size(), transform_idx_less);
    std::sort(transforms.begin(), transforms.end(), transform_idx_less);
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


transform_t* binary_search_transform(int start_idx, int end_idx, int transform_handle) {
    if (start_idx > end_idx) {
        return NULL;
    }

    int mid = (start_idx + end_idx) / 2;
    if (transforms[mid].handle == transform_handle) {
        return &transforms[mid];
    }
    if (transforms[mid].handle < transform_handle) {
        return binary_search_transform(mid+1, end_idx, transform_handle);
    }
    return binary_search_transform(start_idx, mid-1, transform_handle);
}

transform_t* get_transform(int transform_handle) {
    assert(transform_handle >= 0);
    // bool exists = std::binary_search(transforms.begin(), transforms.end(), transform_handle, transform_idx_less);

    // bool exists = false;
    // for (transform_t& transform : transforms) {
    //     if (transform.handle == transform_handle) {
    //         // return &transform;
    //         exists = true;
    //     }
    // }

    return binary_search_transform(0, transforms.size()-1, transform_handle);
	// return NULL;
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