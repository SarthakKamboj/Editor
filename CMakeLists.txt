cmake_minimum_required(VERSION 3.22.1)
project(editor)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/init.cpp
    src/app.cpp
    src/utils/conversion.cpp
    src/utils/io.cpp
    src/utils/time.cpp
    src/transform/transform.cpp
    src/renderer/renderer.cpp
    src/renderer/camera.cpp
    src/renderer/basic/shape_renders.cpp
    src/renderer/opengl/buffers.cpp
    src/renderer/opengl/object_data.cpp
    src/renderer/opengl/resources.cpp
    src/renderer/opengl/vertex.cpp
    src/input/input.cpp
    src/editorItems/addWorldItemModal.cpp
    src/editorItems/gridline.cpp
    src/editorItems/worldItem.cpp

    # imgui files
    external/imgui-docking/imgui_demo.cpp
    external/imgui-docking/imgui_draw.cpp
    external/imgui-docking/imgui_tables.cpp
    external/imgui-docking/imgui_widgets.cpp
    external/imgui-docking/imgui.cpp
    external/imgui-docking/backends/imgui_impl_opengl3.cpp
    external/imgui-docking/backends/imgui_impl_sdl2.cpp
)

add_subdirectory(external/glm)
add_subdirectory(external/glad)
add_subdirectory(external/stb_image)
# add_subdirectory(external/imgui-docking)

target_include_directories(${PROJECT_NAME} 
    PUBLIC external/glm
    PUBLIC external/glad/include
    PUBLIC external/SDL2-x86/include/SDL2
    PUBLIC external/stb_image
    PUBLIC external/imgui-docking
    PUBLIC src
)

target_link_directories(${PROJECT_NAME} 
    PUBLIC external/glm
    PUBLIC external/glad
    PUBLIC external/SDL2-x86/lib
    PUBLIC external/stb_image
)

target_link_libraries(${PROJECT_NAME} glm)
target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} stb_image)
target_link_libraries(${PROJECT_NAME} mingw32 SDL2main SDL2)