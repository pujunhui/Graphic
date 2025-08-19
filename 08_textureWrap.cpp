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
Point q1, q2, q3;

float speed = 0.01f;

void changeUV() {
    p1.uv.x += speed;
    p2.uv.x += speed;
    p3.uv.x += speed;
    q1.uv.x += speed;
    q2.uv.x += speed;
    q3.uv.x += speed;
}

void render() {
    printFps();

    changeUV();

    sgl->clear();

    sgl->setBilinear(true);
    sgl->setTexture(texture);
    //sgl->setTextureWrap(TEXTURE_WRAP_MIRROR);

    sgl->drawTriangle(p1, p2, p3);
    sgl->drawTriangle(q1, q2, q3);
}

void prepare() {
    texture = Image::createImage("assets/textures/goku.jpg");

    p1.x = 0;
    p1.y = 0;
    p1.color = RGBA(255, 0, 0, 255);
    p1.uv = math::vec2f(0.0f, 0.0f);

    p2.x = 400;
    p2.y = 0;
    p2.color = RGBA(0, 255, 0, 255);
    p2.uv = math::vec2f(1.0f, 0.0f);

    p3.x = 400;
    p3.y = 300;
    p3.color = RGBA(0, 0, 255, 255);
    p3.uv = math::vec2f(1.0f, 1.0f);

    q1.x = 400;
    q1.y = 300;
    q1.color = RGBA(0, 0, 255, 255);
    q1.uv = math::vec2f(1.0f, 1.0f);

    q2.x = 0;
    q2.y = 300;
    q2.color = RGBA(0, 255, 0, 255);
    q2.uv = math::vec2f(0.0f, 1.0f);

    q3.x = 0;
    q3.y = 0;
    q3.color = RGBA(255, 0, 0, 255);
    q3.uv = math::vec2f(0.0f, 0.0f);
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
