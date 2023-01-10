#pragma once
#include "core.hpp"
#include "utils.hpp"
#include <stdexcept>

#define P_SPEED 50.f
#define P_ACCELERATION 500
#define B_Y_SPEED 20.f
#define B_INIT_SPEED 40.f
#define P_DRAG .05f

#define BOUND_TOP (1<<0)
#define BOUND_BOTTOM (1<<1)
#define BOUND_LEFT (1<<2)
#define BOUND_RIGHT (1<<3)
#define BOUND_ALL BOUND_TOP | BOUND_BOTTOM | BOUND_LEFT | BOUND_RIGHT

#define ACC_UP (1<<0)
#define ACC_DOWN (1<<1)
#define ACC_LEFT (1<<2)
#define ACC_RIGHT (1<<3)

namespace WinGameAlpha {

typedef uint8_t bound_flags;
typedef uint8_t acceleration_dir;

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

class Kinematic_Object;
class Collider_Boundary;

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
    @return WGA_SUCCESS on success and WGA_FAILURE on failure
*/
wga_err register_collider_object(int collision_group, Kinematic_Object* kobj);
/* Register a kinematic object
    @param collision_group the collision group 
    @param kobj pointer to kinematic object or subclass
*/
void register_kinematic_object(Kinematic_Object* kobj);
/* Register a collider boundary i.e. an aabb collider which is inverted
    @param collision_group the collision group 
    @param bound pointer to boundary object
    @return WGA_SUCCESS on success and WGA_FAILURE on failure
*/
wga_err register_collider_boundary(int collision_group, Collider_Boundary* bound);

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

float getXoffset(){
    return m_x_offset;
}
float getYoffset(){
    return m_y_offset;
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
Collider(aabb_bounds *bounds, bool is_hard_collider, vector<int> target_collision_groups)
: m_bounds(*bounds), hard_collider(is_hard_collider), m_target_collision_groups(target_collision_groups){};

aabb_bounds getBounds(){
    return m_bounds;
}

private:
vector<int> m_target_collision_groups;
aabb_bounds m_bounds;
bool hard_collider = true;
};

class Kinematic_Object {
friend class Entity_Physics;
public:

/* Object to process kinematics and collisions
    @param physics a pointer to the entity physics manager.
    @param init_prop a struct with the initial position velocity and acceleration.
    @param collision_group all collider objects within this group can collide with each other, groups must be declared contiguously i.e. group 0 must exist before group 1
    @param bound_data a struct containing aabb bound coordinates, if bound_data is NULL no collider will be set.
    @param is_hard_collider whether the collider will hard collide with its targets or not
    @param target_collider_groups a vector of the target collider groups
*/
Kinematic_Object(shared_ptr<Entity_Physics> physics, kinematic_initial_properties* init_prop, int collision_group, aabb_bounds* bound_data, bool is_hard_collider, vector<int> target_collider_groups);
Kinematic_Object(shared_ptr<Entity_Physics> physics, kinematic_initial_properties* init_prop);

~Kinematic_Object(){
    m_collider.reset();
}

/* Called every dt tick to process kinematic movement 
    @param dt delta time per frame
*/
virtual void tick(float dt){};

/* Called when a collision is detected 
    @param other_type the type of collider the object has collided with
*/
virtual void onCollision(const collider_type other_type, void* other_collider_ptr, bound_flags active_flags){};

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