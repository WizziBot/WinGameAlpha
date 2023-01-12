#pragma once
#include "core.hpp"
#include "app.hpp"

namespace WinGameAlpha {

class Texture_Manager {
public:
/* Manages and stores render objects and other textures
    @param drawer pointer to drawer object
*/
Texture_Manager(shared_ptr<Drawer> drawer){
    m_drawer = drawer;
}
/* Render object which contains rectangles to be rendered on each draw() call if registered (texture manager will register object)
    @param render_matrix a pointer to the render matrix texture
    @param render_layer the id of the render layer of the object where the render objects within the layer will be rendered together,
    the render layers must be declared contiguously i.e. layer 0 must exist before layer 1
    @return WGA_SUCCESS on succes and WGA_FAILURE on failure
*/
wga_err create_render_object(Render_Matrix* render_matrix, int render_layer);
/* Render matrix is a matrix type texture
    @param x_offset the x_offset from the related object's coordinates
    @param y_offset the y_offset from the related object's coordinates
    @param width the number of elements in width of the matrix
    @param height the number of elements in height of the matrix
    @param matrix a pointer to an array of colours for each unit element
    @param unit_size_x the width of each square unit in relative size
    @param unit_size_y the height of each square unit in relative size
    @return A pointer to the created render matrix
*/
Render_Matrix* create_render_matrix(float x_offset, float y_offset, float width, float height, uint32_t* matrix, float unit_size_x, float unit_size_y);

private:
shared_ptr<Drawer> m_drawer;
vector<Render_Object> render_objects;
vector<Render_Matrix> render_matrices;
};

}