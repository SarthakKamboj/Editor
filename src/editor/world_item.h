#pragma once

#include "transform/transform.h"
#include "app.h"
#include "level_info.h"

struct world_item_t {
    int handle = -1;
	int texture_handle = -1;
	int grid_squares_width = -1;
	int grid_squares_height = -1;
    std::string world_item_name;

    static int selected_world_item_handle;
    const static int NONE_SELECTED = -1;
};

struct placed_world_item_t {
    int handle = -1;
    int transform_handle = -1;
	int world_item_handle = -1;
	glm::vec2 bottom_left_grid_square_pos;
    int rec_render_handle = -1;
};

// world item functions
int create_world_item(const char* path, int squares_width, int squares_height, std::string& name);
world_item_t* get_world_item(int world_handle);
int get_world_item_handle(const char* path, int squares_width, int squares_height);
void remove_world_item(int world_handle);
void clear_world_items();

// placed world item functions
void init_placed_world_items(const char* path);
int place_world_item(int world_item_handle, const glm::vec2& bottom_left_grid_square_pos);
placed_world_item_t* get_placed_world_item(int placed_handle);
void remove_placed_world_item(glm::vec2 grid_square_pos);
void remove_placed_world_item(int placed_handle);
void clear_placed_items();

// SERIALIZATION FUNCTION
void write_world_items_to_file();
void write_world_map_to_file(level_info_t& level_info);

// EDITOR FUNCTIONS
void create_world_item_catalog();
void create_placed_world_items_editor();