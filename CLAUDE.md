# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**hw3d** is a DirectX 11 3D graphics engine written in C++ with model loading, rendering, and ImGui integration. It's a Windows-only application using Visual Studio 2022 (v143 toolset).

## Build & Development

**Build**: Open `hw3d.sln` in Visual Studio 2022 and build the solution. Supports Debug/Release and x86/x64 configurations.

**Key Dependencies**:
- DirectX 11 (d3d11, d3dcompiler)
- Assimp (model loading) - located in `hw3d/Assimp/`
- ImGui (UI) - located in `hw3d/Imgui/`
- DirectXMath (math library)

**Shader Compilation**: HLSL shaders (`.hlsl` files) are compiled to `.cso` bytecode. Shaders include:
- `PhongVS.hlsl` / `PhongPS.hlsl` - Phong lighting with optional specular map
- `SolidVS.hlsl` / `SolidPS.hlsl` - Solid color rendering
- `ColorIndexVS.hlsl` / `ColorIndexPS.hlsl` - Color-based picking/indexing

## Architecture

**Core Rendering Pipeline**:
1. `Graphics` - DirectX 11 device/context management, render target setup
2. `Drawable` - Base class for renderable objects; manages bindables (shaders, buffers, etc.)
3. `Bindable` - Abstract base for GPU resources (vertex/index buffers, shaders, constant buffers, samplers)
4. `BindableCodex` - Singleton cache that deduplicates bindables by UID; ensures shared GPU resources

**Model System**:
- `Model` - Loads `.obj` files via Assimp; parses into a tree of `Node` objects
- `Node` - Represents a transform node in the hierarchy; contains `Mesh` pointers and child nodes
- `Mesh` - Drawable that holds bindables (vertex buffer, index buffer, shaders, textures)

**Input & Camera**:
- `Keyboard` / `Mouse` - Raw input handling
- `Camera` - View matrix management
- `PointLight` - Light source with position and color

**UI**:
- `ImguiManager` - ImGui integration
- `App` - Main application loop; manages window, camera, models, and ImGui rendering

## Key Patterns

**Bindable Deduplication**: All bindables (shaders, textures, buffers) are resolved through `Codex::Resolve<T>(gfx, params...)`. The codex generates a UID from parameters and caches the result, ensuring only one instance per unique configuration exists in GPU memory.

**Transform Hierarchy**: `Node` objects form a tree. Each node has a local transform; `Draw()` accumulates transforms down the hierarchy via `accumulatedTransform` parameter.

**Exception Handling**: Custom exception hierarchy (`ChiliException`, `Graphics::HrException`, `Graphics::InfoException`) with detailed error info. Debug builds include `DxgiInfoManager` for DirectX debug layer messages.

## Current State (ImGUI Branch)

Recent work focuses on ImGui integration and model material handling. The codebase uses a codex container for bindable resource sharing. Latest commit addresses a crash issue.

## File Organization

- `hw3d/` - Main source directory
  - `App.*` - Application entry point and main loop
  - `Graphics.*` - DirectX device/context management
  - `Drawable.*` / `Bindable.*` - Rendering abstraction
  - `Mesh.h` / `Model.*` - Model loading and hierarchy
  - `Camera.*` / `PointLight.*` - Scene objects
  - `Window.*` - Win32 window management
  - `*.hlsl` - Shader source files
  - `Assimp/` - Assimp library for model loading
  - `Imgui/` - ImGui library
  - `Models/` - 3D model assets (`.obj` files)
