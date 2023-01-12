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

class Drawer;

// RENDER MATRIX
class Render_Matrix {
friend class Drawer;
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
    @param render_matrix a pointer to the render matrix texture
    @param render_layer the id of the render layer of the object where the render objects within the layer will be rendered together,
    the render layers must be declared contiguously i.e. layer 0 must exist before layer 1
    @param is_subclass whether this is a subclass
*/
Render_Object(shared_ptr<Drawer> drawer, shared_ptr<Render_Matrix> render_matrix, int render_layer, bool is_subclass);

virtual draw_pos draw_get_pos(){
    draw_pos zero = {0,0};
    return zero;
};

protected:
int m_render_layer;
shared_ptr<Render_Matrix> m_render_matrix;

};

}