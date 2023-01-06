__kernel void draw_rect_kernel(const uint x0,
                               const uint y0,
                               const uint x1,
                               const uint y1,
                               const uint colour,
                               const uint buffer_width,
                               __global uint *buffer)
{
    uint minid = y0 * buffer_width + x0;
    uint maxid = y1 * buffer_width + x1;
    uint gid = get_global_id(0);
    uint overflow = (gid/(x1-x0)) * (buffer_width-x1+x0);
    uint idx = minid + gid + overflow;
    uint stride = get_global_size(0);

    while (idx<maxid){
        buffer[idx] = colour;
        idx = idx + stride + (stride/(x1-x0))*(buffer_width-x1+x0);
    }
}