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

void Window::Show(const char* windowName, int width, int height, int ms){
    std::string className = "WindowClass" + std::to_string(++classCount);
    const char* CLASS_NAME = className.c_str();
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    RegisterClass(&wc);
    
    hwnd = CreateWindowEx(
        0, CLASS_NAME, windowName, WS_OVERLAPPEDWINDOW,
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
        L"BUTTON", text.c_str(), WS_TABSTOP | WS_VISIBLE | WS_CHILD,
        x, y, width, height, hwnd, (HMENU)id, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
    );
    
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

void Window::CreateInput(int id, std::string text, int x, int y, int width, int height){
    HWND input = CreateWindow(
        "EDIT",            
        text.c_str(),                
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
        x, y, width, height,    
        hwnd,
        (HMENU)id, 
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
        NULL
    );
}

void Window::CreateComboBox(int id,int x, int y, int width, int height, std::vector<std::string> items){
    HWND comboBox = CreateWindow(
        "COMBOBOX", 
        NULL,
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, 
        x, y, width, height,    // x, y, width, height
        hwnd, 
        (HMENU)id, 
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL
    );
    for(int i = 0; i < items.size(); i++){
        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)items[i].c_str());
    }
    SendMessage(comboBox, CB_SETCURSEL, 0, 0);
}

void Window::CreateImg(int id, std::string path, int x, int y, int width, int height){
    HWND img = CreateWindow(
        "STATIC", 
        NULL,
        WS_VISIBLE | WS_CHILD | SS_BITMAP,
        x, y, width, height,
        hwnd,
        (HMENU)id,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL
    );
    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
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


void Window::AddLayout(int id, int x, int y, int width, int height){
    HWND hPanel = CreateWindowExA(
        0,                
        "STATIC",        
        NULL,           
        WS_CHILD | WS_VISIBLE | SS_BLACKFRAME,
        x, y, width, height,
        hwnd,
        (HMENU)id,
        hInstance,
        NULL
    );
}
void Window::SetHorizontalLayoutAuto(int id){

}
void Window::SetHorizontalLayout(int id, int offset){

}
void Window::SetVerticalLayoutAuto(int id){
    HWND item = GetElement(id);

}

typedef struct{
    int offset;
    int num;
}LayoutItem;

BOOL CALLBACK EnumChildVerticalLayout(HWND hwnd, LPARAM lParam) {
    LayoutItem* layout = (LayoutItem*)lParam;
    
    return TRUE;
}

void Window::SetVerticalLayout(int id, int offset){
    HWND nid = GetElement(id);
    LayoutItem layout;
    layout.offset = offset;
    layout.num = 0;
    EnumChildWindows(nid, EnumChildVerticalLayout, LPARAM(&layout));
}