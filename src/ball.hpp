#pragma once

#include "core.hpp"
#include "physics.hpp"
#include "renderer.hpp"


namespace WinGameAlpha{

class Ball : public Kinematic_Object, public Render_Object {
friend class Drawer;
public:

Ball(shared_ptr<Entity_Physics> physics, shared_ptr<Drawer> drawer,kinematic_initial_properties* initial_properties, int collision_group, aabb_bounds* bound_data, render_rect_properties* rect_props, int num_rect_props, int render_layer) 
: Kinematic_Object(physics,initial_properties,collision_group,bound_data), Render_Object(drawer, rect_props, num_rect_props, render_layer, true) {};

void onCollision(const collider_type other_type, void* other_collider_ptr, bound_flags active_flags) override;
void tick(float dt) override;
void draw(Drawer* drawer) override;

};

}