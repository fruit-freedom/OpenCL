#include "utils.h"

#include <iostream>
#include <fstream>

std::string read(const std::string& filename) {
    std::ifstream t(filename);
    return {std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>()};
}

cl_program createProgram(cl_device_id deviceID, cl_context context, const std::string& filepath) {
    cl_int errcode_ret;

    /// Create program
    const auto source = read(filepath);
    const char* source_str = source.data();
    size_t source_size = source.length();
    errcode_ret = CL_SUCCESS;
    cl_program program = clCreateProgramWithSource(context, 1, &source_str, &source_size, &errcode_ret);
    CL_ASSERT(errcode_ret)

    /// Build program
    errcode_ret = clBuildProgram(program, 1, &deviceID, nullptr, nullptr, nullptr);
    if (errcode_ret != CL_SUCCESS) {
        switch (errcode_ret) {
            case CL_INVALID_PROGRAM: printf(" if program is not a valid program object.\n");
                break;
            case CL_INVALID_VALUE: printf(" if device_list is NULL and num_devices is greater than zero, or if device_list is not NULL and num_devices is zero.\n");
                break;
            case CL_INVALID_DEVICE: printf(" if OpenCL devices listed in device_list are not in the list of devices associated with program.\n");
                break;
            case CL_INVALID_BINARY: printf(" if program is created with clCreateWithProgramWithBinary and devices listed in device_list do not have a valid program binary loaded.\n");
                break;
            case CL_INVALID_BUILD_OPTIONS: printf(" if the build options specified by options are invalid.\n");
                break;
            case CL_INVALID_OPERATION: printf(" if the build of a program executable for any of the devices listed in device_list by a previous call to clBuildProgram for program has not completed.\n");
                break;
            case CL_COMPILER_NOT_AVAILABLE: printf(" if program is created with clCreateProgramWithSource and a compiler is not available i.e.CL_DEVICE_COMPILER_AVAILABLE specified in the table of OpenCL Device Queries for clGetDeviceInfo is set to CL_FALSE.\n");
                break;
            case CL_BUILD_PROGRAM_FAILURE: printf(" if there is a failure to build the program executable.This error will be returned if clBuildProgram does not return until the build has completed.\n");
                break;
            case CL_OUT_OF_HOST_MEMORY: printf(" if there is a failure to allocate resources required by the OpenCL implementation on the host.\n");
                break;
        }

        if (errcode_ret == CL_BUILD_PROGRAM_FAILURE) {
            // Determine the size of the log
            size_t log_size;
            clGetProgramBuildInfo(program, deviceID, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);

            // Allocate memory for the log
            char* log = (char*)malloc(log_size);

            // Get the log
            clGetProgramBuildInfo(program, deviceID, CL_PROGRAM_BUILD_LOG, log_size, log, nullptr);

            // Print the log
            printf("%s\n", log);
        }
    }

    CL_ASSERT(errcode_ret)

    return program;
}

cl_device_id getDeviceId() {
    /// Get platforms
    cl_uint uNumPlatforms;
    clGetPlatformIDs(0, NULL, &uNumPlatforms);
    // std::cout << uNumPlatforms << " platforms" << std::endl;
    cl_platform_id* pPlatforms = new cl_platform_id[uNumPlatforms];
    clGetPlatformIDs(uNumPlatforms, pPlatforms, &uNumPlatforms);

    /// Get platform info
    const size_t	size = 128;
    char			param_value[size] = { 0 };
    size_t 			param_value_size_ret = 0;
    for (int i = 0; i < uNumPlatforms; ++i) {
        cl_int res = clGetPlatformInfo(pPlatforms[i], CL_PLATFORM_NAME, size, static_cast<void*>(param_value), &param_value_size_ret);
        // printf("Platform %i name is %s\n", pPlatforms[i], param_value);
        param_value_size_ret = 0;
    }

    /// Get CL device id
    int32_t	platform_id = 0;
    if (uNumPlatforms > 1) {
        platform_id = 0;
    }
    cl_device_id deviceID;
    cl_uint uNumGPU;
    clGetDeviceIDs(pPlatforms[platform_id], CL_DEVICE_TYPE_DEFAULT/*CL_DEVICE_TYPE_GPU*/, 1, &deviceID, &uNumGPU);

    /// Print device info
    param_value_size_ret = 0;
    cl_int res1 = clGetDeviceInfo(deviceID, CL_DEVICE_NAME, size, static_cast<void*>(param_value), &param_value_size_ret);
    // printf("Device %i name is %s\n", deviceID, param_value);

    delete[] pPlatforms;

    return deviceID;
}

cl_command_queue createQueue(cl_context context, cl_device_id deviceId) {
    cl_int status = CL_SUCCESS;
    cl_queue_properties qprop[] = {CL_QUEUE_PROPERTIES, (cl_command_queue_properties)CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, 0};
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, deviceId, qprop, &status);
    CL_ASSERT(status)
}
