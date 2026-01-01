cmake_minimum_required(VERSION 3.10)
project(winapi_camera CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(winapi_camera
    src/main.cpp
    src/mf_camera.cpp
    src/jpg_storage.cpp
)

target_link_libraries(winapi_camera
    mf
    mfplat
    mfreadwrite
    mfuuid
    ole32
    windowscodecs
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set_target_properties(winapi_camera PROPERTIES OUTPUT_NAME "winapi_camera_x64")
else()
    set_target_properties(winapi_camera PROPERTIES OUTPUT_NAME "winapi_camera_x86")
endif()

if(MSVC)
    # Static link CRT - use /MT instead of /MD
    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    target_compile_options(winapi_camera PRIVATE /W3)
endif()
