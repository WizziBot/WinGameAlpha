#include "core.hpp"
#include "renderer.hpp"
#include "render_objects.hpp"

namespace WinGameAlpha{

extern bool running;

using std::cout;
using std::cerr;
using std::endl;
using std::string;

#define RNDERR(msg) {cout << "Renderer Error: " << msg << endl; \
                    return WGA_FAILURE;}

Drawer::Drawer(wga_err* drawer_err){
    wga_err err;
#ifdef USING_OPENCL
    err = init_opencl();
#else
    err = WGA_SUCCESS;
#endif
    *drawer_err = err;
}

Drawer::~Drawer(){
#ifdef USING_OPENCL
    clFlush(queue);
    clReleaseCommandQueue(queue);

    clReleaseMemObject(src_buf);
    clReleaseMemObject(matrix_data_buf);
    clReleaseMemObject(matrix_buf);

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
    if (!running) return;
#ifdef USING_OPENCL
    cl_int err = 0;
    clEnqueueUnmapMemObject(queue, src_buf, render_state.memory, 0, NULL, NULL);
    cl_event wait_for_draw;
#endif
    clear_screen(m_background_colour);
    vector<vector<Render_Object*> >::iterator layer;
    shared_ptr<Render_Matrix> matrix;
    draw_pos offset;
    for (layer = render_layers.begin(); layer != render_layers.end(); layer++){
        vector<Render_Object*>::iterator render_object;
        for (render_object = (*layer).begin(); render_object != (*layer).end(); render_object++){
            offset = (*render_object)->draw_get_pos();

            matrix = (*render_object)->m_render_matrix;

            float unit_size_x = matrix->m_unit_size_x;
            float unit_size_y = matrix->m_unit_size_y;
            float matrix_half_height = matrix->m_height/2;
            float matrix_half_width = matrix->m_width/2;
            float square_x_init = offset.x - matrix_half_width*unit_size_x + unit_size_x/2;
            float square_y_init = offset.y - matrix_half_height*unit_size_y + unit_size_y/2;
            int unit_size_x_px = floor(render_state.height*(unit_size_x/100.f));
            int unit_size_y_px = floor(render_state.height*(unit_size_y/100.f));
            int x0_i = floor(render_state.height*(square_x_init/100) - render_state.height*(unit_size_x/200.f) + render_state.width/2.f);
            int x1_i = floor(render_state.height*(square_x_init/100) + render_state.height*(unit_size_x/200.f) + render_state.width/2.f);
            int x0 = x0_i;
            int x1 = x1_i;
            int y0 = floor(render_state.height*(square_y_init/100) - render_state.height*(unit_size_y/200.f) + render_state.height/2.f);
            int y1 = floor(render_state.height*(square_y_init/100) + render_state.height*(unit_size_y/200.f) + render_state.height/2.f);
            
#ifdef USING_OPENCL
            x0 = clamp(0, x0, render_state.width);
            x1 = clamp(0, x1, render_state.width);
            y0 = clamp(0, y0, render_state.height);
            y1 = clamp(0, y1, render_state.height);
            if (x0 == x1 || y0 == y1) continue;
            // matrix_data = {minid, maxid, buffer_width, width, height, unit_width, unit_height, x0}
            // matrix_buffer = {.. uints ..}
            matrix_data = (cl_uint*)clEnqueueMapBuffer(queue, matrix_data_buf, CL_TRUE, CL_MAP_WRITE, 0, MATRIX_DATA_BUF_SIZE * sizeof(cl_uint), 0, NULL, NULL, &err);
            OCLEXERR("Failed to map matrix_data pointer onto device buffer.",err);
            matrix_data[0] = x0 + y0*render_state.width;
            matrix_data[1] = x0 + unit_size_x_px*(int)matrix->m_width + render_state.width*(y0 + unit_size_y_px*(int)matrix->m_height - 1);
            matrix_data[2] = render_state.width;
            matrix_data[3] = matrix->m_width;
            matrix_data[4] = matrix->m_height;
            matrix_data[5] = unit_size_x_px;
            matrix_data[6] = unit_size_y_px;
            matrix_data[7] = render_state.width - unit_size_x_px*matrix->m_width;
            matrix_data[8] = x0 % render_state.width;
            matrix_data[9] = y0;
            clEnqueueUnmapMemObject(queue, matrix_data_buf, matrix_data, 0, NULL, NULL);
            // Wait for previous kernel to finish before changing matrix buffer
            if (render_object != (*layer).begin()) clWaitForEvents(1,&wait_for_draw);
            OCLEX(clEnqueueWriteBuffer(queue, matrix_buf, CL_FALSE, 0, matrix->m_height*matrix->m_width*sizeof(cl_uint),matrix->m_matrix, 0, NULL, NULL));
            // Enqueue kernel
            OCLEX(clEnqueueNDRangeKernel(queue, draw_matrix_kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, &wait_for_draw));

#else
            uint32_t* unit_col = matrix->m_matrix;
            for (int y = 0; y < matrix->m_height; y++){
                for (int x = 0; x < matrix->m_width; x++){
                    if (!((*unit_col) & ALPHA_BIT))
                        draw_rect_px(x0,y0,x1,y1,*unit_col);
                    x0 += unit_size_x_px;
                    x1 += unit_size_x_px;
                    unit_col++;
                }
                y0 += unit_size_y_px;
                y1 += unit_size_y_px;
                x0 = x0_i;
                x1 = x1_i;
            }
#endif
        }
    }
#ifdef USING_OPENCL
    clFinish(queue);
    render_state.memory = clEnqueueMapBuffer(queue, src_buf, CL_TRUE, CL_MAP_READ, 0, src_size * sizeof(cl_uint), 0, NULL, NULL, &err);
    OCLEXERR("Failed to map reder state memory onto device source buffer.",err);
#endif
}

