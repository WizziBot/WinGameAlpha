#pragma once

#include "core.hpp"
#include "utils.hpp"
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <string>

#ifdef USING_OPENCL
// #define CL_HPP_ENABLE_EXCEPTIONS
#define OCL_ERROR_CHECKING
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#define WORK_SIZE 64
#endif

namespace WinGameAlpha{

extern Render_State render_state;

using std::cout;
using std::cerr;
using std::endl;
using std::string;

class Drawer {
public:

Drawer(wga_err& drawer_err);
~Drawer();

/* Clear window to one colour */
void clear_screen(uint32_t colour);
/* Draw rectangle absolute (pixel) coordinates*/
void draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour);
/* Draw rectangle with normalised (to 100) coordinates with respect to the window height
*/
void draw_rect(float x, float y, float width, float height, uint32_t colour);
/* Centered version of draw_rect where 0,0 is the middle of the window */
void draw_crect(float x, float y, float width, float height, uint32_t colour);
#ifdef USING_OPENCL
/* Update render state buffer*/
wga_err cl_update();
#endif
private:
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
cl_uint *src_ptr;

const char *kernel_source = \
"__kernel void draw_rect_kernel(const uint x0,                      \n\
                               const uint y0,                       \n\
                               const uint x1,                       \n\
                               const uint y1,                       \n\
                               const uint colour,                   \n\
                               const uint buffer_width,             \n\
                               __global uint *buffer)               \n\
{                                                                   \n\
    uint minid = y0 * buffer_width + x0;                            \n\
    uint maxid = y1 * buffer_width + x1;                            \n\
    uint gid = get_global_id(0);                                    \n\
    uint overflow = (gid/(x1-x0)) * (buffer_width-x1+x0);           \n\
    uint idx = minid + gid + overflow;                              \n\
    uint stride = get_global_size(0);                               \n\
                                                                    \n\
    while (idx<maxid){                                              \n\
        buffer[idx] = colour;                                       \n\
        idx = idx + stride + (stride/(x1-x0))*(buffer_width-x1+x0); \n\
    }                                                               \n\
}";

wga_err init_opencl();

wga_err cl_draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour);

#endif
};
}