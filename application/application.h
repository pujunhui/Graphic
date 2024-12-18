#pragma once 

#include "../global/base.h"
#include <Windows.h>

#define app Application::getInstance()

class Application {
public:
	~Application();
	
	//���ڷ���ʵ���ľ�̬����
	static Application* getInstance();

    //ע�ᴰ���࣬����һ�����壬��ʾ
	bool initApplication(HINSTANCE hInstance, const uint32_t& width = 800, const uint32_t& height = 600);

    //�й���wndProc�������Ϣ�����ҽ��д���
    void handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    //ÿһ֡/ÿһ��ѭ����������ã������Լ��ַ�������Ϣ
	bool peekMessage();

    void show();

    uint32_t getWidth() const { return mWidth; }
    uint32_t getHeight() const { return mHeight; }
    void* getCanvas() const { return mCanvasBuffer; }

private:
    BOOL createWindow(HINSTANCE hInstance);
    ATOM registerWindowClass(HINSTANCE hInstance);

private:
	//ȫ��Ψһ�ľ�̬����ʵ��
	static Application* mInstance;

    //Ϊtrue��ʾ��ǰ������Ȼ�ڼ�����ʾ������һֱ����
    //Ϊfalse��ʾ�����Ѿ�������رգ�������Ҫ�˳�
    bool mAlive{ true };
    HINSTANCE mWindowInst;
    WCHAR mWindowClassName[100] = L"AppWindow";
    HWND  mHwnd;

	uint32_t mWidth{ 0 };
	uint32_t mHeight{ 0 };

    HDC mhDC;         //��ǰ������dc
    HDC mCanvasDC;    //��������mhDC����ݵĻ�ͼ�õ�dc
    HBITMAP mhBmpl;   //mCanvasDC�ڲ����ɵ�bitmap
    void* mCanvasBuffer{ nullptr }; //mhBmpl��Ӧ���ڴ���ʼλ��ָ��
	Application();
};
