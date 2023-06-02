//#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/opencl.h>
#include <CL/cl_platform.h>
#include <fstream>
#include <vector>
#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "../utils/utils.h"


/// w = a * x + b * y
int weight_sum() {
    const int ARRAY_SIZE = 128;

    /// Get device id
    cl_device_id deviceId = getDeviceId();

    /// Create context
    cl_int status;
    cl_context context = clCreateContext(nullptr, 1, &deviceId, nullptr, nullptr, &status);
    CL_ASSERT(status)

    /// Creating program
    cl_program program = createProgram(deviceId, context, R"(D:\tests\test-opencl\vector_ops\kernel.cl)");

    /// Getting kernel
    status = CL_SUCCESS;
    cl_kernel kernel = clCreateKernel(program, "weight_sum", &status);
    CL_ASSERT(status);

    /// Creating buffers
    status = CL_SUCCESS;
    std::vector<float> h_x(ARRAY_SIZE, 4.0);
    cl_mem buffer_x = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, ARRAY_SIZE * sizeof(float), h_x.data(), &status);
    CL_ASSERT(status);
    std::vector<float> h_y(ARRAY_SIZE, 4.0);
    status = CL_SUCCESS;
    cl_mem buffer_y = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, ARRAY_SIZE * sizeof(float), h_y.data(), &status);
    CL_ASSERT(status);
    status = CL_SUCCESS;
    cl_mem buffer_w = clCreateBuffer(context, CL_MEM_WRITE_ONLY, ARRAY_SIZE * sizeof(float), nullptr, &status);
    CL_ASSERT(status);

    /// Set args
    float a = 4.0;
    status = clSetKernelArg(kernel, 0, sizeof(a), (void*)&a);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 1, sizeof(buffer_x), (void*)&buffer_x);
    CL_ASSERT(status);
    float b = 1.0;
    status = clSetKernelArg(kernel, 2, sizeof(b), (void*)&b);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 3, sizeof(buffer_y), (void*)&buffer_y);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 4, sizeof(buffer_w), (void*)&buffer_w);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 5, sizeof(ARRAY_SIZE), (void*)&ARRAY_SIZE);
    CL_ASSERT(status);

    /// Create command queue
    cl_command_queue queue = createQueue(context, deviceId);

    /// Kernel launching
    status = CL_SUCCESS;
    size_t uGlobalWorkSize = ARRAY_SIZE;
    status = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &uGlobalWorkSize, nullptr, 0, nullptr, nullptr);
    CL_ASSERT(status)
    status = clFinish(queue);
    CL_ASSERT(status)

    /// Отображение буфера в память управляющего узла
    status = CL_SUCCESS;
    auto* h_mapped_w = (float*)clEnqueueMapBuffer(queue, buffer_w, CL_TRUE, CL_MAP_READ, 0,
          ARRAY_SIZE * sizeof(float), 0, nullptr, nullptr, &status
    );
    CL_ASSERT(status)

    /// Использование результатов
    for (int i = 0; i < ARRAY_SIZE; ++i)
        std::cout << i << " = " << h_mapped_w[i] << "; ";
    std::cout << std::endl;

    /// Завершение отображения буфера
    clEnqueueUnmapMemObject(queue, buffer_w, h_mapped_w, 0, nullptr, nullptr);

    /// Удаление объектов и освобождение памяти управляющего узла
    clReleaseMemObject(buffer_x);
    clReleaseMemObject(buffer_y);
    clReleaseMemObject(buffer_w);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return  0;
}

