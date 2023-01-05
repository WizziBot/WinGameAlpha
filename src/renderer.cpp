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
wga_err Drawer::cl_draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour){
    return WGA_SUCCESS; //remove this
}

#define OCLERR(msg) cout << "OpenCL Error: " << msg << endl; \
                     return WGA_FAILURE;

wga_err Drawer::init_opencl(){

    return WGA_FAILURE; // remove this

    // Compare platform vendors and identify AMD platforms
    cl::Platform::get(&platforms);
    if (platforms.size() == 0){
        OCLERR("Unable to retrieve platforms.");
    }
    std::vector<cl::Platform>::iterator iter;
    for(iter = platforms.begin(); iter != platforms.end(); ++iter){
        if(!strcmp((*iter).getInfo<CL_PLATFORM_VENDOR>().c_str(), "Advanced Micro Devices, Inc.")){
            break;
        }
    }
    // Create a context
    cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(*iter)(), 0};
    context = cl::Context(CL_DEVICE_TYPE_GPU, cps);
    
    devices = context.getInfo<CL_CONTEXT_DEVICES>();

}
#endif

}

