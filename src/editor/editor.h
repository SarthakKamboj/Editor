#pragma once

#include "level_info.h"
#include "app.h"
#include "imgui.h"

enum editor_state {
    LOAD_SCREEN = 0,
    EDITOR_SCREEN
};

struct editor_settings_t {
	ImGuiIO* io;
    editor_state state = editor_state::LOAD_SCREEN;
};

struct editor_t {
    editor_settings_t editor_settings;
    level_info_t cur_level{};
};

editor_t init_editor(application_t& app);
void editor_new_frame();
void editor_end_of_frame(editor_t& editor);

void load_level_in_editor(editor_t& editor, level_info_t& level_info);
void unload_level_in_editor(editor_t& editor);