void Drawer::clear_screen(uint32_t colour){
    if (!running) return;
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
    x0 = clamp(0, x0, render_state.width);
    x1 = clamp(0, x1, render_state.width);
    y0 = clamp(0, y0, render_state.height);
    y1 = clamp(0, y1, render_state.height);
    if (x0 == x1 || y0 == y1) return;

    uint32_t* pixel = (uint32_t*)render_state.memory;
    for (int y = y0; y < y1 ; y++){
        pixel = (uint32_t*)render_state.memory + x0 + y*render_state.width;
        for (int x = x0; x < x1 ; x++){
            *pixel++ = colour; 
        }
    }
}

void Drawer::draw_rect(float x, float y, float width, float height, uint32_t colour){

    int x0 = floor(render_state.height*(x/100) - render_state.height*(width/200.f) + render_state.width/2.f);
    int x1 = floor(render_state.height*(x/100) + render_state.height*(width/200.f) + render_state.width/2.f);
    int y0 = floor(render_state.height*(y/100) - render_state.height*(height/200.f) + render_state.height/2.f);
    int y1 = floor(render_state.height*(y/100) + render_state.height*(height/200.f) + render_state.height/2.f);

    draw_rect_px(x0,y0,x1,y1,colour);
}

void Drawer::cl_draw_finish(){
#ifdef USING_OPENCL
    clFinish(queue);
#endif
}

Render_Object::Render_Object(shared_ptr<Drawer> drawer, shared_ptr<Render_Matrix> render_matrix, int render_layer, bool is_subclass)
: m_render_layer(render_layer), m_render_matrix(render_matrix){
    if (render_matrix == NULL) throw std::invalid_argument("Renderer Error: The render matrix must not be null");
    if (is_subclass) WGAERRCHECK(drawer->register_render_object(this));
}

Render_Matrix::Render_Matrix(float x_offset, float y_offset, float width, float height, uint32_t* matrix, float unit_size_x, float unit_size_y)
: m_x_offset(x_offset), m_y_offset(y_offset), m_width(width), m_height(height), m_matrix(matrix), m_unit_size_x(unit_size_x), m_unit_size_y(unit_size_y) {
    if (width == 0 || height == 0) throw std::invalid_argument("Renderer Error: The width and height of render matrix must be above 0");
    if (width*height > MAX_MATRIX_SIZE) throw std::invalid_argument("Renderer Error: Matrix exdeeded max dim size");
}

#ifdef USING_OPENCL // Using OpenCL to render