/// 2) w = a * Y * x,
int rotate_vector() {
    const int ARRAY_SIZE = 128;

    /// Get device id
    cl_device_id deviceId = getDeviceId();

    /// Create context
    cl_int status;
    cl_context context = clCreateContext(nullptr, 1, &deviceId, nullptr, nullptr, &status);
    CL_ASSERT(status)

    /// Creating program
    cl_program program = createProgram(deviceId, context, R"(D:\tests\test-opencl\vector_ops\kernel.cl)");

    /// Getting kernel
    status = CL_SUCCESS;
    cl_kernel kernel = clCreateKernel(program, "rotate_vector", &status);
    CL_ASSERT(status);

    /// Creating buffers
    status = CL_SUCCESS;
    std::vector<float> h_x(ARRAY_SIZE, 1.0);
    cl_mem buffer_x = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, ARRAY_SIZE * sizeof(float), h_x.data(), &status);
    CL_ASSERT(status);
    std::vector<float> h_y(ARRAY_SIZE * ARRAY_SIZE, 1.0);
    status = CL_SUCCESS;
    cl_mem buffer_Y = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, ARRAY_SIZE * ARRAY_SIZE * sizeof(float), h_y.data(), &status);
    CL_ASSERT(status);
    status = CL_SUCCESS;
    cl_mem buffer_w = clCreateBuffer(context, CL_MEM_WRITE_ONLY, ARRAY_SIZE * sizeof(float), nullptr, &status);
    CL_ASSERT(status);

    /// Set args
    float a = 1.0;
    status = clSetKernelArg(kernel, 0, sizeof(a), (void*)&a);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 1, sizeof(buffer_x), (void*)&buffer_x);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 2, sizeof(buffer_Y), (void*)&buffer_Y);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 3, sizeof(buffer_w), (void*)&buffer_w);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 4, sizeof(ARRAY_SIZE), (void*)&ARRAY_SIZE);
    CL_ASSERT(status);

    /// Create command queue
    cl_command_queue queue = createQueue(context, deviceId);

    /// Kernel launching
    status = CL_SUCCESS;
    size_t uGlobalWorkSize = ARRAY_SIZE;
    status = clEnqueueNDRangeKernel(queue, kernel,
        1, /// work_dim
        nullptr, /// global_work_offset
        &uGlobalWorkSize, /// global_work_size
        nullptr, /// local_work_size
        0, nullptr, nullptr
    );
    CL_ASSERT(status)
    status = clFinish(queue);
    CL_ASSERT(status)

    /// Отображение буфера в память управляющего узла
    status = CL_SUCCESS;
    auto* h_mapped_w = (float*)clEnqueueMapBuffer(queue, buffer_w, CL_TRUE, CL_MAP_READ, 0,
                                                  ARRAY_SIZE * sizeof(float), 0, nullptr, nullptr, &status
    );
    CL_ASSERT(status)

    /// Использование результатов
    for (int i = 0; i < ARRAY_SIZE; ++i)
        std::cout << i << " = " << h_mapped_w[i] << "; ";
    std::cout << std::endl;

    /// Завершение отображения буфера
    clEnqueueUnmapMemObject(queue, buffer_w, h_mapped_w, 0, nullptr, nullptr);

    /// Удаление объектов и освобождение памяти управляющего узла
    clReleaseMemObject(buffer_x);
    clReleaseMemObject(buffer_Y);
    clReleaseMemObject(buffer_w);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return  0;
}

int convert_image_to_grayscale() {
    const char* image_path = R"(D:\tests\test-opencl\image.jpg)";

    static const size_t EXP_NUMBER = 1000;

    cv::Mat image = cv::imread(image_path);
    // cv::resize(image, image, cv::Size(300, 300));
    // cv::resize(image, image, cv::Size(100, 100));
    const unsigned int channels = 3;

    auto start_cv = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < EXP_NUMBER; ++i) {
        cv::Mat grayscaled;
        cv::cvtColor(image, grayscaled, cv::COLOR_BGR2GRAY);
    }
    auto stop_cv = std::chrono::high_resolution_clock::now();
    std::cout << "CV time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop_cv - start_cv).count() / EXP_NUMBER << "\n";

    // cv::imshow("Image", image);
    // cv::waitKey(0);

    /// Get device id
    cl_device_id deviceId = getDeviceId();

    /// Create context
    cl_int status;
    cl_context context = clCreateContext(nullptr, 1, &deviceId, nullptr, nullptr, &status);
    CL_ASSERT(status)

    /// Creating program
    cl_program program = createProgram(deviceId, context, R"(D:\tests\test-opencl\vector_ops\kernel.cl)");

    /// Getting kernel
    status = CL_SUCCESS;
    cl_kernel kernel = clCreateKernel(program, "convert_to_grayscale", &status);
    CL_ASSERT(status);

    /// Creating buffers
    status = CL_SUCCESS;
    cl_mem buffer_input = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        image.rows * image.cols * channels * sizeof(unsigned char), image.data, &status);
    CL_ASSERT(status);

    status = CL_SUCCESS;
    cl_mem buffer_output = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
        image.rows * image.cols * sizeof(unsigned char), nullptr, &status);
    CL_ASSERT(status);

    /// Set args
    unsigned int rows = image.rows;
    unsigned int cols = image.cols;
    status = clSetKernelArg(kernel, 0, sizeof(buffer_input), (void*)&buffer_input);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 1, sizeof(buffer_output), (void*)&buffer_output);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 2, sizeof(rows), (void*)&rows);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 3, sizeof(cols), (void*)&cols);
    CL_ASSERT(status);

    /// Create command queue
    cl_command_queue queue = createQueue(context, deviceId);

    auto start_cl = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < EXP_NUMBER; ++i) {
        /// Kernel launching
        status = CL_SUCCESS;
        size_t uGlobalWorkSize = rows * cols;
        status = clEnqueueNDRangeKernel(queue, kernel,
            1, /// work_dim
            nullptr, /// global_work_offset
            &uGlobalWorkSize, /// global_work_size
            nullptr, /// local_work_size
            0, nullptr, nullptr
        );
        CL_ASSERT(status)
        status = clFinish(queue);
        CL_ASSERT(status)        
    }
    auto stop_cl = std::chrono::high_resolution_clock::now();
    std::cout << "CL time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop_cl - start_cl).count() / EXP_NUMBER << "\n";


    /// Отображение буфера в память управляющего узла
    status = CL_SUCCESS;
    auto* mapped_image = (unsigned char*)clEnqueueMapBuffer(queue, buffer_output, CL_TRUE, CL_MAP_READ, 0,
        rows * cols * sizeof(unsigned char), 0, nullptr, nullptr, &status
    );
    CL_ASSERT(status)

    /// Использование результатов
    cv::Mat transformed_image(image.rows, image.cols, CV_8UC1, mapped_image);
    cv::imshow("Image", transformed_image);
    cv::waitKey(0);

    /// Завершение отображения буфера
    clEnqueueUnmapMemObject(queue, buffer_output, mapped_image, 0, nullptr, nullptr);

    /// Удаление объектов и освобождение памяти управляющего узла
    clReleaseMemObject(buffer_input);
    clReleaseMemObject(buffer_output);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return  0;
}

