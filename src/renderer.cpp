#include "renderer.hpp"

namespace WinGameAlpha{

extern bool running;
extern bool resizing;

using std::cout;
using std::cerr;
using std::endl;
using std::string;

#define RNDERR(msg) {cout << "Renderer Error: " << msg << endl; \
                    return WGA_FAILURE;}

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
#ifdef USING_OPENCL
    clFlush(queue);
    clReleaseCommandQueue(queue);

    clReleaseMemObject(src_buf);
    clReleaseMemObject(rect_data_buf);

    clReleaseKernel(draw_rect_kernel);
    clReleaseDevice(device);
    clReleaseContext(context);
    clReleaseProgram(program);
#endif
}

wga_err Drawer::register_render_object(Render_Object* render_obj){
    if (render_obj->m_render_layer > render_layers.size()){
        RNDERR("Render layers must be contiguous: invalid render layer id.");
    } else if (render_obj->m_render_layer == render_layers.size()){
        vector<Render_Object*> render_objs;
        render_objs.push_back(render_obj);
        render_layers.push_back(render_objs);
    } else {
        render_layers.at(render_obj->m_render_layer).push_back(render_obj);
    }
    return WGA_SUCCESS;
}

void Drawer::draw_objects(){
    clear_screen(m_background_colour);
    vector<vector<Render_Object*> >::iterator layer;
    for (layer = render_layers.begin(); layer != render_layers.end(); layer++){
        // Posible optimisation for OpenCL
        vector<Render_Object*>::iterator render_object;
        for (render_object = (*layer).begin(); render_object != (*layer).end(); render_object++){
            if ((*render_object)->m_is_subclass){
                (*render_object)->draw(this);
            } else {
                vector<render_rect_properties>::iterator rect;
                for (rect = (*render_object)->m_rect_props.begin(); rect != (*render_object)->m_rect_props.end(); rect++){
                    draw_rect(rect->x_offset,rect->y_offset,rect->width,rect->height,rect->colour);
                }
            }
        }

    }
}

void Drawer::clear_screen(uint32_t colour){
    if (resizing || !running) return;
    #ifdef USING_OPENCL
        WGAERRCHECK(cl_draw_rect_px(0,0,render_state.width,render_state.height-1,colour));
    #else
    uint32_t* pixel = (uint32_t*)render_state.memory;
    for (int y = 0; y < render_state.height; y++){
        for (int x = 0; x < render_state.width; x++){
            *pixel++ = colour; 
        }
    }
    #endif
}

void Drawer::set_background_colour(uint32_t colour){
    m_background_colour = colour;
}

void Drawer::draw_rect_px(int x0, int y0, int x1, int y1, uint32_t colour){
    if (resizing || !running) return;
    x0 = clamp(0, x0, render_state.width);
    x1 = clamp(0, x1, render_state.width);
    y0 = clamp(0, y0, render_state.height);
    y1 = clamp(0, y1, render_state.height);
    if (x0 == x1 || y0 == y1) return;

#ifdef USING_OPENCL
    WGAERRCHECK(cl_draw_rect_px(x0,y0,x1,y1,colour));
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

    int x0 = floor(render_state.height*(x/100) - render_state.height*(width/200.f) + render_state.width/2.f);
    int x1 = floor(render_state.height*(x/100) + render_state.height*(width/200.f) + render_state.width/2.f);
    int y0 = floor(render_state.height*(y/100) - render_state.height*(height/200.f) + render_state.height/2.f);
    int y1 = floor(render_state.height*(y/100) + render_state.height*(height/200.f) + render_state.height/2.f);
    // cout << "DR x:"<<x<<" y:" <<y <<" w:"<< width << " h:"<<height << endl <<
    // " x0:"<<x0<<" x1:"<<x1<<" y0"<<y0<<" y1"<<y1<< endl;

    draw_rect_px(x0,y0,x1,y1,colour);
}

void Drawer::cl_draw_finish(){
#ifdef USING_OPENCL
    clFinish(queue);
#endif
}

Render_Object::Render_Object(shared_ptr<Drawer> drawer, render_rect_properties* rect_props, int num_rect_props, int render_layer, bool is_subclass)
: m_render_layer(render_layer), m_is_subclass(is_subclass) {
    if (rect_props == NULL || num_rect_props == 0) throw std::invalid_argument("Renderer Error: There must be at least one rect property");
    for (int i=0;i<num_rect_props;i++){
        m_rect_props.push_back(rect_props[i]);
    }
    WGAERRCHECK(drawer->register_render_object(this));
}

#ifdef USING_OPENCL // Using OpenCL to render

#define OCLERR(msg) {cout << "OpenCL Error: " << msg << endl; \
                    return WGA_FAILURE;}

#ifdef OCL_ERROR_CHECKING
#define OCLCHECK(arg) if ((err = arg) != CL_SUCCESS) { cout << "OpenCL Internal Error: Code(" << err << ")\n" \
                      << #arg << endl; \
                      return WGA_FAILURE;}
