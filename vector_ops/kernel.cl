__kernel void memset(__global float* puDst)
{
//    puDst[get_global_id(0)] = get_global_id(0);
    puDst[get_global_id(0)] = 1.0;
}



__kernel void weight_sum(
    float a,
    __global float* x,
    float b,
    __global float* y,
    __global float* w,
    const unsigned int size
) {
    int i = get_global_id(0);
    if(i < size) {
        w[i] = a * x[i] + b * y[i];
    }
}

__kernel void rotate_vector(
    float a,
    __global float* x,
    __global float* Y,
    __global float* w,
    const unsigned int size
) {
    int i = get_global_id(0);
    if (i < size) {
        float res = 0.0;
        for (size_t k = 0; k < size; ++k)
            res += x[k] * Y[i + size * k];

        w[i] = res * a;
    }
}

__kernel void convert_to_grayscale(
    __global unsigned char* input,
    __global unsigned char* output,
    const unsigned int rows,
    const unsigned int cols
) {
    const unsigned int channels = 3;
    int i = get_global_id(0);
    if (i < rows * cols) {
        unsigned char b = input[i * channels + 0];
        unsigned char g = input[i * channels + 1];
        unsigned char r = input[i * channels + 2];
        
        output[i] = (b + g + r) / 3;
    }
}

__kernel void convert_to_negative(
    __global unsigned char* input,
    __global unsigned char* output,
    const unsigned int rows,
    const unsigned int cols
) {
    const unsigned int channels = 3;
    int i = get_global_id(0);
    if (i < rows * cols) {
        output[i * channels + 0] = 255 - input[i * channels + 0];
        output[i * channels + 1] = 255 - input[i * channels + 1];
        output[i * channels + 2] = 255 - input[i * channels + 2];
    }
}

__kernel void blur_image(
    __global unsigned char* input,
    __global unsigned char* output
) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    if (i < 9) {
        unsigned char sum = input[i * 3 + j];
        
        output[i * 3 + j] = input[i * 3 + j];
    }
}