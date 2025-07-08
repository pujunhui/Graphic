#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu_old/gpu.h"
#include "util/helper.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:windows /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:console /entry:mainCRTStartup")

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

void render() {
    printFps();

    sgl->clear();

    //��������
    Point p1{ 100, 100, RGBA(255, 0, 0) };
    Point p2{ 500, 600, RGBA(0, 255, 0) };
    sgl->drawLine(p1, p2);

    //��������Բ��
    int r = 100;
    Point c{ 400, 300, RGBA(255, 0, 0) };

    for (float i = 0; i < 360; i += 10) {
        float radian = DEG2RAD(i);
        int x = r * sin(radian) + c.x;
        int y = r * cos(radian) + c.y;

        Point pt{ x, y, RGBA(rand() % 256, rand() % 256, rand() % 256) };
        sgl->drawLine(c, pt);
    }
}

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,            //��Ӧ�ó���ʵ�������Ψһָ����ǰ����
    _In_opt_ HINSTANCE hPrevInstance,    //������ǰһ��ʵ����һ����null
    _In_ LPWSTR lpCmdLine,               //Ӧ�ó������в���
    _In_ int nCmdShow)                   //���������ʾ(��󻯡���С��������)���������
{
    if (!app->initApplication(hInstance, WIDTH, HEIGHT)) {
        return -1;
    }

    //��bmpָ����ڴ����õ�sgl����
    sgl->initSurface(app->getWidth(), app->getHeight(), app->getCanvas());

    bool alive = true;
    while (alive) {
        alive = app->peekMessage();
        render();
        app->show();
    }

    return 0;
}
