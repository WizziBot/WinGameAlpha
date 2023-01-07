#pragma once

#include "core.hpp"
#include "utils.hpp"
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <vector>
using std::vector;

#ifdef USING_OPENCL
// #define CL_HPP_ENABLE_EXCEPTIONS
#define OCL_ERROR_CHECKING
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#define RECT_DATA_BUF_SIZE 5
#endif

namespace WinGameAlpha{

extern Render_State render_state;

class Render_Object {
public:

Render_Object();
~Render_Object();

virtual void draw();

protected:

};

class Drawer {
public:

Drawer(wga_err& drawer_err);
~Drawer();

wga_err draw_objects();

/* Clear window to one colour */
void clear_screen(uint32_t colour);
/* Draw rectangle absolute (pixel) coordinates*/
void draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour);
/* Draw rectangle with normalised (to 100) coordinates with respect to the window height
*/
void draw_rect(float x, float y, float width, float height, uint32_t colour);
/* Centered version of draw_rect where 0,0 is the middle of the window */
void draw_crect(float x, float y, float width, float height, uint32_t colour);
/* Wrapper for clFinish(queue) */
void cl_draw_finish();
/* Resize event */
wga_err cl_resize();

private:
vector<vector<Render_Object*> > render_layers;

#ifdef USING_OPENCL
cl_uint src_size;
size_t global_work_size;
size_t local_work_size;
cl_platform_id platform;
cl_device_id device;
cl_context context;
cl_command_queue queue;
cl_program program;
cl_kernel draw_rect_kernel;

cl_mem src_buf;
cl_mem rect_data_buf;

// Buffer Map into parameter list of kernel
cl_uint *rect_data;

const char *kernel_source = \
"__kernel void draw_rect_kernel(const __global uint *rect_data,\n\
                               __global uint *buffer)\n\
{\n\
    uint rect_data_local[5];\n\
    rect_data_local[0] = rect_data[0];\n\
    rect_data_local[1] = rect_data[1];\n\
    rect_data_local[2] = rect_data[2];\n\
    rect_data_local[3] = rect_data[3];\n\
    rect_data_local[4] = rect_data[4];\n\
    uint gid = get_global_id(0);\n\
    uint overflow = (gid/rect_data_local[2]) * rect_data_local[3];\n\
    uint idx = rect_data_local[0] + gid + overflow;\n\
    uint stride = get_global_size(0);\n\
\n\
    int i = 0;\n\
    while (idx<rect_data_local[1]){\n\
        buffer[idx] = rect_data_local[4];\n\
        idx = rect_data_local[0] + gid + stride*i + ((gid+stride*i)/(rect_data_local[2])) * rect_data_local[3];\n\
        i++;\n\
    }\n\
}";

/* Initialize OpenCL API*/
wga_err init_opencl();

/* Queue draw on OpenCL device*/
wga_err cl_draw_rect_px(const int x0, const int y0, const int x1, const int y1,const uint32_t colour);

#endif
};
}