wga_err Drawer::cl_resize(){
    clFinish(queue);
    clEnqueueUnmapMemObject(queue, src_buf, render_state.memory, 0, NULL, NULL);
    clReleaseMemObject(src_buf);
    src_size = render_state.width*render_state.height;
    cl_int err;
    src_buf = clCreateBuffer(context, CL_MEM_HOST_READ_ONLY | CL_MEM_WRITE_ONLY, src_size * sizeof(cl_uint), NULL, &err);
    OCLCHECKERR("Failed to create source buffer on resize.",err);
    render_state.memory = clEnqueueMapBuffer(queue, src_buf, CL_TRUE, CL_MAP_READ, 0, src_size * sizeof(cl_uint), 0, NULL, NULL, &err);
    OCLCHECKERR("Failed to map reder state memory onto device source buffer.",err);
    err = clSetKernelArg(draw_matrix_kernel, 2, sizeof(void*), (void*) &src_buf);
    OCLCHECKERR("Failed to set matrix kernel argument src_buf.",err);
    err = clSetKernelArg(draw_rect_kernel, 1, sizeof(void*), (void*) &src_buf);
    OCLCHECKERR("Failed to set rect kernel argument src_buf.",err);
    return WGA_SUCCESS;
}

wga_err Drawer::cl_draw_rect_px(const int x0, const int y0, const int x1, const int y1,const uint32_t colour){
    cl_int err = 0;

    // Write parameters using buffer map
    // rect_data = {minid,maxid,rect_width,wrap_step,colour}
    rect_data = (cl_uint*)clEnqueueMapBuffer(queue, rect_data_buf, CL_TRUE, CL_MAP_WRITE, 0, RECT_DATA_BUF_SIZE * sizeof(cl_uint), 0, NULL, NULL, &err);
    OCLCHECKERR("Rect: Failed to map rect_data pointer onto device buffer.",err);
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
    OCLCHECKERR("Rect: Failed to map reder state memory onto device source buffer.",err);
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

      err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0x10000, buf, NULL);
      cout << buf << endl;
      OCLCHECKERR("Failed to build program.",err);
    }

    // Create kernels
    draw_rect_kernel = clCreateKernel(program, "draw_rect_kernel", &err);
    draw_matrix_kernel = clCreateKernel(program, "draw_matrix_kernel", &err);
    OCLCHECKERR("Failed to create kernel.",err);

    // Create buffers
    src_buf = clCreateBuffer(context, CL_MEM_HOST_READ_ONLY | CL_MEM_WRITE_ONLY, src_size * sizeof(cl_uint), NULL, &err);
    OCLCHECKERR("Failed to create source buffer.",err);
    matrix_data_buf = clCreateBuffer(context, CL_MEM_HOST_WRITE_ONLY | CL_MEM_READ_ONLY, MATRIX_DATA_BUF_SIZE*sizeof(cl_uint), NULL, &err);
    OCLCHECKERR("Failed to create matrix data buffer.",err);
    matrix_buf = clCreateBuffer(context, CL_MEM_HOST_WRITE_ONLY | CL_MEM_READ_ONLY, MAX_MATRIX_SIZE*sizeof(cl_uint), NULL, &err);
    OCLCHECKERR("Failed to create matrix data buffer.",err);
    rect_data_buf = clCreateBuffer(context, CL_MEM_HOST_WRITE_ONLY | CL_MEM_READ_ONLY, RECT_DATA_BUF_SIZE*sizeof(cl_uint), NULL, &err);
    OCLCHECKERR("Failed to create rect_data buffer.",err);

    // Set kernel args
    err = clSetKernelArg(draw_matrix_kernel, 0, sizeof(cl_uint*), (cl_uint*) &matrix_data_buf);
    OCLCHECKERR("Failed to set matrix kernel argument matrix_data_buf.",err);
    err = clSetKernelArg(draw_matrix_kernel, 1, sizeof(cl_uint*), (cl_uint*) &matrix_buf);
    OCLCHECKERR("Failed to set matrix kernel argument matrix_buf.",err);
    err = clSetKernelArg(draw_matrix_kernel, 2, sizeof(void*), (void*) &src_buf);
    OCLCHECKERR("Failed to set matrix kernel argument src_buf.",err);

    err = clSetKernelArg(draw_rect_kernel, 0, sizeof(cl_uint*), (cl_uint*) &rect_data_buf);
    OCLCHECKERR("Failed to set rect kernel argument rect_data_buf.",err);
    err = clSetKernelArg(draw_rect_kernel, 1, sizeof(void*), (void*) &src_buf);
    OCLCHECKERR("Failed to set rect kernel argument src_buf.",err);

    return WGA_SUCCESS;
}
#endif

}

