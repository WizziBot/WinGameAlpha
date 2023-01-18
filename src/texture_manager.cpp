#include "texture_manager.hpp"
#include "renderer.hpp"

namespace WinGameAlpha {

Texture_Manager::~Texture_Manager(){
    for (auto p : m_matrices){
        VirtualFree(p,0,MEM_RELEASE);
    }
}

void Texture_Manager::create_render_object(shared_ptr<Render_Matrix> render_matrix, int render_layer){
    render_objects.push_back(Render_Object(m_drawer,render_matrix,render_layer,false));
}

shared_ptr<Render_Matrix> Texture_Manager::create_render_matrix(float x_offset, float y_offset, float width, float height, uint32_t* matrix, float unit_size_x, float unit_size_y){
    render_matrices.push_back(make_shared<Render_Matrix>(x_offset, y_offset, width, height, matrix, unit_size_x, unit_size_y));
    return render_matrices.back();
}

wga_err Texture_Manager::register_all_objects(){
    wga_err err;
    vector<Render_Object>::iterator iter;
    for (iter = render_objects.begin(); iter != render_objects.end(); iter++){
        shared_ptr<Render_Object> new_obj((Render_Object*)iter.base());
        err = m_drawer->register_render_object(new_obj);
        if (err != WGA_SUCCESS) return WGA_FAILURE;
    }
    return WGA_SUCCESS;
}

wga_err Texture_Manager::load_texture(uint32_t** matrix_dst, int* width, int* height, float* unit_size, string file_name){
    wga_err err;
    FILE* fd = fopen(file_name.c_str(),"r");
    if (fd == NULL) {return WGA_FAILURE;}
    int _width,_height;
    float _unit_size;
    int read = fread(&_width,sizeof(int),1,fd);
    read += fread(&_height,sizeof(int),1,fd);
    read += fread(&_unit_size,sizeof(float),1,fd);
    uint32_t* matrix = (uint32_t*)VirtualAlloc(0,_width*_height*sizeof(uint32_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    read += fread(matrix,sizeof(uint32_t),_width*_height,fd);
    if (read == _width*_height + 3) err = WGA_SUCCESS;
    else err = WGA_FAILURE;
    fclose(fd);
    *width = _width;
    *height = _height;
    *unit_size = _unit_size;
    *matrix_dst = matrix;
    m_matrices.push_back(matrix);
    return err;
}

static inline void flip_array(uint32_t* matrix, int start, int end){
    uint32_t temp;
    int i,j,u;
    for (i = start, j = end-1, u = 0; u < (end-start)/2; i++, j--, u++) {
        temp = matrix[i];
        matrix[i] = matrix[j];
        matrix[j] = temp;
    }  
}

void Texture_Manager::flip_matrix(uint32_t* matrix, int width, int height){
    for (int y=0; y < height; y++){
        flip_array(matrix,y*width,y*width+width);
    }
}

void Texture_Manager::rotate_matrix(uint32_t* matrix, int* width, int* height, int n_rotations){
    int _width=*width,_height=*height;
    int temp;
    uint32_t* temp_matrix = (uint32_t*)VirtualAlloc(0,_width*_height*sizeof(uint32_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    for (int i=0; i < n_rotations; i++){
        uint32_t* unit = temp_matrix;
        for (int x=0; x < _width; x++){
            for (int y=0; y < _height; y++){
                *unit++ = matrix[x + y*_width];
            }
        }
        temp = _width;
        _width = _height;
        _height = temp;
        memcpy(matrix,temp_matrix,sizeof(uint32_t)*_width*_height);
    }
    *width = _width;
    *height = _height;
    VirtualFree(temp_matrix,0,MEM_RELEASE);
}

}