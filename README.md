# Lumen
Lumen is a lightweight, minimal game engine designed for **high-performance real-time rendering and physics simulation**. Written primarily in **C**, Lumen focuses on **efficiency, simplicity, and direct hardware control**, while maintaining extensibility for future enhancements.

## Features

- **Minimalist & Efficient** – Built with a low-overhead architecture to maximize performance.
- **Custom Rendering Pipeline** – A lean OpenGL/Vulkan-based renderer for real-time graphics.
- **Bare-Metal Physics Engine** – Custom physics library for rigid-body dynamics and collision handling.
- **Modular & Extensible** – Designed to be easily expandable without unnecessary complexity.
- **Cross-Platform** – Compatible with Windows, Linux, and macOS.

## 🚀 Roadmap

| Stage          | Status  |
|---------------|---------|
| Initial rendering pipeline (OpenGL) | ⏳ In Progress |
| Basic physics engine (rigid bodies, collisions) | ⏳ Planned |
| Entity-Component System (ECS) | ⏳ Planned |
| Scripting and game logic integration | ⏳ Planned |
| Audio and asset management | ⏳ Planned |

## 🛠️ Getting Started

### Prerequisites
- **C Compiler** (GCC/Clang/MSVC)
- **CMake** (Recommended)
- **Vulkan SDK** (For rendering)
- **GLFW** (For windowing and input)

### Build & Run

```sh
git clone https://github.com/yourusername/Lumen.git
cd Lumen
mkdir build && cd build
cmake ..
make
./Lumen
