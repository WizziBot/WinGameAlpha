#pragma once
#include "core.hpp"
#include "utils.hpp"
#include <stdexcept>

#define P_SPEED 50.f
#define P_ACCELERATION 500
#define B_Y_SPEED 25.f
#define B_INIT_SPEED 50.f
#define P_DRAG .05f

#define BOUND_TOP (1<<0)
#define BOUND_BOTTOM (1<<1)
#define BOUND_LEFT (1<<2)
#define BOUND_RIGHT (1<<3)
#define BOUND_ALL BOUND_TOP | BOUND_BOTTOM | BOUND_LEFT | BOUND_RIGHT

typedef uint8_t bound_flags;

namespace WinGameAlpha {

enum collider_type{
    OBJECT_COLLIDER,
    COLLIDER_BOUNDARY
};

/* Centre aligned aabb bounds*/
struct aabb_bounds{
    float half_width;
    float half_height;
};

struct kinematic_initial_properties{
    float posY=0;
    float posX=0;
    float dy=0;
    float dx=0;
    float ddy=0;
    float ddx=0;
};

class Collider_Group{
public:
vector<Kinematic_Object*> k_objects;
vector<Collider_Boundary*> bounds;
};

class Entity_Physics {
public:

~Entity_Physics();

/* Process kinematics and collisions on all registered objects
    @param dt the delta time of the frame
*/
void physics_tick(const float dt);

/* Register a kinematic object with a collider
    @param collision_group the collision group 
    @param kobj pointer to kinematic object or subclass
*/
void register_collider_object(int collision_group, Kinematic_Object* kobj);
/* Register a kinematic object
    @param collision_group the collision group 
    @param kobj pointer to kinematic object or subclass
*/
void register_kinematic_object(Kinematic_Object* kobj);
/* Register a collider boundary i.e. an aabb collider which is inverted
    @param collision_group the collision group 
    @param bound pointer to boundary object
*/
void register_collider_boundary(int collision_group, Collider_Boundary* bound);

/* Returns the size of the collision_groups vector*/
int getCollisionGoupsSize(){
    return collision_groups.size();
}

private:

vector<Kinematic_Object*> kinematic_objects;
vector<Collider_Group> collision_groups;

/* Processes the kinematics of all registered kinematic objects*/
void process_kinematics(const float dt);
void process_collisions();

};

class Collider_Boundary {
friend class Entity_Physics;
public:

/* Boundary which triggers collide events on collider objects
    @param x_offset the x offset from 0,0 of the boundary
    @param y_offset the y offset from 0,0 of the boundaryyyy
    @param bounds the aabb bounds of the collider
    @param flags the flags BOUND_XXX which indicate whether the top, bottom, left and right sides should trigger a collision
*/
Collider_Boundary(float x_offset, float y_offset, aabb_bounds bounds,bound_flags flags)
: m_x_offset(x_offset), m_y_offset(y_offset), m_bounds(bounds), m_flags(flags){}

aabb_bounds getBounds(){
    return m_bounds;
};
bound_flags getFlags(){
    return m_flags;
}

private:
bound_flags m_flags;
aabb_bounds m_bounds;
float m_x_offset;
float m_y_offset;
};


class Collider {
friend class Entity_Physics;
public:
Collider(aabb_bounds *bounds)
: m_bounds(*bounds) {};

aabb_bounds getBounds(){
    return m_bounds;
}

private:
aabb_bounds m_bounds;
};

class Kinematic_Object {
friend class Entity_Physics;
public:

/* Object to process kinematics and collisions
    @param physics a pointer to the entity physics manager.
    @param initial_properties a struct with the initial position velocity and acceleration.
    @param collision_group all collider objects within this group can collide with each other, groups must be declared contiguously i.e. group 0 must exist before group 1
    @param bound_data a struct containing aabb bound coordinates, if bound_data is NULL no collider will be set.
*/
Kinematic_Object(shared_ptr<Entity_Physics> physics, kinematic_initial_properties* init_prop, int collision_group, aabb_bounds* bound_data);
Kinematic_Object(shared_ptr<Entity_Physics> physics, kinematic_initial_properties* init_prop);

~Kinematic_Object(){
    m_collider.reset();
}

/* Called every dt tick to process kinematic movement */
virtual void tick(const float dt){
    cout << "TICK: THIS SHOULD NEVER RUN" << endl;
};

/* Called when a collision is detected 
    @param other_type the type of collider the object has collided with
*/
virtual void onCollision(collider_type other_type, aabb_bounds other_bounds, bound_flags flags){
    cout << "ONCOLLISION: THIS SHOULD NEVER RUN" << endl;
};

shared_ptr<Collider> getCollider(){
    return m_collider;
}

protected:

shared_ptr<Collider> m_collider;

float m_posY=0;
float m_posX=0;
float m_dy=0;
float m_dx=0;
float m_ddy=0;
float m_ddx=0;

};

}