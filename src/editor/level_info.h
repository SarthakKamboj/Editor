#pragma once

struct level_info_t {
    char output_folder[1024]{};
    char file_name[256]{};
    char full_path[1024+256]{};
};

void create_level_info(level_info_t& level_info, char* full_path);
void create_level_info(level_info_t& level_info);
void load_level_files();