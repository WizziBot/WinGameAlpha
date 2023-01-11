// Not actual kernel code, kernel located in string within renderer.hpp, just used for docs

// matrix_data = {minid, maxid, buffer_width, width, height, unit_width, unit_height, x0}
// matrix_buffer = {.. uints ..}

__kernel void draw_matrix_kernel(const __global uint *matrix_data,
                               const __global uint *matrix_buffer,
                               __global uint *buffer)
{
    uint minid = matrix_data[0];
    uint maxid = matrix_data[1];
    uint buffer_width = matrix_data[2];
    uint width = matrix_data[3];
    uint height = matrix_data[4];
    uint unit_width = matrix_data[5];
    uint unit_height = matrix_data[6];
    uint wrap_step = matrix_data[7];
    uint x0 = matrix_data[8];
    uint y0 = matrix_data[9];
    
    uint gid = get_global_id(0);
    uint overflow = (gid/(width*unit_width)) * wrap_step;
    uint idx = minid + gid + overflow;
    uint stride = get_global_size(0);

    int i = 0;
    int matrix_idx;
    int stride_cutoff;
    while (idx<maxid){
        matrix_idx = (((idx)%buffer_width)-x0)/(unit_width) + ((((idx)/buffer_width) - y0)/(unit_height+1))*width;
        buffer[idx] = matrix_buffer[matrix_idx];
        idx = minid + gid + stride*i + ((gid+stride*i)/(width*unit_width)) * wrap_step;
        i++;
    }
}

__kernel void draw_rect_kernel(const __global uint *rect_data,
                               __global uint *buffer)
{
    uint rect_data_local[5];
    rect_data_local[0] = rect_data[0];
    rect_data_local[1] = rect_data[1];
    rect_data_local[2] = rect_data[2];
    rect_data_local[3] = rect_data[3];
    rect_data_local[4] = rect_data[4];
    uint gid = get_global_id(0);
    uint overflow = (gid/rect_data_local[2]) * rect_data_local[3];
    uint idx = rect_data_local[0] + gid + overflow;
    uint stride = get_global_size(0);

    int i = 0;
    while (idx<rect_data_local[1]){
        buffer[idx] = rect_data_local[4];
        idx = rect_data_local[0] + gid + stride*i + ((gid+stride*i)/(rect_data_local[2])) * rect_data_local[3];
        i++;
    }
}
