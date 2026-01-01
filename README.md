# 📷 Windows API Camera Capture

<div align="center">

![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

**🎯 零依赖 · 无界面 · 即开即用**

[English](#english) | [中文](#中文)

</div>

---

## 中文

### ✨ 功能特点

- 📸 **无界面拍照** - 后台静默运行，无弹窗
- 🎨 **JPEG 输出** - 支持 0-100 质量调节
- 🚀 **零依赖** - 纯 Windows API，无需安装运行时
- 💾 **体积小巧** - x64 仅 141KB，x86 仅 117KB
- 🖥️ **双架构** - 同时支持 x64 和 x86

### 📦 使用方法

```bash
# 默认质量拍照
winapi_camera_x64.exe

# 调整质量 (0-100)
winapi_camera_x64.exe -q 50      # 低质量，文件更小
winapi_camera_x64.exe -q 95      # 高质量

# 指定输出路径
winapi_camera_x64.exe -o photo.jpg

# 组合使用
winapi_camera_x64.exe -q 80 -o output.jpg

# 查看帮助
winapi_camera_x64.exe -h
```

### 🔧 编译指南

#### 环境要求

| 工具 | 版本 |
|------|------|
| Visual Studio | 2019 或更高 |
| CMake | 3.10+ |
| Windows SDK | 10.0+ |

#### 编译步骤

```powershell
# 📁 克隆项目
git clone https://github.com/yourname/winapi-camera.git
cd winapi-camera

# 🔨 编译 x64
mkdir build\x64
cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /DNDEBUG" -S . -B build\x64
cmake --build build\x64 --config Release

# 🔨 编译 x86
mkdir build\x86
cmake -G "Visual Studio 16 2019" -A Win32 -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /DNDEBUG" -S . -B build\x86
cmake --build build\x86 --config Release
```

#### 输出文件

```
build/
├── x64/Release/winapi_camera_x64.exe  (141 KB)
└── x86/Release/winapi_camera_x86.exe  (117 KB)
```

### 📂 项目结构

```
winapi-camera/
├── 📄 CMakeLists.txt      # 构建配置
├── 📄 README.md           # 说明文档
└── 📁 src/
    ├── 📄 main.cpp        # 主程序
    ├── 📄 mf_camera.cpp   # Media Foundation 摄像头
    ├── 📄 mf_camera.h
    ├── 📄 jpg_storage.cpp # WIC JPEG 编码
    └── 📄 jpg_storage.h
```

### 🔗 系统依赖

| DLL | 说明 | 系统自带 |
|-----|------|:--------:|
| MF.dll | Media Foundation | ✅ |
| MFPlat.DLL | MF Platform | ✅ |
| MFReadWrite.dll | MF Read/Write | ✅ |
| windowscodecs.dll | WIC 图像编码 | ✅ |
| ole32.dll | COM Runtime | ✅ |
| KERNEL32.dll | Windows Kernel | ✅ |

> 💡 所有依赖均为 Windows 7+ 系统自带，无需额外安装

---

## English

### ✨ Features

- 📸 **Headless Capture** - Silent background operation, no popup
- 🎨 **JPEG Output** - Quality adjustable from 0-100
- 🚀 **Zero Dependencies** - Pure Windows API, no runtime required
- 💾 **Lightweight** - x64 only 141KB, x86 only 117KB
- 🖥️ **Dual Architecture** - Supports both x64 and x86

### 📦 Usage

```bash
# Default quality capture
winapi_camera_x64.exe

# Adjust quality (0-100)
winapi_camera_x64.exe -q 50      # Low quality, smaller file
winapi_camera_x64.exe -q 95      # High quality

# Specify output path
winapi_camera_x64.exe -o photo.jpg

# Combined usage
winapi_camera_x64.exe -q 80 -o output.jpg

# Show help
winapi_camera_x64.exe -h
```

### 🔧 Build Guide

#### Requirements

| Tool | Version |
|------|---------|
| Visual Studio | 2019 or later |
| CMake | 3.10+ |
| Windows SDK | 10.0+ |

#### Build Steps

```powershell
# 📁 Clone project
git clone https://github.com/yourname/winapi-camera.git
cd winapi-camera

# 🔨 Build x64
mkdir build\x64
cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /DNDEBUG" -S . -B build\x64
cmake --build build\x64 --config Release

# 🔨 Build x86
mkdir build\x86
cmake -G "Visual Studio 16 2019" -A Win32 -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /DNDEBUG" -S . -B build\x86
cmake --build build\x86 --config Release
```

#### Output Files

```
build/
├── x64/Release/winapi_camera_x64.exe  (141 KB)
└── x86/Release/winapi_camera_x86.exe  (117 KB)
```

### 📂 Project Structure

```
winapi-camera/
├── 📄 CMakeLists.txt      # Build config
├── 📄 README.md           # Documentation
└── 📁 src/
    ├── 📄 main.cpp        # Entry point
    ├── 📄 mf_camera.cpp   # Media Foundation camera
    ├── 📄 mf_camera.h
    ├── 📄 jpg_storage.cpp # WIC JPEG encoder
    └── 📄 jpg_storage.h
```

### 🔗 System Dependencies

| DLL | Description | Built-in |
|-----|-------------|:--------:|
| MF.dll | Media Foundation | ✅ |
| MFPlat.DLL | MF Platform | ✅ |
| MFReadWrite.dll | MF Read/Write | ✅ |
| windowscodecs.dll | WIC Image Codec | ✅ |
| ole32.dll | COM Runtime | ✅ |
| KERNEL32.dll | Windows Kernel | ✅ |

> 💡 All dependencies are built into Windows 7+, no additional installation required

---

<div align="center">

**Made with ❤️ using pure Windows API**

⭐ Star this repo if you find it useful!

</div>
