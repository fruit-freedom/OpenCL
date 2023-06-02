#include <CL/opencl.h>
#include <string>

#define CL_ASSERT(status)                                                                                                  \
{                                                                                                                          \
    if (status != CL_SUCCESS) {                                                                                            \
        std::cerr << "CL assertion error [" << status << "] in file " << __FILE__ << " in line " << __LINE__ << std::endl; \
        exit(EXIT_FAILURE);                                                                                                \
    }                                                                                                                      \
}

std::string read(const std::string& filename);

cl_program createProgram(cl_device_id device_id, cl_context context, const std::string&);

cl_device_id getDeviceId();

cl_command_queue createQueue(cl_context, cl_device_id);
