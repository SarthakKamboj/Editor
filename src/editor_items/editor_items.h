#pragma once

#include "imgui.h"
#include "app.h"

void imgui_new_frame();
void create_dockspace(ImGuiIO& io);
void create_menu_bar();
void create_world_map_window(application_t& app, float x_offset);
void imgui_end_of_frame(ImGuiIO &io);
void create_editor_windows(ImGuiIO &io, application_t& app, float x_offset);