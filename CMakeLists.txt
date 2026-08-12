cmake_minimum_required(VERSION 3.20)
project(wengine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(assimp            CONFIG REQUIRED)
find_package(glfw3             CONFIG REQUIRED)
find_package(glm               CONFIG REQUIRED)
find_package(glad              CONFIG REQUIRED)
find_package(imgui             CONFIG REQUIRED)
find_package(nlohmann_json     CONFIG REQUIRED)
find_package(spdlog            CONFIG REQUIRED)
find_package(Stb               REQUIRED)

file(GLOB_RECURSE WENGINE_SOURCES
    "src/*.cpp"
    "wengine/**/*.cpp"
)

add_executable(wengine ${WENGINE_SOURCES})

target_include_directories(wengine PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${Stb_INCLUDE_DIR}
)

target_link_libraries(wengine PRIVATE
    assimp::assimp
    glfw
    glm::glm
    glad::glad
    imgui::imgui
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)