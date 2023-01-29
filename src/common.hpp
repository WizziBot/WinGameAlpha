#pragma once

#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44
#define VK_R 0x52

#define btn_down(b) input.buttons[b].down
#define btn_pressed(b) (input.buttons[b].down && input.buttons[b].changed)
#define btn_ctrl_pressed(b) (input.buttons[b].down && input.buttons[b].changed && input.buttons[b].ctrl)
#define btn_released(b) (!input.buttons[b].down && input.buttons[b].changed)

namespace WinGameAlpha{

struct Button_State{
    bool down = false;
    bool changed = false;
    bool ctrl = false;
};

#ifdef ENABLE_MOUSE
struct Mouse_State{
    int x_pos = 0;
    int y_pos = 0;
    bool down = false;
    bool changed = false;
};
#endif

enum Button_Type{
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_KUP,
    BUTTON_KDOWN,
    BUTTON_KLEFT,
    BUTTON_KRIGHT,
    BUTTON_PAUSE,
    BUTTON_RESET,

    BUTTON_COUNT // Relys on enum 0 based incremented
};

struct Input {
    Button_State buttons[BUTTON_COUNT];
#ifdef ENABLE_MOUSE
    Mouse_State mouse_state;
#endif
};

}