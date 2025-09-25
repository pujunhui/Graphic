#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu_old/gpu.h"
#include "util/helper.h"
#include "application/image.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:windows /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:console /entry:mainCRTStartup")

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

Image* texture;
Point p1, p2, p3;
Point p4, p5, p6;

void render() {
    printFps();

    sgl->clear();
    sgl->setTexture(texture);

    // �ڽ�����
    sgl->setBilinear(false);
    sgl->drawTriangle(p1, p2, p3);

    // ˫���Բ���
    sgl->setBilinear(true);
    sgl->drawTriangle(p4, p5, p6);
}

void prepare() {
    texture = Image::createImage("assets/textures/goku.jpg");

    p1.x = 0;
    p1.y = 0;
    p1.color = RGBA(255, 0, 0, 255);
    p1.uv = math::vec2f(0.0f, 0.0f);

    p2.x = 200;
    p2.y = 300;
    p2.color = RGBA(0, 255, 0, 255);
    p2.uv = math::vec2f(0.5f, 1.0f);

    p3.x = 400;
    p3.y = 0;
    p3.color = RGBA(0, 0, 255, 255);
    p3.uv = math::vec2f(1.0f, 0.0f);

    p4.x = 400;
    p4.y = 0;
    p4.color = RGBA(255, 0, 0, 255);
    p4.uv = math::vec2f(0.0f, 0.0f);

    p5.x = 600;
    p5.y = 300;
    p5.color = RGBA(0, 255, 0, 255);
    p5.uv = math::vec2f(0.5f, 1.0f);

    p6.x = 800;
    p6.y = 0;
    p6.color = RGBA(0, 0, 255, 255);
    p6.uv = math::vec2f(1.0f, 0.0f);
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

    prepare();

    bool alive = true;
    while (alive) {
        alive = app->peekMessage();
        render();
        app->show();
    }

    return 0;
}