#define OCLCHECKERR(msg,err_no) if (err_no != CL_SUCCESS) { cout << "OpenCL Internal Error: "<< msg <<" Code(" << err_no << ")" << endl; \
                      return WGA_FAILURE;}
#else
#define OCLCHECK(arg) arg
#define OCLCHECKERR(err)
#endif

wga_err Drawer::cl_resize(){
    if (resizing || !running) return WGA_SUCCESS;
    clFlush(queue);
    cout << "RESIZE" << endl;
    clReleaseMemObject(src_buf);
    cout << "RESIZE" << endl;
    src_size = render_state.width*render_state.height;
    cl_int err;
    src_buf = clCreateBuffer(context, CL_MEM_HOST_READ_ONLY | CL_MEM_WRITE_ONLY, src_size * sizeof(cl_uint), NULL, &err);
    OCLCHECKERR("Failed to create source buffer on resize.",err);
    return WGA_SUCCESS;
}

wga_err Drawer::cl_draw_rect_px(const int x0, const int y0, const int x1, const int y1,const uint32_t colour){
    // return WGA_SUCCESS;
    cl_int err = 0;

    // Write parameters using buffer map
    // rect_data = {minid,maxid,rect_width,wrap_step,colour}
    rect_data = (cl_uint*)clEnqueueMapBuffer(queue, rect_data_buf, CL_TRUE, CL_MAP_WRITE, 0, RECT_DATA_BUF_SIZE * sizeof(cl_uint), 0, NULL, NULL, &err);
    OCLCHECKERR("Failed to map rect_data pointer onto device buffer.",err);
    rect_data[0] = (cl_uint)y0*render_state.width+x0;
    rect_data[1] = (cl_uint)y1*render_state.width+x1;
    rect_data[2] = (cl_uint)x1-x0;
    rect_data[3] = (cl_uint)render_state.width-x1+x0;
    rect_data[4] = (cl_uint)colour;
    clEnqueueUnmapMemObject(queue, rect_data_buf, rect_data, 0, NULL, NULL);

    // Enqueue Kernel, also unmap src buf so it can me modified
    clEnqueueUnmapMemObject(queue, src_buf, render_state.memory, 0, NULL, NULL);

    OCLCHECK(clEnqueueNDRangeKernel(queue, draw_rect_kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL));
    clFinish(queue);

    render_state.memory = clEnqueueMapBuffer(queue, src_buf, CL_TRUE, CL_MAP_READ, 0, src_size * sizeof(cl_uint), 0, NULL, NULL, &err);
    OCLCHECKERR("Failed to map reder state memory onto device source buffer.",err);

    // Sleep(1000);
    return WGA_SUCCESS;
}


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
    src_size = (cl_uint)render_state.width*render_state.height;
    
    // Compute work wize
    cl_uint compute_units;
    OCLCHECK(clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &compute_units, NULL)); 
    
    cl_uint ws = 64;
    global_work_size = compute_units * 7 * ws; // 7 wavefronts per SIMD
    local_work_size = ws;

    // Create context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    OCLCHECKERR("Failed to create context.",err);
    // Create queue
    cl_command_queue_properties prop[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, 0};

    queue = clCreateCommandQueueWithProperties(context, device, prop, &err);
    OCLCHECKERR("Failed to create command queue.",err);
    if (queue == NULL) OCLERR("Queue creation failed.");

    // Create program
    program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &err);
    OCLCHECKERR("Failed to create program.",err);

    // Build program
    err = clBuildProgram(program, 1, &device, "", NULL, NULL);
    // Compiler error info
    if(err != CL_SUCCESS) {
      char buf[0x10000];

      clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0x10000, buf, NULL);
      cout << buf << endl;
      OCLCHECKERR("Failed to build program.",err);
    }

    // Create kernels
    draw_rect_kernel = clCreateKernel(program, "draw_rect_kernel", &err);
    OCLCHECKERR("Failed to create kernel.",err);

    // Create buffers
    src_buf = clCreateBuffer(context, CL_MEM_HOST_READ_ONLY | CL_MEM_WRITE_ONLY, src_size * sizeof(cl_uint), NULL, &err);
    OCLCHECKERR("Failed to create source buffer.",err);
    rect_data_buf = clCreateBuffer(context, CL_MEM_HOST_WRITE_ONLY | CL_MEM_READ_ONLY, RECT_DATA_BUF_SIZE*sizeof(cl_uint), NULL, &err);
    OCLCHECKERR("Failed to create rect_data buffer.",err);

    // Set kernel args
    err = clSetKernelArg(draw_rect_kernel, 0, sizeof(cl_uint*), (cl_uint*) &rect_data_buf);
    OCLCHECKERR("Failed to set kernel argument rect_data_buf.",err);
    err = clSetKernelArg(draw_rect_kernel, 1, sizeof(void*), (void*) &src_buf);
    OCLCHECKERR("Failed to set kernel argument src_buf.",err);

    return WGA_SUCCESS;
}
#endif

}

