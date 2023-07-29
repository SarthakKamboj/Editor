#include "loader.h"
#include "imgui.h"
#include <vector>
#include "app.h"
#include <dirent.h>
#include <stdio.h>

#define NEW_FILE_MODAL_TITLE "New File Modal"

extern std::vector<level_info_t> level_infos;
extern application_t app;

static bool new_file_modal_opened = false;

void loader(application_t& app) {
    ImGui::Begin("Loader");
    if (ImGui::Button("New file")) {
        new_file_modal_opened = true;
    }

    for (int i = 0; i < level_infos.size(); i++) {
        level_info_t& level_info = level_infos[i];
        if (ImGui::Button(level_info.full_path)) {
            FILE* file = fopen(level_info.full_path, "r");
            if (!file) {
                delete_path_from_load_config(level_info.full_path);
                level_infos.erase(level_infos.begin()+i, level_infos.begin()+i+1);
                i--;
                continue;
            }
            load_level_in_app(app, level_info);
            fclose(file);
        }
    }
    ImGui::End();
    create_new_level_file_modal();
}

void create_new_level_file_modal() {
    static bool prev_opened = false;
    static level_info_t new_level_info{};
    if (!prev_opened && new_file_modal_opened) {
        ImGui::OpenPopup(NEW_FILE_MODAL_TITLE);
    }

	if (ImGui::BeginPopupModal(NEW_FILE_MODAL_TITLE, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Folder path", new_level_info.output_folder, sizeof(new_level_info.output_folder));
        ImGui::InputText("File name", new_level_info.file_name, sizeof(new_level_info.file_name));
        static bool works = true;
        static char error_text[256]{};
        if (ImGui::Button("Create")) {
            works = true;
            DIR* dir = opendir(new_level_info.output_folder);
            if (!dir) {
                works = false;
                const char* msg = "Folder does not exist";
                memcpy(error_text, msg, strlen(msg));
            } 
            else if (strcmp(new_level_info.file_name, "") == 0) {
                closedir(dir);
                works = false;
                const char* msg = "File name not provided";
                memcpy(error_text, msg, strlen(msg)); 
            }
            else {
                closedir(dir);
                sprintf(new_level_info.full_path, "%s\\%s.gme", new_level_info.output_folder, new_level_info.file_name);
                write_path_to_load_config(new_level_info.full_path);
                level_infos.push_back(new_level_info);
                load_level_in_app(app, new_level_info);
                new_file_modal_opened = false;
                ImGui::CloseCurrentPopup();
            }
        }
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
            new_file_modal_opened = false;
        }

        if (!works) {
            ImGui::Text(error_text);
        }
		ImGui::EndPopup();
    }

    prev_opened = new_file_modal_opened;
}

void write_path_to_load_config(const char* path) {
    const char* load_settings_file = "./load_settings.gmeconfig";
    FILE* settings_file = fopen(load_settings_file, "r+");
    if (settings_file) {
        while (!feof(settings_file)) {
            char line[1024];
            fscanf(settings_file, "%s\n", line);
            if (strcmp(line, path) == 0) {
                fclose(settings_file);
                return;
            }
        }
        fprintf(settings_file, "%s\n", path);
        fclose(settings_file);

        FILE* new_file = fopen(path, "w");
        assert(new_file != NULL);
        fclose(new_file);
    }
}

void delete_path_from_load_config(const char* path) {
    const char* tmp_file_name = "./tmp.gmeconfig";
    FILE* tmp = fopen(tmp_file_name, "w");
    assert(tmp != NULL);

    const char* load_settings_file = "./load_settings.gmeconfig";
    FILE* settings_file = fopen(load_settings_file, "r");
    if (settings_file) {
        while (!feof(settings_file)) {
            char line[1024];
            // fgets(line, 1024, settings_file);
            fscanf(settings_file, "%s\n", line);
            if (strcmp(line, path) != 0) {
                fprintf(tmp, "%s\n", line);
            }
        }
        fclose(settings_file);
    }
    fclose(tmp);
    remove(load_settings_file);
    rename(tmp_file_name, load_settings_file);
}