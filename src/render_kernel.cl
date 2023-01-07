// Not actual kernel code, kernel located in string within renderer.hpp, just used for docs

// rect_data = {rect_data[0],rect_data[1],rect_data[2],rect_data[3],rect_data[4]}

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
