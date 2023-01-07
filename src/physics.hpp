

namespace WinGameAlpha {

class Entity_Physics {
public:

private:

};

class Kinematic_Object {
public:


virtual void tick(float dt);

protected:

float m_posY;
float m_posX;
float m_dy;
float m_dx;
float m_ddy;
float m_ddx;

};

}