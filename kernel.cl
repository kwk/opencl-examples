__kernel void vector_sqrt(__global const float* input,
                          __global float* output,
                          const unsigned int count)
{
    int i = get_global_id(0);
    if (i < count) {
        // Using sqrt() function from libclc
        output[i] = sqrt(input[i]);
    }
}
