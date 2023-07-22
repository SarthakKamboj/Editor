#include "worldItem.h"
#include <iostream>
#include <vector>
#include "imgui.h"
#include <string>
#include <fstream>
#include "transform/transform.h"
#include "renderer/basic/shape_renders.h"
#include "constants.h"
#include "input/input.h"
#include <stdexcept>
#include <map>

#define C_FILE_IO 1

std::vector<world_item_t> world_items;
std::vector<placed_world_item_t> placed_items;

int world_item_t::selected_world_item_handle = world_item_t::NONE_SELECTED;

int create_world_item(const char* path, int squares_width, int squares_height, std::string& name) {
 
    int existing_handle = get_world_item_handle(path, squares_width, squares_height);
    if (existing_handle != -1) {
        return existing_handle;
    }

    static int running_count = 0;
	world_item_t world_item;
	world_item.texture_handle = create_texture(path);
    world_item.world_item_name = name;
	world_item.grid_squares_width = squares_width;
	world_item.grid_squares_height = squares_height;	
    world_item.handle = running_count;
	world_items.push_back(world_item);
    running_count++;
    world_item_t::selected_world_item_handle = world_item.handle;
	return world_item.handle;
}

int get_world_item_handle(const char* path, int squares_width, int squares_height) {
    for (world_item_t& world_item : world_items) {
        texture_t* ptr = get_texture(world_item.texture_handle);
        assert(ptr != NULL);
        texture_t& texture = *ptr;
        if (texture.path == std::string(path) && squares_width == world_item.grid_squares_width
            && squares_height == world_item.grid_squares_height) {
            return world_item.handle;
        }
    }
    return -1;
}

#if C_FILE_IO

void write_world_items_to_file() {
    FILE* out_file;
    out_file = fopen("world_items.txt", "w");
    if (out_file) {
        for (world_item_t& world_item : world_items) {
            texture_t& tex = *get_texture(world_item.texture_handle);
            fprintf(out_file, "%s %s %s %s %i %s %i\n", world_item.world_item_name.c_str(), WORLD_ITEM_TEXT_FILE_DELIM, tex.path.c_str(), WORLD_ITEM_TEXT_FILE_DELIM, world_item.grid_squares_width, WORLD_ITEM_TEXT_FILE_DELIM, world_item.grid_squares_height);
        }
        fclose(out_file);
    } else {
        throw std::runtime_error("could not open world items file");
    }
}

// TODO: remove use of world item handles since those may change with the application
void write_world_map_to_file() {
    FILE* out_file;
	out_file = fopen("level1.txt", "w");
    std::map<int, int> handle_to_idx_map;
    if (out_file) {
        fprintf(out_file, "WORLD_ITEMS\n");
        for (int i = 0; i < world_items.size(); i++) {
            world_item_t& world_item = world_items[i];
            texture_t* ptr = get_texture(world_item.texture_handle);
            assert(ptr != NULL);
            texture_t& tex = *ptr;
            fprintf(out_file, "%s %s %s %s %i %s %i\n", world_item.world_item_name.c_str(), WORLD_ITEM_TEXT_FILE_DELIM, tex.path.c_str(), WORLD_ITEM_TEXT_FILE_DELIM, world_item.grid_squares_width, WORLD_ITEM_TEXT_FILE_DELIM, world_item.grid_squares_height);
            handle_to_idx_map[world_item.handle] = i;
        }

        fprintf(out_file, "PLACED_ITEMS\n");
        for (placed_world_item_t& placed_item : placed_items) {
            fprintf(out_file, "%i %s %i %s %i\n", handle_to_idx_map[placed_item.world_item_handle], WORLD_ITEM_TEXT_FILE_DELIM, (int)placed_item.bottom_left_grid_square_pos.x, WORLD_ITEM_TEXT_FILE_DELIM, (int)placed_item.bottom_left_grid_square_pos.y);
        }
        fclose(out_file);
    } else {
        throw std::runtime_error("could not open world items file");
    }
}

#else

