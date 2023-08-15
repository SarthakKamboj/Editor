#include "level_info.h"
#include <vector>
#include <stdio.h>
#include <string.h>

std::vector<level_info_t> level_infos;

void create_level_info(level_info_t& level_info) {
    char* last_slash = strrchr(level_info.full_path, '\\');
    memcpy(level_info.file_name, last_slash+1, strlen(last_slash+1));
    int num_chars_of_folder = last_slash - level_info.full_path;
    memcpy(level_info.output_folder, level_info.full_path, num_chars_of_folder);
}

void create_level_info(level_info_t& level_info, char* full_path) {
    char* last_slash = strrchr(full_path, '\\');
    memcpy(level_info.file_name, last_slash+1, strlen(last_slash+1));
    int num_chars_of_folder = last_slash - level_info.full_path;
    memcpy(level_info.output_folder, level_info.full_path, num_chars_of_folder);
    memcpy(level_info.full_path, full_path, strlen(full_path));
}

void load_level_files() {
    const char* load_settings_file = "./load_settings.gmeconfig";
    FILE* settings_file = fopen(load_settings_file, "r");
    if (settings_file) {
        while (!feof(settings_file)) {
            level_info_t level_info;
            fscanf(settings_file, "%s\n", level_info.full_path);
            if (strcmp(level_info.full_path, "") == 0) {
                break;
            }
            create_level_info(level_info);
            level_infos.push_back(level_info);
        }
        fclose(settings_file);
    }
}