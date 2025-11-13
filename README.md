# OpenCL Hello World Examples

This project demonstrates OpenCL usage with libclc math functions across different device types (CPU, Nvidia GPU, AMD GPU).

## Project Structure

```
opencl-examples/
├── CMakeLists.txt
├── kernel.cl
├── src/
│   ├── hello_opencl_cpu.cpp
│   ├── hello_opencl_nvidia.cpp
│   └── hello_opencl_amd.cpp
└── README.md
```

## Features

- Three separate binaries targeting different OpenCL devices:
  - `hello_opencl_cpu`: Targets CPU devices
  - `hello_opencl_nvidia`: Targets Nvidia GPU devices
  - `hello_opencl_amd`: Targets AMD GPU devices
- Uses libclc's `sqrt()` math function in the OpenCL kernel
- Demonstrates basic OpenCL workflow: context creation, kernel compilation, buffer management, and execution

## Prerequisites

- CMake 3.10 or higher
- C++14 compatible compiler
- OpenCL development headers and libraries
- At least one of the following:
  - CPU OpenCL runtime (e.g., Intel OpenCL Runtime, PoCL)
  - Nvidia GPU with CUDA/OpenCL support
  - AMD GPU with ROCm/OpenCL support

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running

From the build directory:

```bash
# Run CPU version
./hello_opencl_cpu

# Run Nvidia GPU version
./hello_opencl_nvidia

# Run AMD GPU version
./hello_opencl_amd
```

## How It Works

Each program:
1. Initializes input data (array of squared integers)
2. Finds the appropriate OpenCL device (CPU, Nvidia GPU, or AMD GPU)
3. Compiles the kernel from `kernel.cl`
4. Executes the `vector_sqrt` kernel which uses libclc's `sqrt()` function
5. Retrieves and displays the results

The kernel implementation uses the standard libclc math function `sqrt()` which is available on all OpenCL implementations.