void write_world_items_to_file() {
    std::ofstream out_file;
	out_file.open("world_items.txt");
    if (out_file.is_open()) {
        for (world_item_t& world_item : world_items) {
            texture_t& tex = *get_texture(world_item.texture_handle);
            out_file << world_item.world_item_name << WORLD_ITEM_TEXT_FILE_DELIM << tex.path << WORLD_ITEM_TEXT_FILE_DELIM << std::to_string(world_item.grid_squares_width) << WORLD_ITEM_TEXT_FILE_DELIM << std::to_string(world_item.grid_squares_height) << "\n";
        }
        out_file.close();
    } else {
        throw std::runtime_error("could not open world items file");
    }
}

// TODO: remove use of world item handles since those may change with the application
void write_world_map_to_file() {
    std::ofstream out_file;
	out_file.open("level1.txt");
    std::map<int, int> handle_to_idx_map;
    if (out_file.is_open()) {
        out_file << "WORLD_ITEMS" << std::endl;
        for (int i = 0; i < world_items.size(); i++) {
            world_item_t& world_item = world_items[i];
            texture_t* ptr = get_texture(world_item.texture_handle);
            assert(ptr != NULL);
            texture_t& tex = *ptr;
            out_file << world_item.world_item_name << WORLD_ITEM_TEXT_FILE_DELIM << tex.path << WORLD_ITEM_TEXT_FILE_DELIM << std::to_string(world_item.grid_squares_width) << WORLD_ITEM_TEXT_FILE_DELIM << std::to_string(world_item.grid_squares_height) << "\n";
            handle_to_idx_map[world_item.handle] = i;
        }

        out_file << "\nPLACED_ITEMS" << std::endl;
        for (placed_world_item_t& placed_item : placed_items) {
            out_file << handle_to_idx_map[placed_item.world_item_handle] << WORLD_ITEM_TEXT_FILE_DELIM << placed_item.bottom_left_grid_square_pos.x << WORLD_ITEM_TEXT_FILE_DELIM << placed_item.bottom_left_grid_square_pos.y << "\n";
        }
        out_file.close();
    } else {
        throw std::runtime_error("could not open world items file");
    }
}
#endif


world_item_t* get_world_item(int world_handle) {
    for (world_item_t& world_item : world_items) {
        if (world_item.handle == world_handle) {
            return &world_item;
        }
    }
	return NULL;
}

void remove_world_item(int world_handle) {
    int idx_to_remove = -1;
    for (int i = 0; i < world_items.size(); i++) {
        if (world_items[i].handle == world_handle) {
            idx_to_remove = i;
            break;
        }
    }

    assert(idx_to_remove != -1);

    for (int i = 0; i < placed_items.size(); i++) {
        if (placed_items[i].world_item_handle == world_handle) {
            remove_placed_world_item(placed_items[i].handle);
            i--;
        }
    }

    world_items.erase(world_items.begin() + idx_to_remove, world_items.begin() + idx_to_remove + 1);
    if (world_item_t::selected_world_item_handle == world_handle) {
        world_item_t::selected_world_item_handle = world_item_t::NONE_SELECTED;
    }
}

extern mouse_state_t mouse_state;

