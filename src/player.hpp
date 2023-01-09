#pragma once

#include "core.hpp"
#include "physics.hpp"
#include "renderer.hpp"

namespace WinGameAlpha{
    
class Player : public Kinematic_Object, public Render_Object {
friend class Drawer;
public:

/* Player - kinematic object and render object
    @param physics a pointer to the entity physics manager.
    @param drawer drawer object instantiated on application side
    @param initial_properties a struct with the initial position velocity and acceleration of the kinematic object.
    @param collision_group all collider objects within this group can collide with each other, groups must be contiguous i.e. group 0 must exist before group 1
    @param target_collision_groups the collision groups for which this object will register collisions i.e. collide against
    @param bound_data a struct containing aabb bound coordinates
    @param rect_props a pointer to an array of rect properties
    @param num_rect_props the length of the rect_props array
    @param render_layer the id of the render layer of the object where the render objects within the layer will be rendered together,
    the render layers must be declared contiguously i.e. layer 0 must exist before layer 1
*/
Player(shared_ptr<Entity_Physics> physics, shared_ptr<Drawer> drawer,kinematic_initial_properties* initial_properties, int collision_group, vector<int> target_collision_groups, aabb_bounds* bound_data, render_rect_properties* rect_props, int num_rect_props, int render_layer) 
: Kinematic_Object(physics,initial_properties,collision_group,bound_data,true,target_collision_groups), Render_Object(drawer, rect_props, num_rect_props, render_layer, true), restricted_dir(0) {};

void onCollision(const collider_type other_type, void* other_collider_ptr, bound_flags active_flags) override;
void tick(float dt) override;
void draw(Drawer* drawer) override;
void accelerate(acceleration_dir dir_flags);

private:

acceleration_dir restricted_dir;
float cooldown_timer;

};

}