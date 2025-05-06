#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>
#include <vector>
#include <functional>

typedef struct {
    int id;
    std::function<void()> onClick;
} UIData;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Window {
private:
    int uiCount = 0;
    std::vector<UIData> uiElements;
    static HINSTANCE hInstance;
    static int nCmdShow;
    
public:
    HWND hwnd;
    static int classCount;
    std::function<void()> onCreate;
    static std::vector<std::function<void()>> onTimer;
    static std::function<void()> onPaint;
    static void Init(HINSTANCE ins, int cmd){
        hInstance = ins;
        nCmdShow = cmd;
        classCount = 0;
    }
    void Show(const char* windowName, int width, int height, int ms=-1);
    void CreateButton(int id, std::wstring text, int x, int y, int width, int height);
    void CreateLabel(int id, std::string text, int x, int y, int width, int height);
    void CreateInput(int id, std::string text, int x, int y, int width, int height);
    void CreateComboBox(int id,int x, int y, int width, int height, std::vector<std::string> items);
    void CreateImg(int id, std::string path, int x, int y, int width, int height);
    void AddOnClick(int id, std::function<void()> onClick);
    void AddLayout(int id, int x, int y, int width, int height);
    void SetHorizontalLayoutAuto(int id);
    void SetHorizontalLayout(int id, int offset);
    void SetVerticalLayoutAuto(int id);
    void SetVerticalLayout(int id, int offset);
    int GenerateId(){
        return ++uiCount;
    }

    HWND GetElement(int id){
        return GetDlgItem(hwnd, id);
    }
    void SetText(int id, std::string text);
    std::string GetText(int id);
    void UIClick(int id);
};



#endif 
