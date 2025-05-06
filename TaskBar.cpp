#include<windows.h>
#include<iostream>
#include <algorithm>

#include "main.h"

using namespace std;

int a =0 ;
int buttonSize = 40;
int buttonOffset = 5;
float num = 0;
HWND thisHwnd;
typedef struct{
    HWND windowTitle;
    HWND button;
}WindowTitleSet;

vector<WindowTitleSet> titles;
vector<HWND> hwnds;
bool isVisible = false;


BOOL IsAltTabWindow(HWND hwnd) {
    if(hwnd == thisHwnd) return FALSE;
    if (!IsWindowVisible(hwnd)) return FALSE;
    if (GetWindow(hwnd, GW_OWNER) != NULL) return FALSE;
    char title[256];
    GetWindowText(hwnd, title, sizeof(title) / sizeof(char));
    if (title[0] == L'\0') return FALSE;
    if (strcmp(title, "Microsoft Text Input Application") == 0)
    return FALSE;
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    if (style & WS_EX_TOOLWINDOW) return FALSE;


    return TRUE;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (IsAltTabWindow(hwnd)) {
        ((vector<HWND>*)lParam)->push_back(hwnd);
    }
    return TRUE;
}
BOOL CALLBACK EnumWindowsProcCheck(HWND hwnd, LPARAM lParam) {
    vector<HWND>* hwndsPtr = (vector<HWND>*)lParam;

    if (IsAltTabWindow(hwnd) && (find(hwndsPtr->begin(), hwndsPtr->end(), hwnd) == hwndsPtr->end())) {
        hwndsPtr->push_back(hwnd);
    }
    return TRUE;
}
HICON GetWindowIcon(HWND hwnd) {
    HICON hIcon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_BIG, 0);
    if (!hIcon) {
        hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICON);
    }
    return hIcon;
}

void RePosAnsSize(Window window, float value){
    // int height = 2*buttonOffset + buttonSize;
    // int width = 2*buttonOffset + buttonSize*titles.size() + buttonOffset*(titles.size()-1);
    // int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    // int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    // SetWindowPos(window.hwnd, HWND_TOPMOST, (screenWidth-width)/2, screenHeight-height+((1-value)*(1-value)*height), width, height,   SWP_NOACTIVATE |SWP_NOZORDER);
    // for(int i = 0; i < titles.size(); i++){
    //     SetWindowPos(titles[i].button, HWND_TOP, buttonOffset+i*(buttonSize+buttonOffset) , buttonOffset, buttonSize, buttonSize, SWP_NOSIZE);
    // }

    int width = 2*buttonOffset + buttonSize;
    int height = 2*buttonOffset + buttonSize*titles.size() + buttonOffset*(titles.size()-1);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    SetWindowPos(window.hwnd, HWND_TOPMOST,((value)*(value)*width)-width, (screenHeight-height)/2, width, height,   SWP_NOACTIVATE |SWP_NOZORDER);
    for(int i = 0; i < titles.size(); i++){
        SetWindowPos(titles[i].button, HWND_TOP,buttonOffset, buttonOffset+i*(buttonSize+buttonOffset), buttonSize, buttonSize, SWP_NOSIZE);
    }

    
}

int AddButton(Window* window, int i){
    int id = window->GenerateId();
    HICON hIcon = GetWindowIcon(hwnds[i]);
    hIcon = window->ResizeImage(hIcon, 32, 32);
    window->CreateButton(id, hIcon, 0, 0, buttonSize, buttonSize);
    window->AddOnClick(id, [window, i](){
        HWND hwnd = hwnds[i];
        HWND foreground = GetForegroundWindow();
        if (foreground == hwnd) {
            ShowWindow(hwnd, SW_MINIMIZE);
        } else {
            if (IsIconic(hwnd)) {
                ShowWindow(hwnd, SW_RESTORE);
            } else {
                ShowWindow(hwnd, SW_SHOW);
            }
            SetForegroundWindow(hwnd);
        }
    
    });
    return id;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    Window::Init(hInstance, nCmdShow);
    Window window;
    window.onCreate = [&window](){
        SetWindowLong(window.hwnd, GWL_EXSTYLE, GetWindowLong(window.hwnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);

        thisHwnd = window.hwnd;
        SetWindowPos(
            window.hwnd,
            HWND_TOPMOST,  
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE  
        );

        EnumWindows(EnumWindowsProc, (LPARAM)&hwnds);
        for (int i = 0; i < hwnds.size(); i++) {
            wstring title(256, L'\0');
            GetWindowTextW(hwnds[i], &title[0], title.size());
            int id = AddButton(&window, i);
            WindowTitleSet set;
            set.windowTitle = hwnds[i];
            set.button = window.GetElement(id);
            titles.push_back(set);
        }
        RePosAnsSize(window, 0);
        Window::onTimer.push_back([ &window](){
            bool isChange = false;
            for(int i = 0; i < hwnds.size(); i++){
                
                while(!IsWindow(hwnds[i])){
                    DestroyWindow(titles[i].button);
                    hwnds.erase(hwnds.begin() + i);
                    titles.erase(titles.begin() + i);
                    i--;
                    isChange = true;
                }
            }
            int num1 = hwnds.size();
            EnumWindows(EnumWindowsProcCheck, (LPARAM)&hwnds);  
            int num2 = hwnds.size();
            if(num2 > num1){
                for(int i = num1; i<num2; i++){
                    wstring title(256, L'\0');
                    GetWindowTextW(hwnds[i], &title[0], title.size());
                    int id = AddButton(&window, i);
                    WindowTitleSet set;
                    set.windowTitle = hwnds[i];
                    set.button = window.GetElement(id);
                    titles.push_back(set);
                    isChange = true;
                }
            }
            POINT pt;
            GetCursorPos(&pt);
            RECT winRect;
            GetWindowRect(window.hwnd, &winRect);
            bool inWindow = PtInRect(&winRect, pt);
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
            int EDGE_THRESHOLD = 5;
            bool atEdge =
            pt.x <= EDGE_THRESHOLD || pt.y <= EDGE_THRESHOLD ||
            pt.x >= screenWidth - EDGE_THRESHOLD || pt.y >= screenHeight - EDGE_THRESHOLD;
            
            if (atEdge || inWindow) {
                if(num >= 1){
                    num =1;
                }else{
                    isChange = true;   
                    num += 0.1;
                }
            }  else {
                if(num <= 0){
                    num =0;
                }else{   
                    isChange = true;
                    num -= 0.1;
                }
            }
            if(isChange){
                RePosAnsSize(window, num);
            }

        });
    };
    window.Show("asdasd", 100, 50, WS_POPUP, 16 );
    return 0;

}


