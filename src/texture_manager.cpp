#include "texture_manager.hpp"
#include "renderer.hpp"

namespace WinGameAlpha {

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

wga_err Texture_Manager::load_texture(uint32_t** matrix_dst, int* width, int* height, const char* file_name){
    wga_err err;
    FILE* fd = fopen(file_name,"r");
    if (fd == NULL) {return WGA_FAILURE;}
    int _width,_height;
    int read = fread(&_width,sizeof(int),1,fd);
    read += fread(&_height,sizeof(int),1,fd);
    uint32_t* matrix = (uint32_t*)VirtualAlloc(0,_width*_height*sizeof(uint32_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    read += fread(matrix,sizeof(uint32_t),_width*_height,fd);
    if (read == _width*_height + 2) err = WGA_SUCCESS;
    else err = WGA_FAILURE;
    fclose(fd);
    *width = _width;
    *height = _height;
    *matrix_dst = matrix;
    return err;
}

}