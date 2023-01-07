#pragma once

#define P_SPEED 50.f
#define P_ACCELERATION 500
#define B_Y_SPEED 25.f
#define B_INIT_SPEED 50.f
#define P_DRAG .05f

namespace WinGameAlpha {

class Entity_Physics {
public:

void physics_tick();

private:

vector<Kinematic_Object*> k_objects;
vector<Collider*> colliders;

void process_kinematics();
void process_collisions();

};

class Collider {
public:

private:

};

class Kinematic_Object {
public:

Kinematic_Object(float posY = 0,float posX = 0, float dy = 0, float dx = 0, float ddy = 0, float ddx = 0)
: m_posY(posY), m_posX(posX), m_dy(dy), m_dx(dx), m_ddy(ddy), m_ddx(ddx){};

~Kinematic_Object(){};

virtual void tick(float dt){};

protected:

float m_posY;
float m_posX;
float m_dy;
float m_dx;
float m_ddy;
float m_ddx;

};

}