
#include <Windows.h>
#include <iostream>

/* DEFINES */
#define W_WIDTH 640
#define W_HEIGHT 360
#define C_ONMSG "Hello"
#define DEBUG_MODE

bool running = true;
void* buffer_memory;
int buffer_width;
int buffer_height;
BITMAPINFO buffer_bitmap_info;

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
            buffer_width = rect.right - rect.left;
            buffer_height = rect.bottom - rect.top;
            int buffer_size = buffer_width*buffer_height*sizeof(uint32_t);

            #ifdef DEBUG_MODE
            std::cout << "Width: " << buffer_width << ", Height: " << buffer_height << std::endl;
            #endif

            if (buffer_memory) VirtualFree(buffer_memory,0,MEM_RELEASE);
            buffer_memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (buffer_memory == NULL){
                std::cout << "MEMFAIL" << std::endl;
            }

            buffer_bitmap_info.bmiHeader.biSize = sizeof(buffer_bitmap_info.bmiHeader);
            buffer_bitmap_info.bmiHeader.biWidth = buffer_width;
            buffer_bitmap_info.bmiHeader.biHeight = buffer_height;
            buffer_bitmap_info.bmiHeader.biPlanes = 1;
            buffer_bitmap_info.bmiHeader.biBitCount = 32;
            buffer_bitmap_info.bmiHeader.biCompression = BI_RGB;
        } break;

        default: {
            result = DefWindowProc(hWnd,Msg,wParam,lParam);
        }
    }
    return result;
}

int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ){
    std::cout << C_ONMSG << std::endl;
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

    while (running){
        MSG message;
        while (PeekMessage(&message,window,0,0,PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        int y, x;
        uint32_t* pixel = (uint32_t*)buffer_memory;
        for (y = 0; y < buffer_height; y++){
            for (x = 0; x < buffer_width; x++){
                *pixel = 0xff5500;
                pixel++;
            }
        }

        StretchDIBits(hdc, 0, 0, buffer_width, buffer_height, 0, 0, buffer_width, buffer_height, buffer_memory, &buffer_bitmap_info, DIB_RGB_COLORS, SRCCOPY);

    }

    return 0;
}