__kernel void saxpy(const global uint *x,
                    const global uint *y,
                    const uint colour,
                    const uint buffer_width,
                    __global uint *buffer)
{
    uint gid = get_global_id(0);
    uint stride = get_global_size(0);
}