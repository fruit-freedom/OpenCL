__kernel void multiply_global(
    __global float* A,
    __global float* B,
    __global float* C,
    unsigned int size
) {
    int x = get_global_id(0); /// col_idx
    int y = get_global_id(1); /// row_idx

    if (x < size && y < size) {
        int sum = 0;
        for (int i = 0 ; i < size ; ++i)
            sum += A[y * size + i] * B[x + i * size];
        C[x + y * size] = sum;
    }
}

__kernel void multiply_local(
    __global float* A,
    __global float* B,
    __global float* C,
    unsigned int size
) {
    const size_t BLOCK_SIZE = 4;
    const int row = get_local_id(0);
    const int col = get_local_id(1);    
    const int globalRow = BLOCK_SIZE * get_group_id(0) + row;
    const int globalCol = BLOCK_SIZE * get_group_id(1) + col;

    __local float Asub[BLOCK_SIZE][BLOCK_SIZE];
    __local float Bsub[BLOCK_SIZE][BLOCK_SIZE];

    float acc = 0.0;
    for (size_t t = 0; t < (size / BLOCK_SIZE); ++t) {

        const size_t tiledRow = t * BLOCK_SIZE;
        const size_t tiledCol = t * BLOCK_SIZE;

        Asub[row][col] = A[globalRow * BLOCK_SIZE + tiledCol + col];
        Bsub[row][col] = B[(tiledRow + row) * BLOCK_SIZE + col];

        barrier(CLK_LOCAL_MEM_FENCE);

        // acc = Bsub[row][col];
        for (int k = 0; k < BLOCK_SIZE; ++k)
            acc += Asub[row][k] * Bsub[k][col];

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    C[globalRow * size + globalCol] = acc;
}
