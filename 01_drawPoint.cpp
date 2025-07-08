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

    //����ѩ����
    for (uint32_t i = 0; i < app->getWidth(); i++) {
        for (uint32_t j = 0; j < app->getHeight(); j++) {
            uint32_t v = std::rand() % 256;
            RGBA color(v, v, v, v);
            sgl->drawPoint(i, j, color);
        }
    }

    //ͨ��������л���
    for (uint32_t i = 0; i < app->getWidth(); i++) {
        sgl->drawPoint(i, 400, RGBA(255, 0, 0));
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
