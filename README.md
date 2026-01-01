# Windows API Camera Capture

无界面摄像头捕获程序，使用纯 Windows API，零外部依赖。

## 功能

- 无界面调用摄像头拍照
- 输出 JPEG 格式，支持质量调节
- 零外部依赖，可在任何 Windows 7+ 系统直接运行

## 使用方法

```
winapi_camera_x64.exe [options]

Options:
  -q <0-100>  JPEG 质量 (默认: 85)
  -o <path>   输出文件路径 (默认: 自动生成时间戳文件名)
  -h          显示帮助
```

### 示例

```bash
# 默认质量拍照
winapi_camera_x64.exe

# 低质量（文件更小）
winapi_camera_x64.exe -q 50

# 高质量
winapi_camera_x64.exe -q 95

# 指定输出文件名
winapi_camera_x64.exe -o photo.jpg

# 组合使用
winapi_camera_x64.exe -q 80 -o output.jpg
```

## 编译要求

- Windows 10 SDK
- Visual Studio 2019 或更高版本
- CMake 3.10+

## 编译步骤

### 1. 安装依赖

1. 安装 [Visual Studio 2019/2022](https://visualstudio.microsoft.com/)
   - 选择 "使用 C++ 的桌面开发" 工作负载

2. 安装 [CMake](https://cmake.org/download/)
   - 安装时勾选 "Add CMake to system PATH"

### 2. 编译 x64 版本

```cmd
:: 创建构建目录
mkdir build\x64
cd build\x64

:: 配置 CMake (静态链接 CRT，无运行时依赖)
cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /DNDEBUG" ../..

:: 编译
cmake --build . --config Release

:: 输出文件: build\x64\Release\winapi_camera_x64.exe
```

### 3. 编译 x86 版本

```cmd
mkdir build\x86
cd build\x86

cmake -G "Visual Studio 16 2019" -A Win32 -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /DNDEBUG" ../..

cmake --build . --config Release

:: 输出文件: build\x86\Release\winapi_camera_x86.exe
```

### 4. 一键编译脚本

也可以使用 PowerShell 脚本：

```powershell
# x64 编译
$cmakePath = "C:\Program Files\CMake\bin\cmake.exe"
New-Item -ItemType Directory -Path "build\x64" -Force
& $cmakePath -G "Visual Studio 16 2019" -A x64 -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /DNDEBUG" -S . -B build\x64
& $cmakePath --build build\x64 --config Release
```

## 项目结构

```
winapi-camera/
├── CMakeLists.txt      # CMake 构建配置
├── README.md           # 本文档
└── src/
    ├── main.cpp        # 主程序入口
    ├── mf_camera.cpp   # Media Foundation 摄像头模块
    ├── mf_camera.h
    ├── jpg_storage.cpp # JPEG 存储模块 (WIC)
    └── jpg_storage.h
```

## 依赖说明

编译后的 exe 只依赖 Windows 系统 DLL：

| DLL | 说明 |
|-----|------|
| MF.dll | Media Foundation |
| MFPlat.DLL | Media Foundation Platform |
| MFReadWrite.dll | Media Foundation Read/Write |
| ole32.dll | COM Runtime |
| windowscodecs.dll | Windows Imaging Component |
| KERNEL32.dll | Windows Kernel |

所有依赖都是 Windows 7+ 系统自带，无需安装任何运行时。

## 技术实现

- **摄像头访问**: Media Foundation API
- **JPEG 编码**: Windows Imaging Component (WIC)
- **静态链接**: /MT 编译选项，无 VCRUNTIME 依赖
