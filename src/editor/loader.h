#pragma once

#include "app.h"
#include "editor/editor.h"

void create_new_level_file_modal();
void create_load_screen_window(editor_t& editor);
void write_path_to_load_config(const char* path);
void delete_path_from_load_config(const char* path);