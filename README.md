[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)

Skore is a work in progress 3D/2D game engine with focus on parallelism and high performance using data-oriented design and modern graphics techniques,
source code is made based on c++ subset [Orthodox C++](https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b)
---
## Principles
- Data-oriented design.
- Lightweight
- Be able to deliver out-of-box modding, for the developer or the modding community
- Use of Parallel Programming, taking advantage of the multicore processors.
- Heavily optimized allowing thousands of entities and large maps.
- Plug and play and modular.

## Planned Features
- Multiplatform (Windows, Linux, MacOS).
- Mobile device support (Android, iOS).
- Multithreading ECS using Archetypes
- Vulkan, DX12 and Metal backends using HLSL shaders.
- OpenGL for compatibility mode with older hardware
- Fully customizable Render Graph
- Runtime reflection system
- Custom STL types 
- Asset and level parallel streaming
- Creation Graphs for asset manipulation
- System Graphs (Visual scripting for ECS)
- Native C++ scripting using plugin pattern (no need to recompile the engine)
- C# (using dotnet instead of mono) scripting module.
- Asset and script hot reloading
- Audio.
- Network.
---
## Building
Skore uses cmake to build, all external libraries are included in the build system and compiles together with the engine for each platform
but some platforms like linux might require some library installation like xorg-dev 

```
git clone https://github.com/skoreengine/Skore.git
cd Skore
mkdir Build
cd Build
cmake ..
cmake --build .
```

---
## License
Skore is licensed under Apache License Version 2.0.