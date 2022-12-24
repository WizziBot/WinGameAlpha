
#include <iostream>
#include "core.hpp"
#include "renderer.hpp"
#include "app.hpp"
#include "common.hpp"

/* DEFINES */
#define W_WIDTH APP_WIDTH
#define W_HEIGHT APP_HEIGHT
#define C_ONMSG "WinGameAlpha: Started"
// #define DEBUG_MODE


namespace WinGameAlpha {

Render_State render_state;

static bool running = true;
extern bool changes;

LRESULT window_callback(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam){
    LRESULT result = 0;
    switch(Msg){
        case WM_CLOSE:
        case WM_DESTROY: {
            running = false;
        } break;

        case WM_SIZE: {
            RECT rect;
            GetClientRect(hWnd,&rect);
            render_state.width = rect.right - rect.left;
            render_state.height = rect.bottom - rect.top;
            int buffer_size = render_state.width*render_state.height*sizeof(uint32_t);

            if (render_state.memory) VirtualFree(render_state.memory,0,MEM_RELEASE);
            render_state.memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (render_state.memory == NULL){
                std::cerr << "Memory assignment failure: Render state" << std::endl;
            }
    
            render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
            render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
            
            // Render every window size update
            render_update();
        } break;

        default: {
            result = DefWindowProc(hWnd,Msg,wParam,lParam);
        }
    }
    return result;
}
}

using namespace WinGameAlpha;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
    
    std::cout << C_ONMSG << std::endl;

    // Init bitmap header
    render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
    render_state.bitmap_info.bmiHeader.biPlanes = 1;
    render_state.bitmap_info.bmiHeader.biBitCount = 32;
    render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

    // Create Window Class
    WNDCLASS window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = (LPCTSTR)"Game Window Class";
    window_class.lpfnWndProc = (WNDPROC)window_callback;
    // Register Class
    RegisterClass(&window_class);

    // Create Window
    HWND window = CreateWindowA((LPCSTR)window_class.lpszClassName, (LPCSTR)"WinGameAlpha", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, W_WIDTH, W_HEIGHT, 0, 0, hInstance,0);
    HDC hdc = GetDC(window);

    // Frist time render
    render_init();

    Input input = {};

    while (running){
        MSG message;

        for (int i = 0; i < BUTTON_COUNT; i++){
            input.buttons[i].changed = false;
        }

        while (PeekMessage(&message,window,0,0,PM_REMOVE)) {
            switch(message.message) {
                case WM_KEYUP:
                case WM_KEYDOWN: {
                    uint32_t vk_code = (uint32_t)message.wParam;
                    bool is_down = ((message.lParam & (1<<31)) == 0);

                    switch (vk_code){
                        case VK_W:{
                            input.buttons[BUTTON_UP].down = is_down;
                            input.buttons[BUTTON_UP].changed = true;
                        }break;
                        case VK_A:{
                            input.buttons[BUTTON_LEFT].down = is_down;
                            input.buttons[BUTTON_LEFT].changed = true;
                        }break;
                        case VK_S:{
                            input.buttons[BUTTON_DOWN].down = is_down;
                            input.buttons[BUTTON_DOWN].changed = true;
                        }break;
                        case VK_D:{
                            input.buttons[BUTTON_RIGHT].down = is_down;
                            input.buttons[BUTTON_RIGHT].changed = true;
                        }break;
                    }
                }break;
                default: {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
            }
            changes = true; // Tell tick to react to changes
        }

        // Render every tick
        render_tick(input);

        // Overwrite screen buffer
        StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

        Sleep(TICK_DELAY);
    }

    return 0;
}