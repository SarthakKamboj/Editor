#pragma once

#include "app.h"

void create_new_level_file_modal();
void loader(application_t& app);
void write_path_to_load_config(const char* path);
void delete_path_from_load_config(const char* path);