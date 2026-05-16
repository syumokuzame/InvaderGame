cmake_minimum_required(VERSION 3.20)
project(InvaderGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ソースファイル
file(GLOB_RECURSE GAME_SOURCES "Game/src/*.cpp")
file(GLOB_RECURSE ENGINE_SOURCES "Engine/src/*.cpp")

add_executable(InvaderGame ${GAME_SOURCES} ${ENGINE_SOURCES})

target_include_directories(InvaderGame PRIVATE Game/include Engine/include)

# Windows Console API は標準で利用可能（追加リンク不要）
if(MSVC)
    # 警告レベル 4、警告をエラーとして扱う
    target_compile_options(InvaderGame PRIVATE /W4)
else()
    target_compile_options(InvaderGame PRIVATE -Wall -Wextra -pedantic)
endif()

# 出力先を build/ 直下に統一
set_target_properties(InvaderGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
