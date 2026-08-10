# ============================================================
#  Render Master — C++ 版建置腳本 (CMake)
#  CMake 會自動從 GitHub 下載並編譯 GLFW + GLAD，你不用手動抓。
# ============================================================
cmake_minimum_required(VERSION 3.20)
project(RenderMaster LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ---- GLFW：開視窗 + 鍵盤滑鼠輸入（相當於 C# 的 Silk.NET.Windowing/Input）----
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ---- GLAD：載入 OpenGL 函式指標（相當於 C# 的 Silk.NET.OpenGL）----
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.6
    SOURCE_SUBDIR  cmake
)
FetchContent_MakeAvailable(glad)
# 產生一個 OpenGL 3.3 Core 的 GLAD 函式庫
glad_add_library(glad_gl_core_33 REPRODUCIBLE API gl:core=3.3)

# ---- GLM：向量/矩陣數學庫（相當於 C# 的 System.Numerics，但專為 OpenGL 設計）----
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# ---- Assimp：讀取 3D 模型檔（.obj/.fbx/.gltf…）。只開 OBJ 讀取以加快編譯 ----
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.4.3
)
set(ASSIMP_BUILD_TESTS                    OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL                        OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS             OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER             ON  CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS             OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)

# ---- 我們的程式 ----
add_executable(RenderMaster main.cpp)
target_link_libraries(RenderMaster PRIVATE glfw glad_gl_core_33 glm::glm assimp::assimp)

# 告訴 MSVC：我們的原始碼是 UTF-8（只套在自己的程式，不污染第三方庫如 assimp/zlib）
if (MSVC)
    target_compile_options(RenderMaster PRIVATE /utf-8)
endif()

# 把要載入的模型檔完整路徑，編進程式（用 Assimp 內建的蜘蛛模型）
target_compile_definitions(RenderMaster PRIVATE
    MODEL_FILE="${assimp_SOURCE_DIR}/test/models/OBJ/spider.obj")
