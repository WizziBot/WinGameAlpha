#pragma once

#include "core.hpp"
#include "physics.hpp"
#include "renderer.hpp"

namespace WinGameAlpha{
    
class Player : public Kinematic_Object, public Render_Object {
friend class Drawer;
public:

/* Ball - kinematic object and render object
    @param physics a pointer to the entity physics manager.
    @param drawer drawer object instantiated on application side
    @param initial_properties a struct with the initial position velocity and acceleration of the kinematic object.
    @param collision_group all collider objects within this group can collide with each other, groups must be contiguous i.e. group 0 must exist before group 1
    @param target_collision_groups the collision groups for which this object will register collisions i.e. collide against
    @param bound_data a struct containing aabb bound coordinates
    @param render_matrix a pointer to a render matrix instance, note that the render matrix is not copied
    @param render_layer the id of the render layer of the object where the render objects within the layer will be rendered together,
    the render layers must be declared contiguously i.e. layer 0 must exist before layer 1
*/
Player(shared_ptr<Entity_Physics> physics, shared_ptr<Drawer> drawer,kinematic_initial_properties* initial_properties, int collision_group, vector<int> target_collision_groups, aabb_bounds* bound_data, shared_ptr<Render_Matrix> render_matrix, int render_layer) 
: Kinematic_Object(physics,initial_properties,collision_group,bound_data,false,target_collision_groups), Render_Object(drawer, render_matrix, render_layer, true), restricted_dir(0) {};

// Physics
void onCollision(const collider_type other_type, void* other_collider_ptr, bound_flags active_flags, int other_collider_group) override;
void tick(float dt) override;
void accelerate(acceleration_dir dir_flags);

// Rendering
draw_pos draw_get_pos() override{
    draw_pos pos = {m_posX,m_posY};
    return pos;
};

private:

acceleration_dir restricted_dir;
float cooldown_timer=0;
float shake_timer=2.f;
bool shaking = false;

};

}