#include <CL/cl.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstring>

std::string loadKernel(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open kernel file: " << filename << std::endl;
        exit(1);
    }
    return std::string(std::istreambuf_iterator<char>(file),
                      std::istreambuf_iterator<char>());
}

int main() {
    std::cout << "OpenCL Hello World - Nvidia GPU Device" << std::endl;

    const int ARRAY_SIZE = 10;
    std::vector<float> input(ARRAY_SIZE);
    std::vector<float> output(ARRAY_SIZE);

    // Initialize input data
    for (int i = 0; i < ARRAY_SIZE; i++) {
        input[i] = static_cast<float>(i * i);
    }

    cl_int err;
    cl_uint numPlatforms;
    cl_platform_id platform = NULL;
    cl_device_id device = NULL;

    // Get all platforms
    err = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (err != CL_SUCCESS || numPlatforms == 0) {
        std::cerr << "Failed to get platforms" << std::endl;
        return 1;
    }

    std::vector<cl_platform_id> platforms(numPlatforms);
    clGetPlatformIDs(numPlatforms, platforms.data(), NULL);

    // Find Nvidia platform
    for (cl_uint i = 0; i < numPlatforms; i++) {
        char platformName[256];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platformName), platformName, NULL);
        if (std::strstr(platformName, "NVIDIA") != NULL) {
            platform = platforms[i];
            std::cout << "Found Nvidia platform: " << platformName << std::endl;
            break;
        }
    }

    if (platform == NULL) {
        std::cerr << "Nvidia platform not found" << std::endl;
        return 1;
    }

    // Get GPU device
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to get Nvidia GPU device" << std::endl;
        return 1;
    }

    // Print device name
    char deviceName[256];
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
    std::cout << "Using device: " << deviceName << std::endl;

    // Create context
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to create context" << std::endl;
        return 1;
    }

    // Create command queue
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, NULL, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to create command queue" << std::endl;
        return 1;
    }

    // Load and compile kernel
    std::string kernelSource = loadKernel("kernel.cl");
    const char* kernelSourcePtr = kernelSource.c_str();
    size_t kernelSize = kernelSource.size();

    cl_program program = clCreateProgramWithSource(context, 1, &kernelSourcePtr, &kernelSize, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to create program" << std::endl;
        return 1;
    }

    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to build program" << std::endl;
        char buildLog[4096];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
        std::cerr << "Build log:\n" << buildLog << std::endl;
        return 1;
    }

    cl_kernel kernel = clCreateKernel(program, "vector_sqrt", &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to create kernel" << std::endl;
        return 1;
    }

    // Create buffers
    cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                        ARRAY_SIZE * sizeof(float), input.data(), &err);
    cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                         ARRAY_SIZE * sizeof(float), NULL, &err);

    // Set kernel arguments
    unsigned int count = ARRAY_SIZE;
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);
    clSetKernelArg(kernel, 2, sizeof(unsigned int), &count);

    // Execute kernel
    size_t globalSize = ARRAY_SIZE;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to enqueue kernel" << std::endl;
        return 1;
    }

    // Read results
    clEnqueueReadBuffer(queue, outputBuffer, CL_TRUE, 0, ARRAY_SIZE * sizeof(float),
                       output.data(), 0, NULL, NULL);

    // Display results
    std::cout << "\nResults (using libclc sqrt function):" << std::endl;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        std::cout << "sqrt(" << input[i] << ") = " << output[i]
                  << " (expected: " << std::sqrt(input[i]) << ")" << std::endl;
    }

    // Cleanup
    clReleaseMemObject(inputBuffer);
    clReleaseMemObject(outputBuffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    std::cout << "\nSuccess!" << std::endl;
    return 0;
}
