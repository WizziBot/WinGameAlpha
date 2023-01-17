#pragma once
#include "core.hpp"
#include "render_objects.hpp"

namespace WinGameAlpha {


class Texture_Manager {
public:
/* Manages and stores render objects and other textures
    @param drawer pointer to drawer object
*/
Texture_Manager(shared_ptr<Drawer> drawer){
    m_drawer = drawer;
}
/* Render object which contains rectangles to be rendered on each draw() call if registered
    @param render_matrix a pointer to the render matrix texture
    @param render_layer the id of the render layer of the object where the render objects within the layer will be rendered together,
    the render layers must be declared contiguously i.e. layer 0 must exist before layer 1
    @return WGA_SUCCESS on succes and WGA_FAILURE on failure
*/
void create_render_object(shared_ptr<Render_Matrix> render_matrix, int render_layer);
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
shared_ptr<Render_Matrix> create_render_matrix(float x_offset, float y_offset, float width, float height, uint32_t* matrix, float unit_size_x, float unit_size_y);
/* Register all objects, must be called AFTER all objects are created, will cause memory errors otherwise. */
wga_err register_all_objects();

/* Read a render matrix texture from a .wgat file
    @param matrix_dst double pointer to the location where the render matrix will be stored
    @param width a pointer to where the width will be stored
    @param height a pointer to where the height will be stored
    @param unit_size a pointer to where the unit size will be stored
    @param file_name the name of the file
*/
static wga_err load_texture(uint32_t** matrix_dst, int* width, int* height, float* unit_size, string file_name);

private:
shared_ptr<Drawer> m_drawer;
vector<Render_Object> render_objects;
vector<shared_ptr<Render_Matrix>> render_matrices;
};

}