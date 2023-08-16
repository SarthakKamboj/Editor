#pragma once

#include "level_info.h"

enum world_map_section {
    WORLD_MAP_NONE, WORLD_ITEMS, PLACED_WORLD_ITEMS, LIGHTS
};

// void write_world_items_to_file();
void write_world_map_to_file(level_info_t& level_info);
void read_world_map_file(const char* path);