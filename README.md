# OpenCL Hello World Examples

This project demonstrates OpenCL usage with libclc math functions across different device types (CPU, Nvidia GPU, AMD GPU, Mesa).

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
4. Executes the `vector_sqrt` kernel which uses libclc's `sqrt()` function
5. Retrieves and displays the results

The kernel implementation uses the standard libclc math function `sqrt()` which is available on all OpenCL implementations.

## Understanding libclc

You might notice when running `ldd` on the compiled binaries that none of them link against libclc:

```bash
$ ldd hello_opencl_cpu
    libOpenCL.so.1 => /lib64/libOpenCL.so.1
    libstdc++.so.6 => /lib64/libstdc++.so.6
    # ... no libclc!
```

This is expected behavior because **libclc is a compiler library, not a runtime library**.

### How libclc Works

1. **Compile-time, not runtime**: libclc provides the *source code implementation* of OpenCL built-in functions (like `sqrt()`, `sin()`, `cos()`, etc.). It's used during kernel compilation, not when your host application runs.

2. **The compilation process**: When you call `clBuildProgram()` in your host code:
   - The OpenCL driver/compiler compiles your kernel source code
   - During compilation, the OpenCL compiler uses libclc to provide implementations of built-in functions
   - The libclc code gets compiled into the kernel binary (SPIR-V, PTX, or native GPU code)
   - This compiled kernel is what actually runs on the device

3. **What you link against**: Your host application only links against `libOpenCL.so` - the OpenCL ICD (Installable Client Driver) loader. The ICD loader dispatches calls to the appropriate vendor-specific OpenCL implementation, which includes the compiler that uses libclc.

### Analogy

Think of libclc like compiler headers in C/C++:
- Your C program uses `printf()` from libc, and you link against libc at runtime
- But libclc is more like the compiler's built-in headers - it's used during compilation to generate the final code, not linked at runtime
- The `sqrt()` function in your `kernel.cl` file gets compiled into device-specific instructions when `clBuildProgram()` runs, and libclc provides the implementation that the compiler uses

### Why You Don't Need to Install libclc Separately

You might wonder: "If the kernel uses `sqrt()` from libclc, why can I build and run these examples without installing libclc?"

The answer is that **OpenCL runtimes bundle their own implementations of the standard library**:

1. **Build time (CMake)**: Your host application only needs:
   - OpenCL headers (the API declarations)
   - OpenCL ICD Loader library (`libOpenCL.so`)
   - No libclc required!

2. **Runtime (kernel compilation)**: When `clBuildProgram()` runs, the OpenCL implementation provides built-in function implementations. Different vendors do this differently:

   - **PoCL** (CPU runtime): Ships with pre-compiled LLVM bitcode files (`/usr/share/pocl/kernel-*.bc`, ~6MB each) containing all OpenCL built-in functions. These are automatically linked with your kernel during compilation.

   - **NVIDIA**: CUDA toolkit includes built-in implementations in the driver's compiler.

   - **AMD ROCm**: Similarly bundles standard library implementations with the driver.

3. **libclc is the reference implementation**: While libclc is the open-source reference implementation of OpenCL's standard library, vendors can (and do) provide their own implementations bundled with their drivers. You only need libclc if you're developing an OpenCL compiler itself.

You can verify this on a system with PoCL installed:
```bash
$ rpm -ql pocl | grep "\.bc$"
/usr/share/pocl/kernel-x86_64-redhat-linux-gnu-avx2.bc
/usr/share/pocl/kernel-x86_64-redhat-linux-gnu-sse2.bc
# ... more architecture-specific bitcode files

$ llvm-dis /usr/share/pocl/kernel-*.bc -o - | grep "define.*sqrt"
# Shows sqrt() implementations in LLVM IR
```

**Bottom line**: The OpenCL runtime you install (PoCL, NVIDIA drivers, AMD ROCm, etc.) includes everything needed to compile kernels with standard functions. libclc is only needed by people building OpenCL compilers, not by people using OpenCL.
