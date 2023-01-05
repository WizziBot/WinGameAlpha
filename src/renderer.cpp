#include "renderer.hpp"

namespace WinGameAlpha{

using std::cout;
using std::cerr;
using std::endl;
using std::string;

Drawer::Drawer(wga_err& drawer_err){
    wga_err err;
#ifdef USING_OPENCL
    err = init_opencl();
#else
    err = WGA_SUCCESS;
#endif
    drawer_err = err;
}

Drawer::~Drawer(){
}

void Drawer::clear_screen(uint32_t colour){
    uint32_t* pixel = (uint32_t*)render_state.memory;
    for (int y = 0; y < render_state.height; y++){
        for (int x = 0; x < render_state.width; x++){
            *pixel++ = colour; 
        }
    }
}

void Drawer::draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour){
    x0 = clamp(0, x0, render_state.width);
    x1 = clamp(0, x1, render_state.width);
    y0 = clamp(0, y0, render_state.height);
    y1 = clamp(0, y1, render_state.height);

#ifdef USING_OPENCL
    cl_draw_rect_px(x0,y0,x1,y1,colour);
#else
    uint32_t* pixel = (uint32_t*)render_state.memory;
    for (int y = y0; y < y1 ; y++){
        pixel = (uint32_t*)render_state.memory + x0 + y*render_state.width;
        for (int x = x0; x < x1 ; x++){
            *pixel++ = colour; 
        }
    }
#endif
}

void Drawer::draw_rect(float x, float y, float width, float height, uint32_t colour){
    int x0 = floor(render_state.height*(x/100));
    int x1 = floor(render_state.height*(x/100) + render_state.height*(width/100));
    int y0 = floor(render_state.height*(y/100));
    int y1 = floor(render_state.height*(y/100) + render_state.height*(height/100));
    
    draw_rect_px(x0,y0,x1,y1,colour);
}

void Drawer::draw_crect(float x, float y, float width, float height, uint32_t colour){

    int x0 = floor(render_state.height*(x/100) - render_state.height*(width/200.f) + render_state.width/2.f);
    int x1 = floor(render_state.height*(x/100) + render_state.height*(width/200.f) + render_state.width/2.f);
    int y0 = floor(render_state.height*(y/100) - render_state.height*(height/200.f) + render_state.height/2.f);
    int y1 = floor(render_state.height*(y/100) + render_state.height*(height/200.f) + render_state.height/2.f);

    draw_rect_px(x0,y0,x1,y1,colour);
}

#ifdef USING_OPENCL

#define OCLERR(msg) {cout << "OpenCL Error: " << msg << endl; \
                    return WGA_FAILURE;}

#ifdef OCL_ERROR_CHECKING
#define OCLCHECK(arg) if ((err = arg) != CL_SUCCESS) { cout << "OpenCL Internal Error: Code(" << err << ")\n" \
                      << #arg << endl; \
                      return WGA_FAILURE;}
#define OCLCHECKERR(err_no) if (err_no != CL_SUCCESS) { cout << "OpenCL Internal Error: Code(" << err_no << ")" << endl; \
                      return WGA_FAILURE;}
#else
#define OCLCHECK(arg) arg
#define OCLCHECKERR(err)
#endif

wga_err Drawer::cl_draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour){
    return WGA_SUCCESS; //remove this
}

void Drawer::cl_update(){

};

wga_err Drawer::init_opencl(){

    cl_int err;
    // Get a platform.

    cl_uint num_platforms;
    OCLCHECK(clGetPlatformIDs(1, &platform, &num_platforms));
    if (num_platforms == 0) OCLERR("No Available OpenCL platforms.");

    // Get a device (GPU)
    cl_uint num_devices;
    OCLCHECK(clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU , 1, &device, &num_devices));
    if (num_platforms == 0) OCLERR("No Available OpenCL devices.");

    // Compute work sizes (going to need to be recomputed on resize events)
    cl_uint src_size = (cl_uint)render_state.width*render_state.height;
    cl_uint compute_units;
    size_t global_work_size;
    size_t local_work_size;
    size_t num_groups;
    OCLCHECK(clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &compute_units, NULL));
    cl_uint ws = 64;
    global_work_size = compute_units * 7 * ws; // 7 wavefronts per SIMD
    while((src_size / 10) % global_work_size != 0)
        global_work_size += ws;
    local_work_size = ws;
    num_groups = global_work_size/local_work_size;
    printf("GWS: %u, LWS: %u, NSI: %u\n",global_work_size,local_work_size,src_size);

    // Create context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    OCLCHECKERR(err);

    // Create queue
    cl_command_queue_properties prop[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, 0};

    queue = clCreateCommandQueueWithProperties(context, device, prop, &err);
    OCLCHECKERR(err);
    if (queue == NULL) OCLERR("Queue creation failed.");

    // Create program
    program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &err);
    OCLCHECKERR(err);
    // Build program
    err = clBuildProgram(program, 1, &device, "", NULL, NULL);
    // Compiler error info
    if(err != CL_SUCCESS) {
      char buf[0x10000];

      clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0x10000, buf, NULL);
      printf("\n%s\n", buf);
      OCLCHECKERR(err);
    }

    // Create kernels
    draw_rect_kernel = clCreateKernel(program, "draw_rect_kernel", &err);
    OCLCHECKERR(err);
    // Create buffers
    src_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, src_size * sizeof(cl_uint), src_ptr, &err);
    OCLCHECKERR(err);
    

}
#endif

}

