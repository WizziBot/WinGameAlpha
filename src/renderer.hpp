#pragma once

// Using OpenCL macro
// #define USING_OPENCL

#ifdef USING_OPENCL
#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/opencl.hpp>
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
    
private:
#ifdef USING_OPENCL
std::vector<cl::Platform> platforms;
cl::Context context;
std::vector<cl::Device> devices;
cl::CommandQueue queue;
cl::Program program;
cl::Kernel kernel;

cl::Buffer bufX;
cl::Buffer bufY;
cl::Buffer bufBuff;
cl_uint buffer_width;
cl_uint colour;

string kernelStr =
"__kernel void saxpy(const global float * x,  \n"
"                    __global float * y,      \n"
"                    const float a)           \n"
"{                                            \n"
"   uint gid = get_global_id(0);              \n"
"   y[gid] = a* x[gid];                       \n"
"}                                            \n";

wga_err init_opencl();

wga_err cl_draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour);
#endif
};
}