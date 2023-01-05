__kernel void draw_rect_kernel(const uint x0,
                               const uint y0,
                               const uint x1,
                               const uint y1,
                               const uint colour,
                               const uint buffer_width,
                               const uint buffer_size,
                               __global uint *buffer)
{
    uint count = (buffer_size/10) / get_global_size(0);
    uint idx = get_global_id(0);
    uint stride = get_global_size(0);
    // for loop over the next 10 pixels and if within bounds, paint that pixel
    int i,u;
    for (i=0; i<count; i++, idx += stride){
        for (u=0; u<10; u++){
            if (buffer[i+u] % buffer_width > x0){

            }
        }
    }
}