void update_world_item_catalog() {
    const float image_height = 50.f;
	ImGui::Begin("World Items");
    std::string select_handle_text = "Selected Handle: " + std::to_string(world_item_t::selected_world_item_handle);
    ImGui::Text(select_handle_text.c_str());
	for (int i = 0; i < world_items.size(); i++) {
        // push id because the titles are used as IDs and if within the same window, multiple items have the same id, it can cause issues
        // push id helps solve that by ensuring unique ids
        // https://github.com/ocornut/imgui/issues/74
        ImGui::PushID(i);
		world_item_t& item = world_items[i];
        texture_t* tex_ptr = get_texture(item.texture_handle);
        assert(tex_ptr != NULL);
		texture_t& texture = *tex_ptr;
        float ratio = 50.f / texture.height;
        // ? uv0 = top left uv coordinate and uv1 = bottom right uv coordinate
		ImGui::Image((void*)texture.id, ImVec2(texture.width * ratio, texture.height * ratio), ImVec2(0,1), ImVec2(1,0));
        ImGui::Text(item.world_item_name.c_str());
        std::string width_text = "Width: " + std::to_string(item.grid_squares_width);
        ImGui::Text(width_text.c_str());
        std::string height_text = "Height: " + std::to_string(item.grid_squares_height);
        ImGui::Text(height_text.c_str());
        std::string handle_text = "Handle: " + std::to_string(item.handle);
        ImGui::Text(handle_text.c_str());
		if (world_item_t::selected_world_item_handle != i) {
            bool clicked_on_select = ImGui::Button("Select Item");
            if (clicked_on_select) {
                world_item_t::selected_world_item_handle = item.handle;
            }
        } else {
            ImGui::Button("Currently Selected");
        } 
        if (ImGui::Button("Remove World Item")) {
            remove_world_item(item.handle);
        }
        ImGui::PopID();
	}
	ImGui::End();
}

int place_world_item(int world_item_handle, const glm::vec2& bottom_left_grid_square_pos) {
    if (world_item_handle == world_item_t::NONE_SELECTED) {
        return -1;
    }

    for (const placed_world_item_t& placed_item : placed_items) {
        if (placed_item.world_item_handle == world_item_handle && 
           placed_item.bottom_left_grid_square_pos == bottom_left_grid_square_pos ) {
            return -1;
        }
    }

    static int running_count = 0;

	placed_world_item_t placed_world_item;
	placed_world_item.world_item_handle = world_item_handle;
	placed_world_item.bottom_left_grid_square_pos = bottom_left_grid_square_pos;
    placed_world_item.handle = running_count;
    running_count++;

    world_item_t* world_item_ptr = get_world_item(world_item_handle);
    assert(world_item_ptr != NULL);
    world_item_t& world_item = *world_item_ptr;
    glm::vec2 center_grid_square;
    center_grid_square.x = bottom_left_grid_square_pos.x + (world_item.grid_squares_width / 2.f);
    center_grid_square.y = bottom_left_grid_square_pos.y + (world_item.grid_squares_height / 2.f);

    glm::vec3 position(1.f);
    position.x = center_grid_square.x * GRID_SQUARE_WIDTH;
    position.y = center_grid_square.y * GRID_SQUARE_WIDTH;

    int transform_handle = create_transform(position, glm::vec3(1), 0);
	glm::vec3 color(1);
    placed_world_item.rec_render_handle = create_rectangle_render(transform_handle, color, 
        world_item.texture_handle, 
        world_item.grid_squares_width * GRID_SQUARE_WIDTH, 
        world_item.grid_squares_height * GRID_SQUARE_WIDTH, false, 1.0f);
	placed_items.push_back(placed_world_item);
	return placed_world_item.handle;
}

placed_world_item_t* get_placed_world_item(int placed_handle) {
    for (placed_world_item_t& placed_item : placed_items) {
        if (placed_handle == placed_item.handle) {
            return &placed_item;
        }
    }
    return NULL;
}

void remove_placed_world_item(glm::vec2 grid_square_pos) {
    for (int i = 0; i < placed_items.size(); i++) {
        const placed_world_item_t& placed_item = placed_items[i];
        if (placed_item.bottom_left_grid_square_pos.x == grid_square_pos.x &&
            placed_item.bottom_left_grid_square_pos.y == grid_square_pos.y) {
            remove_rectangle_render(placed_item.rec_render_handle);
            placed_items.erase(placed_items.begin() + i, placed_items.begin() + i + 1);
            i--;
        }
    }
}

void remove_placed_world_item(int placed_handle) {
    int idx_to_remove = 0;
    for (int i = 0; i < placed_items.size(); i++) {
        const placed_world_item_t& placed_item = placed_items[i];
        if (placed_item.handle == placed_handle) {
            remove_rectangle_render(placed_item.rec_render_handle);
            idx_to_remove = i;
        }
    }
    placed_items.erase(placed_items.begin() + idx_to_remove, placed_items.begin() + idx_to_remove + 1);
}