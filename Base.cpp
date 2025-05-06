#include<windows.h>
#include<iostream>
#include <algorithm>

#include "main.h"

using namespace std;

int a =0 ;



BOOL IsAltTabWindow(HWND hwnd) {
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

typedef struct{
    HWND windowTitle;
    HWND button;
}WindowTitleSet;

vector<WindowTitleSet> titles;
vector<HWND> hwnds;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    Window::Init(hInstance, nCmdShow);
    Window window;
    window.onCreate = [&window](){
        EnumWindows(EnumWindowsProc, (LPARAM)&hwnds);
        for (int i = 0; i < hwnds.size(); i++) {
            wstring title(256, L'\0');
            GetWindowTextW(hwnds[i], &title[0], title.size());
            int id = window.GenerateId();
            window.CreateButton(id, title, 100, 50*i+50, 300, 30);
            WindowTitleSet set;
            set.windowTitle = hwnds[i];
            set.button = window.GetElement(id);
            titles.push_back(set);
        }
        Window::onTimer.push_back([ &window](){
            for(int i = 0; i < hwnds.size(); i++){
                while(!IsWindow(hwnds[i])){
                    DestroyWindow(titles[i].button);
                    hwnds.erase(hwnds.begin() + i);
                    titles.erase(titles.begin() + i);
                    for(int j = i; j < titles.size(); j++){
                        RECT rect;
                        GetWindowRect (titles[j].button, &rect);
                        POINT pt = {rect.left, rect.top};  
                        ScreenToClient(window.hwnd, &pt);  
                        SetWindowPos(titles[j].button, HWND_TOP, pt.x, pt.y-50, 0, 0, SWP_NOSIZE);
                    }
                }
                if(IsWindow(hwnds[i])){
                    wstring title(256, L'\0');
                    GetWindowTextW(hwnds[i], &title[0], title.size());
                    wstring title2(256, L'\0');
                    GetWindowTextW(titles[i].button, &title2[0], title2.size());
                    if(title != title2){
                        SetWindowTextW(titles[i].button, title.c_str());
                    }
                }
            }
            int num1 = hwnds.size();
            EnumWindows(EnumWindowsProcCheck, (LPARAM)&hwnds);  
            int num2 = hwnds.size();
            if(num2 > num1){
                for(int i = num1; i<num2; i++){
                    cout << "Asd" << endl;
                    wstring title(256, L'\0');
                    GetWindowTextW(hwnds[i], &title[0], title.size());
                    int id = window.GenerateId();
                    window.CreateButton(id, title, 100, 50*i+50, 300, 30);
                    WindowTitleSet set;
                    set.windowTitle = hwnds[i];
                    set.button = window.GetElement(id);
                    titles.push_back(set);
                }
            }

        });
    };
    window.Show("asdasd", 500, 600, 16);
    return 0;

}


