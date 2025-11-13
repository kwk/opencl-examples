cmake_minimum_required(VERSION 3.10)
project(OpenCLHelloWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define OpenCL target version to suppress cl_version.h warning
add_compile_definitions(CL_TARGET_OPENCL_VERSION=300)

find_package(OpenCL REQUIRED)

if(NOT OpenCL_FOUND)
    message(FATAL_ERROR "OpenCL not found. Please install OpenCL development libraries:\n"
            "Fedora/RHEL: sudo dnf install opencl-headers OpenCL-ICD-Loader-devel pocl\n"
            "Ubuntu/Debian: sudo apt-get install opencl-headers ocl-icd-opencl-dev pocl-opencl-icd\n"
            "Arch: sudo pacman -S opencl-headers ocl-icd pocl")
endif()

include_directories(${OpenCL_INCLUDE_DIRS})

# Common kernel file
configure_file(${CMAKE_SOURCE_DIR}/kernel.cl ${CMAKE_BINARY_DIR}/kernel.cl COPYONLY)

# CPU device binary
add_executable(hello_opencl_cpu src/hello_opencl_cpu.cpp)
target_link_libraries(hello_opencl_cpu ${OpenCL_LIBRARIES})

# Nvidia GPU device binary
add_executable(hello_opencl_nvidia src/hello_opencl_nvidia.cpp)
target_link_libraries(hello_opencl_nvidia ${OpenCL_LIBRARIES})

# AMD GPU device binary
add_executable(hello_opencl_amd src/hello_opencl_amd.cpp)
target_link_libraries(hello_opencl_amd ${OpenCL_LIBRARIES})

# Mesa device binary
add_executable(hello_opencl_mesa src/hello_opencl_mesa.cpp)
target_link_libraries(hello_opencl_mesa ${OpenCL_LIBRARIES})
