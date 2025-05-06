#include"main.h"

HINSTANCE Window::hInstance = nullptr;
int Window::nCmdShow = 0;
int Window::classCount = 0;
std::vector<std::function<void()>> Window::onTimer = {[]() {}};
std::function<void()> Window::onPaint = [](){};


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Window* pThis = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (uMsg) {
        case WM_TIMER:
            for(int i = 0; i < pThis->onTimer.size(); i++){
                pThis->onTimer[i]();
            }
            break;
        case WM_COMMAND:
            pThis->UIClick(LOWORD(wParam));
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            pThis->onPaint();
            break;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Window::UIClick(int id){
    for(int i = 0; i < uiElements.size(); i++){
        if(uiElements[i].id == id){
            uiElements[i].onClick();
            break;
        }
    }
}

void Window::Show(const char* windowName, int width, int height,int type, int ms ){
    std::string className = "WindowClass" + std::to_string(++classCount);
    const char* CLASS_NAME = className.c_str();
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    RegisterClass(&wc);
    
    hwnd = CreateWindowEx(
        0, CLASS_NAME, windowName, type,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, hInstance, NULL
    );
    
    if (!hwnd) return ;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);  
    ShowWindow(hwnd, nCmdShow);
    onCreate();
    if(ms > 0){
        SetTimer(hwnd, 1, ms, NULL);  
    }
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Window::CreateButton(int id, std::wstring text, int x, int y, int width, int height){
    HWND button = CreateWindowExW(0,
        L"BUTTON", text.c_str(), WS_VISIBLE | WS_CHILD | BS_ICON,
        x, y, width, height, hwnd, (HMENU)id, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
    );
    
}
void Window::CreateButton(int id, HICON icon, int x, int y, int width, int height){
    HWND button = CreateWindowExW(0,
        L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_ICON,
        x, y, width, height, hwnd, (HMENU)id, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
    );
    SendMessage(button, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
    
}
HICON Window::ResizeImage(HICON hIcon, int width, int height){
    HICON resizedIcon = (HICON)CopyImage(
        hIcon,           
        IMAGE_ICON,
        32, 32,         
        LR_COPYFROMRESOURCE
    );
    return resizedIcon;
}

void Window::AddOnClick(int id, std::function<void()> onClick){
    uiElements.push_back({id, onClick});
}
void Window::CreateLabel(int id, std::string text, int x, int y, int width, int height){
    HWND label = CreateWindow(
        "STATIC",          
        text.c_str(),       
        WS_VISIBLE | WS_CHILD, 
        x, y, width, height,     
        hwnd,              
        (HMENU)id,       
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
        NULL
    );
}



void Window::SetText(int id, std::string text){
    HWND item = GetElement(id);
    SetWindowText(item, text.c_str());
}


std::string Window::GetText(int id){
    HWND item = GetElement(id);
    int len = GetWindowTextLength(item);
    char* buffer = new char[len+1];
    GetWindowText(item, buffer, len+1);
    std::string text = buffer;
    delete[] buffer;
    return text;
}

