#pragma once

#include "core.hpp"
#include "render_objects.hpp"
#include "utils.hpp"

#include <stdint.h>
#include <math.h>
#include <iostream>


#ifdef USING_OPENCL
// #define CL_HPP_ENABLE_EXCEPTIONS
#define OCL_ERROR_CHECKING
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#define MATRIX_DATA_BUF_SIZE 11
#define RECT_DATA_BUF_SIZE 5
#define OCLERR(msg) {cerr << "OpenCL Error: " << msg << endl; \
                    return WGA_FAILURE;}

#ifdef OCL_ERROR_CHECKING
#define OCLCHECK(arg) if ((err = arg) != CL_SUCCESS) { cerr << "OpenCL Internal Error: Code(" << err << ")\n" \
                      << #arg << endl; \
                      return WGA_FAILURE;}
#define OCLCHECKERR(msg,err_no) if (err_no != CL_SUCCESS) { cerr << "OpenCL Internal Error: "<< msg <<" Code(" << err_no << ")" << endl; \
                      return WGA_FAILURE;}
#define OCLEX(arg) if ((err = arg) != CL_SUCCESS) { cerr << "OpenCL Internal Error: Code(" << err << ")\n" \
                      << #arg << endl;\
                      throw std::invalid_argument("OPENCLERR");}
#define OCLEXERR(msg,err_no) if (err_no != CL_SUCCESS) { cerr << "OpenCL Internal Error: "<< msg <<" Code(" << err_no << ")" << endl; \
                      throw std::invalid_argument("OPENCLERR");}

#else
#define OCLCHECK(arg) arg
#define OCLCHECKERR(err)
#define OCLEX(arg) arg
#define OCLEXERR(err)
#endif // OCL_ERROR_CHECKING

#endif

#define MAX_MATRIX_SIZE 128*128*4
#define ALPHA_BIT (uint32_t)(1<<31) //high bit of 32bit unsigned
#define AB ALPHA_BIT
#define RENDER_OBJECTS_ALLOCATE_SIZE 5
#define TEXT_OBJ_ALLOCATE_SIZE 5

namespace WinGameAlpha{

extern Render_State render_state;

// DRAWER
class Drawer {
friend class Render_Object;
friend class Texture_Manager;
friend class Text_Object;
public:

Drawer(wga_err* drawer_err);
~Drawer();

/* Draw all registered render objects*/
void draw_objects();

/* Clear window to one colour */
void clear_screen(uint32_t colour);
/* Draw rectangle with normalised (to 100) coordinates with respect to the window height.
    0,0 is the centre of the window.
    @param x the x position @param y the y position
    @param width the width of the rectangle @param height the height of the rectangle 
    @param colour a 24bit rgb colour value with 8bit padding
*/
void draw_rect(float x, float y, float width, float height, uint32_t colour);

/* Wrapper for clFinish(queue) */
void cl_draw_finish();
/* Resize event */
wga_err cl_resize();
/* Set background to be coloured*/
void set_background_colour(uint32_t colour);

private:

/* Registers render object with the drawer. Gets rendered every call to draw().
    Note: The id of the render layer of the object is a group where the render objects within the layer will be rendered together,
    the render layers must be declared contiguously i.e. layer 0 must exist before layer 1
    @param render_obj a pointer to the render object
    @return WGA_SUCCESS on success and WGA_FAILURE on except
*/
wga_err register_render_object(Render_Object* render_obj);
wga_err register_render_object(Render_Object* render_obj, list<Render_Object*>::iterator& obj_iter);
/* Removes render object from references
    @param render_layer the render layer of the object to remove
    @param index the index of the object within the objects in the render layer
*/
void unregister_render_objects(int render_layer, list<Render_Object*>::iterator start, int size);
/* Draw rectangle absolute (pixel) coordinates*/
void draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour);
uint32_t m_background_colour=0;
vector<list<Render_Object* > > render_layers;

#ifdef USING_OPENCL
cl_uint src_size;
size_t global_work_size;
size_t local_work_size;
cl_platform_id platform;
cl_device_id device;
cl_context context;
cl_command_queue queue;
cl_program program;

// Kernel on OpenCL device
cl_kernel draw_rect_kernel;
cl_kernel draw_matrix_kernel;

// Memory object on OpenCL device
cl_mem src_buf;
cl_mem matrix_data_buf;
cl_mem matrix_buf;
cl_mem rect_data_buf;

// Buffer Map into parameter list of kernel
cl_uint *rect_data;
cl_uint *matrix_data;
cl_uint *matrix_buffer;

const char *kernel_source = \
"__kernel void draw_matrix_kernel(const __global uint *matrix_data,\n\
                               const __global uint *matrix_buffer,\n\
                               __global uint *buffer)\n\
{\n\
    uint minid = matrix_data[0];\n\
    uint maxid = matrix_data[1];\n\
    uint buffer_width = matrix_data[2];\n\
    uint width = matrix_data[3];\n\
    uint height = matrix_data[4];\n\
    uint unit_width = matrix_data[5];\n\
    uint unit_height = matrix_data[6];\n\
    uint wrap_step = matrix_data[7];\n\
    uint x0 = matrix_data[8];\n\
    uint y0 = matrix_data[9];\n\
    uint mask = matrix_data[10];\n\
    \n\
    uint gid = get_global_id(0);\n\
    uint overflow = (gid/(width*unit_width)) * wrap_step;\n\
    uint idx = minid + gid + overflow;\n\
    uint stride = get_global_size(0);\n\
\n\
    int i = 1;\n\
    int matrix_idx;\n\
    while (idx<maxid){\n\
        matrix_idx = ((idx%buffer_width)-x0)/(unit_width) + (((idx/buffer_width) - y0)/(unit_height))*width;\n\
        if (matrix_buffer[matrix_idx] != 0x80000000){\n\
            if (mask == 0x80000000) buffer[idx] = matrix_buffer[matrix_idx];\n\
            else buffer[idx] = mask;\n\
        }\n\
        idx = minid + gid + stride*i + ((gid+stride*i)/(width*unit_width)) * wrap_step;\n\
        i++;\n\
    }\n\
}\n\
__kernel void draw_rect_kernel(const __global uint *rect_data,\n\
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

/* Draw rect on opencl device, deprecated for main textures */
wga_err cl_draw_rect_px(const int x0, const int y0, const int x1, const int y1,const uint32_t colour);

#endif
};
}