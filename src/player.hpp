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
    @param bound_data a struct containing aabb bound coordinates, if bound_data is NULL no collider will be set.
*/
Player(shared_ptr<Entity_Physics> physics, shared_ptr<Drawer> drawer,kinematic_initial_properties* initial_properties, int collision_group, aabb_bounds* bound_data, render_rect_properties* rect_props, int num_rect_props, int render_layer) 
: Kinematic_Object(physics,initial_properties,collision_group,bound_data), Render_Object(drawer, rect_props, num_rect_props, render_layer) {};

void onCollision(collider_type other_type, aabb_bounds other_bounds, bound_flags flags) override;
void tick(const float dt) override;
void draw() override;

};

}