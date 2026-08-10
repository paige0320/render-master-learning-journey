# Render Master

A small real-time 3D renderer written from scratch in **C++ / OpenGL**, built as a hands-on journey into graphics programming.

> 從零開始、親手用 C++ 與 OpenGL 打造的小型即時 3D 渲染器。

![screenshot](docs/screenshot.png)
<!-- 換成你的截圖/GIF：把檔案放到 docs/screenshot.png，或改這行的路徑 -->

---

## Features

- **Fly camera** — WASD + mouse look, frame-rate independent (delta time)
- **Phong lighting** — ambient + diffuse + specular
- **Real-time shadows** — shadow mapping with an off-screen depth pass, depth bias, and PCF soft edges (press `B` / `P` to toggle bias / PCF and see the difference)
- **Model loading** — imports real `.obj` meshes via [Assimp], auto-centered and scaled to fit
- **Procedural geometry** — cube and UV-sphere generated in code, drawn with indexed buffers (EBO)

Also included: reusable `Shader`, `Mesh`, and `Camera` header classes from an engine-refactor pass (`Shader.h` / `Mesh.h` / `Camera.h`).

Built through a step-by-step learning path that also covered textures, specular maps, and multiple light types (directional / point / spotlight). Interactive WebGL recreations of every step:

**🔗 Interactive demos:** _(paste your shared artifact link here)_

---

## Controls

| Key / Input | Action |
|-------------|--------|
| `W` `A` `S` `D` | Move |
| Mouse | Look around |
| `Esc` | Quit |

---

## Build

The project uses **CMake** and fetches all dependencies automatically — you don't download anything by hand.

**Requirements**
- A C++17 compiler (tested with MSVC / Visual Studio 2022)
- [CMake] 3.20+
- [Git] (used by CMake to fetch dependencies)
- [Python] 3 with `jinja2` (used to generate the GLAD OpenGL loader) — `pip install jinja2`

**Dependencies (fetched automatically by CMake)**
- [GLFW] — window & input
- [GLAD] — OpenGL 3.3 function loader
- [GLM] — vector / matrix math
- [Assimp] — 3D model importing

**Build & run (Windows / PowerShell)**
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
./build/Debug/RenderMaster.exe
```

> The first configure/build takes a few minutes while dependencies are downloaded and compiled (Assimp is large). Subsequent builds are fast.

---

## Project structure

```
render-master/
├─ main.cpp        # window setup, scene assembly, render loop
├─ Shader.h        # compile/link a shader program + set uniforms
├─ Mesh.h          # owns a VAO/VBO/EBO, draws indexed geometry
├─ Camera.h        # first-person fly camera
├─ CMakeLists.txt  # fetches GLFW / GLAD / GLM / Assimp
└─ README.md
```

---

## What I learned

Built one concept at a time, each verified by running it:

`window → triangle → shaders → matrices → 3D & MVP → camera → textures → Phong lighting → lighting maps → light types → indexed geometry (EBO) → model loading → shadow mapping → engine-style architecture`

---

## Acknowledgements

- Structured around the excellent [LearnOpenGL] tutorials
- Built with [GLFW], [GLAD], [GLM], and [Assimp]

[Assimp]: https://github.com/assimp/assimp
[GLFW]: https://www.glfw.org/
[GLAD]: https://github.com/Dav1dde/glad
[GLM]: https://github.com/g-truc/glm
[CMake]: https://cmake.org/
[Git]: https://git-scm.com/
[Python]: https://www.python.org/
[LearnOpenGL]: https://learnopengl.com/
