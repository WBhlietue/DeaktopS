#include <windows.h>
#include <iostream>


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    char title[256];
    GetWindowText(hwnd, title, sizeof(title) / sizeof(char));

    if (IsWindowVisible(hwnd) && GetWindowTextLength(hwnd) != 0) {
        std::cout << hwnd << " :  " << title << std::endl;
    }
    
    return TRUE;
}

int main() {
    EnumWindows(EnumWindowsProc, 0);
     return 0;
}
