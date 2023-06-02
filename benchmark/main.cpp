#include <CL/opencl.h>
#include <CL/cl_platform.h>
#include <fstream>
#include <vector>
#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "../utils/utils.h"

static constexpr const size_t EXP_NUMBER = 1;
// static constexpr const int MATRIX_SIZE = 64;
static constexpr const int MATRIX_SIZE = 16;

std::vector<float> getMatrix(size_t size) {
    // return std::vector<float>(size);

    std::vector<float> vec;
    for (size_t i = 0; i < size; ++i)
        vec.push_back(rand() % 1000);

    // return {
    //     1, 2, 3, 4,
    //     5, 6, 7, 8,
    //     9, 10, 11, 12,
    //     13, 14, 15, 16
    // };
    return vec;
}

void compute_local(cl_command_queue queue, cl_kernel kernel) {
    cl_int status;
    static const size_t GROUP_SIZE = 4;
    for (size_t i = 0; i < EXP_NUMBER; ++i) {

        size_t global[2] = {MATRIX_SIZE, MATRIX_SIZE};
        size_t local[2] = {GROUP_SIZE, GROUP_SIZE};
        status = clEnqueueNDRangeKernel(queue, kernel,
            2, /// work_dim
            nullptr, /// global_work_offset
            global, /// global_work_size
            local, /// local_work_size
            0, nullptr, nullptr
        );
        CL_ASSERT(status)
        status = clFinish(queue);
        CL_ASSERT(status)
    }
}

int main() {
    /// Get device id
    cl_device_id deviceId = getDeviceId();

    /// Create context
    cl_int status;
    cl_context context = clCreateContext(nullptr, 1, &deviceId, nullptr, nullptr, &status);
    CL_ASSERT(status)

    /// Creating program
    cl_program program = createProgram(deviceId, context, R"(D:\tests\test-opencl\benchmark\kernel.h)");

    /// Getting kernel
    // cl_kernel kernel = clCreateKernel(program, "multiply_global", &status);
    cl_kernel kernel = clCreateKernel(program, "multiply_local", &status);
    CL_ASSERT(status);

    /// Creating buffers
    std::vector<float> h_A = getMatrix(MATRIX_SIZE * MATRIX_SIZE);    
    std::vector<float> h_B = getMatrix(MATRIX_SIZE * MATRIX_SIZE);    

    cl_mem d_A = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, MATRIX_SIZE * MATRIX_SIZE * sizeof(float), h_A.data(), &status);
    CL_ASSERT(status);
    cl_mem d_B = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, MATRIX_SIZE * MATRIX_SIZE * sizeof(float), h_B.data(), &status);
    CL_ASSERT(status);
    cl_mem d_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY,  MATRIX_SIZE * MATRIX_SIZE * sizeof(float), nullptr, &status);
    CL_ASSERT(status);

    /// Set args
    status = clSetKernelArg(kernel, 0, sizeof(d_A), (void*)&d_A);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 1, sizeof(d_B), (void*)&d_B);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 2, sizeof(d_C), (void*)&d_C);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 3, sizeof(MATRIX_SIZE), (void*)&MATRIX_SIZE);
    CL_ASSERT(status);

    /// Create command queue
    cl_command_queue queue = createQueue(context, deviceId);



    /// ---- Global configuration ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
    // auto start_cl = std::chrono::high_resolution_clock::now();
    // for (size_t i = 0; i < EXP_NUMBER; ++i) {
    //     size_t global[2] = {MATRIX_SIZE, MATRIX_SIZE};
    //     size_t local[2] = {1, 1};
    //     status = clEnqueueNDRangeKernel(queue, kernel,
    //         2, /// work_dim
    //         nullptr, /// global_work_offset
    //         global, /// global_work_size
    //         local, /// local_work_size
    //         0, nullptr, nullptr
    //     );
    //     CL_ASSERT(status)
    //     status = clFinish(queue);
    //     CL_ASSERT(status)
    // }
    // auto stop_cl = std::chrono::high_resolution_clock::now();
    // std::cout << "CL time:  " << std::chrono::duration_cast<std::chrono::microseconds>(stop_cl - start_cl).count() / EXP_NUMBER << "\n";



    /// ---- Local configuration ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
    static const size_t GROUP_SIZE = 4;
    auto start_cl = std::chrono::high_resolution_clock::now();
    // for (size_t i = 0; i < EXP_NUMBER; ++i) {

    //     size_t global[2] = {MATRIX_SIZE, MATRIX_SIZE};
    //     size_t local[2] = {GROUP_SIZE, GROUP_SIZE};
    //     status = clEnqueueNDRangeKernel(queue, kernel,
    //         2, /// work_dim
    //         nullptr, /// global_work_offset
    //         global, /// global_work_size
    //         local, /// local_work_size
    //         0, nullptr, nullptr
    //     );
    //     CL_ASSERT(status)
    //     status = clFinish(queue);
    //     CL_ASSERT(status)
    // }
    compute_local(queue, kernel);
    auto stop_cl = std::chrono::high_resolution_clock::now();
    std::cout << "CL time:  " << std::chrono::duration_cast<std::chrono::microseconds>(stop_cl - start_cl).count() / EXP_NUMBER << "\n";



    /// Отображение буфера в память управляющего узла
    auto* h_C = (float*)clEnqueueMapBuffer(queue, d_C, CL_TRUE, CL_MAP_READ, 0,
        MATRIX_SIZE * MATRIX_SIZE * sizeof(float), 0, nullptr, nullptr, &status
    );
    CL_ASSERT(status)

    /// Использование результатов
    // for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; ++i)
    //     std::cout << h_C[i] << ((i + 1) % MATRIX_SIZE == 0 ? "\n" : ", ");
    // std::cout << std::endl;

    auto start_cpu = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < MATRIX_SIZE; ++i) {
        for (size_t j = 0; j < MATRIX_SIZE; ++j) {
            float target = 0;
            float real = h_C[i * MATRIX_SIZE + j];

            for (size_t k = 0; k < MATRIX_SIZE; ++k)
                target += h_A[i * MATRIX_SIZE + k] * h_B[k * MATRIX_SIZE + j];

            // if (target != real)
                printf("Target: %f Real: %f\n", target, real);
        }
    }
    auto stop_cpu = std::chrono::high_resolution_clock::now();
    std::cout << "CPU time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop_cpu - start_cpu).count() << "\n";


    /// Завершение отображения буфера
    clEnqueueUnmapMemObject(queue, d_C, h_C, 0, nullptr, nullptr);

    /// Удаление объектов и освобождение памяти управляющего узла
    clReleaseMemObject(d_A);
    clReleaseMemObject(d_B);
    clReleaseMemObject(d_C);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return  0;
}

