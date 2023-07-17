#include "addWorldItemModal.h"
#include "imgui.h"
#include "worldItem.h"
#include <stdio.h>
#include <iostream>

// static std::string img_file_path;
static char img_file_path[1024];
static int num_squares_wide = 1;
static int num_squares_high = 1;
// static char* name_buffer = (char*)malloc(255);
static char name_buffer[255];
static bool popup_opened = false;

void open_add_world_modal() {
    popup_opened = true;
	num_squares_high = 1;
	num_squares_wide = 1;
    memset(img_file_path, 0, sizeof(img_file_path));
    memset(name_buffer, 0, 255);
}

const char* get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

void update_add_world_item_modal() {
    static bool prev_opened = false;
    if (!prev_opened && popup_opened) {
        ImGui::OpenPopup(ADD_WORLD_ITEM_MODAL_TITLE);
    }

	if (ImGui::BeginPopupModal(ADD_WORLD_ITEM_MODAL_TITLE, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Image Path", img_file_path, sizeof(img_file_path));
		ImGui::SliderInt("Number of Squares wide", &num_squares_wide, 1, 8);
		ImGui::SliderInt("Number of Squares high", &num_squares_high, 1, 8);

        // ImGui::InputText("Label: ", name_buffer, 255);
        ImGui::InputText("Label", name_buffer, sizeof(name_buffer));

        static bool works = true;
		if (ImGui::Button("Add")) {
            const char* img_ext = get_filename_ext(img_file_path);
            std::cout << img_ext << std::endl;
            if (strcmp(img_ext, "png") != 0 && strcmp(img_ext, "jpg") != 0) {
                works = false;
            } else {
                FILE* file = fopen(img_file_path, "r");
                if (file) {
                    fclose(file);
                    std::string name(name_buffer);
                    create_world_item(img_file_path, num_squares_wide, num_squares_high, name);
                    popup_opened = false;
                    works = true;
                    ImGui::CloseCurrentPopup();
                } else {
                    works = false;
                }
            }
		}
        if (!works) {
            ImGui::Text("File path is not valid");
        }

		if (ImGui::Button("Cancel")) {
            popup_opened = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

    prev_opened = popup_opened;
}