#pragma once

#include "imgui.h"
#include "app.h"
#include "editor.h"

void create_dockspace(editor_t& editor);
void create_menu_bar(editor_t& editor);
void create_world_map_window(application_t& app, float x_offset);
// void imgui_end_of_frame(ImGuiIO &io);
void create_level_info_window(editor_t &editor);
void create_light_editor_window();

void create_editor_windows(editor_t& editor, application_t &app, float x_offset);