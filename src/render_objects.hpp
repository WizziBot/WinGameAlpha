#pragma once
#include "core.hpp"

namespace WinGameAlpha {

struct render_rect_properties{
    float x_offset=0;
    float y_offset=0;
    float width;
    float height;
    uint32_t colour;
};

struct draw_pos {
    float x;
    float y;
};

struct unit_dims {
    float x;
    float y;
};


class Drawer;
class Texture_Manager;
class Text_Object;
class Render_Object;

// RENDER MATRIX
class Render_Matrix {
friend class Drawer;
friend class Text_Object;
public:

/* Render matrix is a matrix type texture
    @param x_offset the x_offset from the related object's coordinates
    @param y_offset the y_offset from the related object's coordinates
    @param width the number of elements in width of the matrix - must be an integer
    @param height the number of elements in height of the matrix - must be an integer
    @param matrix a pointer to an array of colours for each unit element
    @param unit_size_x the width of each square unit in relative size
    @param unit_size_y the height of each square unit in relative size
*/
Render_Matrix(float x_offset, float y_offset, float width, float height, uint32_t* matrix, float unit_size_x, float unit_size_y);

void edit_matrix_offset(float x_offset, float y_offset){
    m_x_offset = x_offset;
    m_y_offset = y_offset;
}

void edit_matrix_matrix(float width, float height, uint32_t* matrix){
    m_width = width;
    m_height = height;
    m_matrix = matrix;
}

void edit_matrix_unit_size(float unit_size_x, float unit_size_y){
    m_unit_size_x = unit_size_x;
    m_unit_size_y = unit_size_y;
}

unit_dims get_unit_dims(){
    return (unit_dims){.x=m_unit_size_x,.y=m_unit_size_y};
}

void free_matrix(){
    VirtualFree(m_matrix,0,MEM_RELEASE);
}

private:
float m_unit_size_x;
float m_unit_size_y;
float m_x_offset=0;
float m_y_offset=0;
uint32_t* m_matrix;
float m_width;
float m_height;
};

// RENDER OBJECT
class Render_Object {
friend class Drawer;
public:
/* Render object which contains rectangles to be rendered on each draw() call if registered
    @param drawer a pointer to the drawer instance
    @param render_matrix a pointer to the default render matrix texture
    @param render_layer the id of the render layer of the object where the render objects within the layer will be rendered together,
    the render layers must be declared contiguously i.e. layer 0 must exist before layer 1
    @param is_subclass whether this is a subclass
*/
Render_Object(shared_ptr<Drawer> drawer, Render_Matrix* render_matrix, int render_layer, bool is_subclass);

virtual draw_pos draw_get_pos(){
    return m_draw_pos;
};

void draw_set_pos(draw_pos dpos){
    m_draw_pos = dpos;
}

void set_unit_dims(unit_dims udims){
    r_unit_size_x = udims.x;
    r_unit_size_y = udims.y;
}

void append_render_matrix(Render_Matrix* render_matrix){
    m_render_matrices.push_back(render_matrix);
}

void switch_active_matrix(int index){
    if (index == active_idx) return;
    if (index < m_render_matrices.size() && index >=0){
        m_render_matrix = m_render_matrices.at(index);
        active_idx = index;
    }
}

void set_mask(uint32_t colour){
    is_mask = true;
    mask_colour = colour;
}

protected:
draw_pos m_draw_pos = {0,0};
int m_render_layer;
vector<Render_Matrix*> m_render_matrices;
Render_Matrix* m_render_matrix;
int active_idx = 0;
float r_unit_size_x;
float r_unit_size_y;
bool is_mask = false;
uint32_t mask_colour = 0;

};

class Character_Library {
friend class Texture_Manager;
public:


Render_Matrix* get_character_matrix(char character);

private:
vector<Render_Matrix* > character_list;
int length =0;
};

class Text_Object {
public:

Text_Object(shared_ptr<Drawer> drawer, shared_ptr<Texture_Manager> texture_manager, string text, float offset_x, float offset_y, float unit_size, int char_width,int render_layer);
/* Change the text displayed by the text object
*/
void change_text(string text){
    clean_text();
    set_text(text);
    display();
}

void set_offset(draw_pos dpos){
    m_offset = dpos;
}

/* Use character textures as mask for a colour*/
void set_mask(uint32_t colour){
    use_mask = true;
    mask_colour = colour;
}

private:
draw_pos m_offset;
void set_text(string text);
void clean_text();
void display();
shared_ptr<Drawer> m_drawer;
shared_ptr<Texture_Manager> m_texture_manager;
Character_Library* character_library; 
vector<shared_ptr<Render_Object>> text_characters;
list<Render_Object*>::iterator text_idx;
string text_literal;
float m_unit_size=0;
int m_render_layer;
int m_char_width;
bool use_mask=false;
uint32_t mask_colour = 0;
};

}