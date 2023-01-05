#pragma once

// Using OpenCL macro
#define USING_OPENCL

#ifdef USING_OPENCL
// #define CL_HPP_ENABLE_EXCEPTIONS
#define OCL_ERROR_CHECKING
#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#endif

#include "core.hpp"
#include "utils.hpp"
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <string>

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
void cl_update();
#endif
private:
#ifdef USING_OPENCL
cl_platform_id platform;
cl_device_id device;
cl_context context;
cl_command_queue queue;
cl_program program;
cl_kernel draw_rect_kernel;
cl_mem src_buf;
cl_uint *src_ptr;

const char *kernel_source =
"__kernel void saxpy(const global uint *x,   \n\
                    const global uint *y,    \n\
                    const uint colour,       \n\
                    const uint buffer_width, \n\
                    __global uint *buffer)   \n\
{                                            \n\
    uint gid = get_global_id(0);             \n\
    uint stride = get_global_size(0);        \n\
}                                            \n";

wga_err init_opencl();

wga_err cl_draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour);

#endif
};
}