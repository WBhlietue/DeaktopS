#include <string>
#include <iostream>
#include <Windows.h>

static BOOL CALLBACK enumWindowCallback(HWND hWnd, LPARAM lparam) {
    int length = GetWindowTextLength(hWnd);
    char* buffer = new char[length + 1];
    GetWindowText(hWnd, buffer, length + 1);
    std::string windowTitle(buffer);
    delete[] buffer;

    // List visible windows with a non-empty title
    if (IsWindowVisible(hWnd) && length != 0) {
        std::cout << hWnd << ":  " << windowTitle << std::endl;
    }
    return TRUE;
}

int main() {
    std::cout << "Enmumerating windows..." << std::endl;
    EnumWindows(enumWindowCallback, 0);
    std::cin.ignore();
    return 0;
}