#pragma once

#include "core.hpp"
#include "physics.hpp"
#include "renderer.hpp"


namespace WinGameAlpha{

class Ball : public Kinematic_Object, public Render_Object {
friend class Drawer;
public:

Ball(float posY = 0, float posX = 0, float dy= 0, float dx = B_INIT_SPEED)
: m_posY(posY), m_posX(posX), m_dy(dy), m_dx(dx){};
~Ball(){};

void tick(float dt) override{
    this->m_posY = this->m_posY + this->m_dy * dt + this->m_ddy*dt*dt*.5f;
    this->m_dy = this->m_dy + this->m_ddy * dt - P_DRAG*this->m_dy;
};

void draw() override{

};


/* Kinematics tick */
void apply_physics(float dt);

private:
    float m_posY;
    float m_posX;
    float m_dy;
    float m_dx;
    
    void process_collisions();
};

}