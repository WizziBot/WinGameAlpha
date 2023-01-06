// Not actual kernel code, kernel located in string within renderer.hpp, just used for docs

__kernel void draw_rect_kernel(__global uint *x0,
                               __global uint *y0,
                               __global uint *x1,
                               __global uint *y1,
                               __global uint *colour,
                               const uint buffer_width,
                               __global uint *buffer)
{
    uint minid = *y0 * buffer_width + *x0;
    uint maxid = *y1 * buffer_width + *x1;
    uint gid = get_global_id(0);
    uint rect_width = *x1-*x0;
    uint wrap_step = (buffer_width-*x1+*x0);
    uint overflow = (gid/(rect_width)) * wrap_step;
    uint idx = minid + gid + overflow;
    uint stride = get_global_size(0);

    while (idx<maxid){
        buffer[idx] = *colour;
        idx = idx + stride + ((stride)/(rect_width))*wrap_step;
    }
}