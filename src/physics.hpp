#pragma once

#define P_SPEED 50.f
#define P_ACCELERATION 500
#define B_Y_SPEED 25.f
#define B_INIT_SPEED 50.f
#define P_DRAG .05f

namespace WinGameAlpha {


struct aabb_bounds{
    float x0;
    float y0;
    float x1;
    float y1;
};

struct kinematic_initial_properties{
    float m_posY;
    float m_posX;
    float m_dy;
    float m_dx;
    float m_ddy;
    float m_ddx;
};

class Entity_Physics {
public:

Entity_Physics();
~Entity_Physics();

void physics_tick();

/* Register a kinematic object with or without collider
    @param initial_properties a struct with the initial position velocity and acceleration.
    @param bound_data a struct containing aabb bound coordinates, if bound_data is NULL no collider will be set.
*/

// This needs to go and put everything in player constructor
wga_err register_kinematic_object(kinematic_initial_properties* initial_properties, aabb_bounds* bound_data);

private:

vector<Kinematic_Object> k_objects;
vector<Collider_Boundary> collider_bounds;

void process_kinematics();
void process_collisions();

};


class Collider_Boundary {
public:

Collider_Boundary(aabb_bounds bounds)
: m_bounds(bounds){}
~Collider_Boundary(){}

void getBounds();

private:
aabb_bounds m_bounds;
};


class Collider {
public:
Collider(int collision_group)
: m_collision_group(collision_group){}
~Collider(){}

private:
int m_collision_group;
};

class Kinematic_Object {
public:

Kinematic_Object(float posY,float posX, float dy, float dx, float ddy, float ddx, Collider* collider)
: m_posY(posY), m_posX(posX), m_dy(dy), m_dx(dx), m_ddy(ddy), m_ddx(ddx), m_collider(collider){}

~Kinematic_Object(){}

virtual void tick(float dt){}

protected:

Collider* m_collider;

float m_posY;
float m_posX;
float m_dy;
float m_dx;
float m_ddy;
float m_ddx;

};

}