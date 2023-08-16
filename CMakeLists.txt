cmake_minimum_required(VERSION 3.22.1)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()
# set(CMAKE_BUILD_TYPE "Release")

project(editor)

message("project editor")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
try_compile(HAS_FS 
    "${CMAKE_BINARY_DIR}/temp" 
    "${CMAKE_SOURCE_DIR}/tests/has_filesystem.c"
    CMAKE_FLAGS -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON
    LINK_LIBRARIES stdc++fs
)

if(HAS_FS)
    message(STATUS "Compiler has filesystem support")
else()
    message(STATUS "Compiler is missing filesystem capabilities")
endif(HAS_FS)

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
    src/renderer/basic/quad.cpp
    src/renderer/basic/light.cpp
    src/renderer/graphics/buffers.cpp
    src/renderer/graphics/mesh.cpp
    src/renderer/graphics/render_resources.cpp
    src/renderer/graphics/vertex.cpp
    src/input/input.cpp
    src/editor/add_world_item_modal.cpp
    src/editor/editor_windows.cpp
    src/editor/editor.cpp
    src/editor/gridline.cpp
    src/editor/level_info.cpp
    src/editor/world_item.cpp
    src/editor/loader.cpp

    # imgui files
    external/imgui-docking/imgui_demo.cpp
    external/imgui-docking/imgui_draw.cpp
    external/imgui-docking/imgui_tables.cpp
    external/imgui-docking/imgui_widgets.cpp
    external/imgui-docking/imgui.cpp
    external/imgui-docking/backends/imgui_impl_opengl3.cpp
    external/imgui-docking/backends/imgui_impl_sdl2.cpp
)

# set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# try_compile(HAS_FS 
#     "${CMAKE_BINARY_DIR}/temp" 
#     "${CMAKE_SOURCE_DIR}/tests/has_filesystem.c"
#     CMAKE_FLAGS -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON
#     LINK_LIBRARIES stdc++fs
# )

# if(HAS_FS)
#     message(STATUS "Compiler has filesystem support")
# else()
# #   .... You could also try searching for boost::filesystem here.
#     message(STATUS "Compiler is missing filesystem capabilities")
# endif(HAS_FS)

# set(CMAKE_CXX_FLAGS "-std=c++17 -lstdc++fs")

add_subdirectory(external/glm)
add_subdirectory(external/glad)
add_subdirectory(external/stb_image)
# add_subdirectory(external/SDL2-2.28.2)

# add_subdirectory(external/imgui-docking)

target_include_directories(${PROJECT_NAME} 
    PUBLIC external/glm
    PUBLIC external/glad/include
    PUBLIC external/SDL2-x86/include/SDL2
    # PUBLIC external/SDL2-2.28.2/include
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

target_link_libraries(${PROJECT_NAME} PRIVATE glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE stb_image)
target_link_libraries(${PROJECT_NAME} PRIVATE mingw32 SDL2main SDL2)
# target_link_libraries(${PROJECT_NAME} PRIVATE mingw32)
# if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    # target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
# endif()
# target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
# target_link_libraries(${PROJECT_NAME} stdc++fs)