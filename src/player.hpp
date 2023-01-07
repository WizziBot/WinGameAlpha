#include "core.hpp"
#include "physics.hpp"

namespace WinGameAlpha{
    
class Player : public Kinematic_Object {
public:

Player();
~Player();

// also extend to some render object base class
// make friend of drawer?

private:
    void apply_physics(float dt);
    void process_collisions();
};

}