# OpenCL Hello World Examples

This project demonstrates OpenCL usage with standard built-in math functions across different device types (CPU, Nvidia GPU, AMD GPU, Mesa).

## Project Structure

```
opencl-examples/
├── CMakeLists.txt
├── kernel.cl
├── src/
│   ├── hello_opencl_cpu.cpp
│   ├── hello_opencl_nvidia.cpp
│   ├── hello_opencl_amd.cpp
│   ├── hello_opencl_mesa_gpu.cpp
│   └── hello_opencl_mesa_cpu.cpp
└── README.md
```

## Features

- Five separate binaries targeting different OpenCL devices:
  - `hello_opencl_cpu`: Targets CPU devices (PoCL)
  - `hello_opencl_nvidia`: Targets Nvidia GPU devices
  - `hello_opencl_amd`: Targets AMD GPU devices
  - `hello_opencl_mesa_gpu`: Targets Mesa OpenCL GPU devices (Rusticl/Clover)
  - `hello_opencl_mesa_cpu`: Targets Mesa OpenCL CPU devices (Rusticl/Clover)
- Uses the OpenCL C built-in `sqrt()` math function from the [OpenCL C Specification](https://registry.khronos.org/OpenCL/specs/3.0-unified/html/OpenCL_C.html#built-in-math-functions)
- Demonstrates basic OpenCL workflow: context creation, kernel compilation, buffer management, and execution

## Prerequisites

- CMake 3.10 or higher
- C++14 compatible compiler
- OpenCL development headers and libraries
- At least one of the following:
  - CPU OpenCL runtime (e.g., Intel OpenCL Runtime, PoCL)
  - Nvidia GPU with CUDA/OpenCL support
  - AMD GPU with ROCm/OpenCL support
  - Mesa OpenCL support (Rusticl or Clover) for Intel/AMD/other GPUs

### Installing OpenCL

**Fedora/RHEL:**
```bash
sudo dnf install opencl-headers OpenCL-ICD-Loader-devel pocl
```

**Ubuntu/Debian:**
```bash
sudo apt-get install opencl-headers ocl-icd-opencl-dev pocl-opencl-icd
```

**Arch Linux:**
```bash
sudo pacman -S opencl-headers ocl-icd pocl
```

Note: PoCL provides a portable CPU-based OpenCL implementation for testing and development.

### Installing Mesa OpenCL (Optional)

Mesa provides OpenCL support for Intel and AMD GPUs through Rusticl (newer, Rust-based) or Clover (older, Gallium-based).

**Fedora/RHEL:**
```bash
sudo dnf install mesa-libOpenCL
```

**Ubuntu/Debian:**
```bash
sudo apt-get install mesa-opencl-icd
```

**Arch Linux:**
```bash
sudo pacman -S opencl-mesa
```

Note: Mesa OpenCL requires compatible GPU hardware (Intel, AMD, or other Mesa-supported GPUs). Check `clinfo` after installation to verify Mesa is detected.

**Important for Rusticl:** You need to enable your GPU driver by setting the `RUSTICL_ENABLE` environment variable. For Intel GPUs, use `iris`; for AMD GPUs, use `radeonsi`. Example:
```bash
export RUSTICL_ENABLE=iris  # For Intel GPUs
# or
export RUSTICL_ENABLE=radeonsi  # For AMD GPUs
```

For more information, see the [official Mesa Rusticl documentation](https://docs.mesa3d.org/rusticl.html).

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

# Run Mesa GPU version (requires mesa-libOpenCL and RUSTICL_ENABLE)
RUSTICL_ENABLE=iris ./hello_opencl_mesa_gpu  # Use iris for Intel, radeonsi for AMD

# Run Mesa CPU version (requires mesa-libOpenCL)
./hello_opencl_mesa_cpu
```

## How It Works

Each program:
1. Initializes input data (array of squared integers)
2. Finds the appropriate OpenCL device (CPU, Nvidia GPU, AMD GPU, or Mesa GPU)
3. Compiles the kernel from `kernel.cl`
4. Executes the `vector_sqrt` kernel which uses the OpenCL C built-in `sqrt()` function
5. Retrieves and displays the results

The kernel uses `sqrt()`, a standard built-in function defined in the [OpenCL C Specification](https://registry.khronos.org/OpenCL/specs/3.0-unified/html/OpenCL_C.html#built-in-math-functions) as part of the "Built-in Scalar and Vector Argument Math Functions". This function is available on all OpenCL implementations.

## Understanding OpenCL Built-in Functions and libclc

You might notice when running `ldd` on the compiled binaries that they only link against the OpenCL ICD loader:

```bash
$ ldd hello_opencl_cpu
    libOpenCL.so.1 => /lib64/libOpenCL.so.1
    libstdc++.so.6 => /lib64/libstdc++.so.6
    # ... no libclc or other libraries!
```

This is expected behavior. Here's how OpenCL built-in functions work:

### OpenCL Built-in Functions are Defined by the Specification

The `sqrt()` function (and other math functions like `sin()`, `cos()`, etc.) are **defined by the OpenCL C Specification**, not by any particular library. The [OpenCL C 3.0 Specification](https://registry.khronos.org/OpenCL/specs/3.0-unified/html/OpenCL_C.html#built-in-math-functions) defines these as "Built-in Scalar and Vector Argument Math Functions" that must be available in all conforming OpenCL implementations.

### How Implementations Provide Built-in Functions

When you call `clBuildProgram()` in your host code, the OpenCL implementation compiles your kernel and provides implementations of built-in functions:

1. **The compilation process**:
   - The OpenCL driver/compiler compiles your kernel source code
   - During compilation, the implementation provides built-in function implementations
   - These get compiled into the kernel binary (SPIR-V, PTX, or native GPU code)
   - The compiled kernel is what actually runs on the device

2. **Different implementations use different approaches**:
   - Some use **libclc** (an open-source library providing reference implementations)
   - Others use proprietary implementations built into their drivers
   - All must conform to the OpenCL C Specification

### What is libclc?

**libclc** is an open-source library that provides portable implementations of OpenCL built-in functions. It's a **compiler library, not a runtime library**:

- It provides LLVM bitcode implementations of OpenCL built-in functions
- It's used by some OpenCL compilers during kernel compilation
- It's NOT linked at runtime - the implementations get compiled into your kernel
- Your host application only links against `libOpenCL.so` (the ICD loader)

### Why You Don't Need to Install libclc Separately

You might wonder: "Why can I build and run these examples without installing libclc?"

The answer is that **OpenCL runtimes provide their own implementations of built-in functions**:

1. **Build time (CMake)**: Your host application only needs:
   - OpenCL headers (the API declarations)
   - OpenCL ICD Loader library (`libOpenCL.so`)
   - No libclc required!

2. **Runtime (kernel compilation)**: When `clBuildProgram()` runs, the OpenCL implementation provides built-in function implementations. Different vendors do this differently:

   - **PoCL** (CPU runtime): Ships with pre-compiled LLVM bitcode files (`/usr/share/pocl/kernel-*.bc`, ~6MB each) containing implementations of all OpenCL built-in functions (based on libclc). These are automatically linked with your kernel during compilation.

   - **NVIDIA**: CUDA toolkit includes proprietary built-in function implementations in the driver's compiler.

   - **AMD ROCm**: Similarly bundles built-in function implementations with the driver.

   - **Mesa Rusticl**: Uses libclc-based implementations for built-in functions.

3. **The role of libclc**: While libclc is the open-source reference implementation used by many OpenCL compilers, vendors can provide their own implementations. All implementations must conform to the OpenCL C Specification. You only need to install libclc separately if you're developing an OpenCL compiler itself.

You can verify how PoCL provides built-in functions on a system with PoCL installed:
```bash
$ rpm -ql pocl | grep "\.bc$"
/usr/share/pocl/kernel-x86_64-redhat-linux-gnu-avx2.bc
/usr/share/pocl/kernel-x86_64-redhat-linux-gnu-sse2.bc
# ... more architecture-specific bitcode files

$ llvm-dis /usr/share/pocl/kernel-*.bc -o - | grep "define.*sqrt"
# Shows sqrt() implementations in LLVM IR (based on libclc)
```

**Bottom line**: The OpenCL runtime you install (PoCL, NVIDIA drivers, AMD ROCm, Mesa Rusticl, etc.) includes everything needed to compile kernels with OpenCL C built-in functions as defined by the specification. libclc is used internally by some implementations, but you don't need to install it separately.