int convert_image_to_negative() {
    const char* image_path = R"(D:\tests\test-opencl\image.jpg)";

    cv::Mat image = cv::imread(image_path);
    const unsigned int channels = 3;

    // cv::imshow("Image", image);
    // cv::waitKey(0);

    /// Get device id
    cl_device_id deviceId = getDeviceId();

    /// Create context
    cl_int status;
    cl_context context = clCreateContext(nullptr, 1, &deviceId, nullptr, nullptr, &status);
    CL_ASSERT(status)

    /// Creating program
    cl_program program = createProgram(deviceId, context, R"(D:\tests\test-opencl\vector_ops\kernel.cl)");

    /// Getting kernel
    status = CL_SUCCESS;
    cl_kernel kernel = clCreateKernel(program, "convert_to_negative", &status);
    CL_ASSERT(status);

    /// Creating buffers
    status = CL_SUCCESS;
    cl_mem buffer_input = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        image.rows * image.cols * channels * sizeof(unsigned char), image.data, &status);
    CL_ASSERT(status);

    status = CL_SUCCESS;
    cl_mem buffer_output = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
        image.rows * image.cols * channels * sizeof(unsigned char), nullptr, &status);
    CL_ASSERT(status);

    /// Set args
    unsigned int rows = image.rows;
    unsigned int cols = image.cols;
    status = clSetKernelArg(kernel, 0, sizeof(buffer_input), (void*)&buffer_input);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 1, sizeof(buffer_output), (void*)&buffer_output);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 2, sizeof(rows), (void*)&rows);
    CL_ASSERT(status);
    status = clSetKernelArg(kernel, 3, sizeof(cols), (void*)&cols);
    CL_ASSERT(status);

    /// Create command queue
    cl_command_queue queue = createQueue(context, deviceId);

    /// Kernel launching
    status = CL_SUCCESS;
    size_t uGlobalWorkSize = rows * cols;
    status = clEnqueueNDRangeKernel(queue, kernel,
        1, /// work_dim
        nullptr, /// global_work_offset
        &uGlobalWorkSize, /// global_work_size
        nullptr, /// local_work_size
        0, nullptr, nullptr
    );
    CL_ASSERT(status)
    status = clFinish(queue);
    CL_ASSERT(status)

    /// Отображение буфера в память управляющего узла
    status = CL_SUCCESS;
    auto* mapped_image = (unsigned char*)clEnqueueMapBuffer(queue, buffer_output, CL_TRUE, CL_MAP_READ, 0,
        rows * cols * channels * sizeof(unsigned char), 0, nullptr, nullptr, &status
    );
    CL_ASSERT(status)

    /// Использование результатов
    cv::Mat transformed_image(image.rows, image.cols, CV_8UC3, mapped_image);
    cv::imshow("Image", transformed_image);
    cv::waitKey(0);

    /// Завершение отображения буфера
    clEnqueueUnmapMemObject(queue, buffer_output, mapped_image, 0, nullptr, nullptr);

    /// Удаление объектов и освобождение памяти управляющего узла
    clReleaseMemObject(buffer_input);
    clReleaseMemObject(buffer_output);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return  0;
}


int main() {
    // return weight_sum();
    // return rotate_vector();
    return convert_image_to_grayscale();
    // return convert_image_to_negative();
    // return blur_image();
    return  0;
}
