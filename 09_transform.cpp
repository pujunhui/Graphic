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
Point p1, p2, p3, p4;

math::vec4f pos1, pos2, pos3, pos4;

math::mat4f modelMatrix;
math::mat4f viewMatrix;
math::mat4f perspectiveMatrix;
math::mat4f screenMatrix;

float angle = 0.01f;
float cameraPos = 5.0f;

void transform() {
    angle += 0.01f;
    //cameraPos += 0.01f;

    //模型变换
    //绕模型坐标y轴旋转
    modelMatrix = math::rotate(math::mat4f(1.0f), angle, math::vec3f{ 1.0f, 1.0f, 1.0f });

    modelMatrix = math::scale(modelMatrix, 0.5f, 1.0f, 1.0f);

    //模拟摄像机往后退
    auto cameraMatrix = math::translate(math::mat4f(1.0f), math::vec3f{ 0.0f, 0.0f, cameraPos });
    viewMatrix = math::inverse(cameraMatrix);

    //mvp矩阵相乘
    auto sp1 = perspectiveMatrix * viewMatrix * modelMatrix * pos1;
    auto sp2 = perspectiveMatrix * viewMatrix * modelMatrix * pos2;
    auto sp3 = perspectiveMatrix * viewMatrix * modelMatrix * pos3;
    auto sp4 = perspectiveMatrix * viewMatrix * modelMatrix * pos4;

    //透视除法（此次故意设计z!=0）
    sp1 /= sp1.w;
    sp2 /= sp2.w;
    sp3 /= sp3.w;
    sp4 /= sp4.w;

    sp1.w = 1.0f;
    sp2.w = 1.0f;
    sp3.w = 1.0f;
    sp4.w = 1.0f;

    //屏幕空间
    sp1 = screenMatrix * sp1;
    sp2 = screenMatrix * sp2;
    sp3 = screenMatrix * sp3;
    sp4 = screenMatrix * sp4;

    p1.x = sp1.x;
    p1.y = sp1.y;
    p2.x = sp2.x;
    p2.y = sp2.y;
    p3.x = sp3.x;
    p3.y = sp3.y;
    p4.x = sp4.x;
    p4.y = sp4.y;
}

void render() {
    printFps();

    transform();
     
    sgl->clear();

    sgl->setBilinear(true);
    sgl->setTexture(texture);

    sgl->drawTriangle(p1, p2, p3);
    sgl->drawTriangle(p2, p3, p4);
}

void prepare() {
    texture = Image::createImage("assets/textures/goku.jpg");

    p1.color = RGBA(255, 0, 0, 255);
    p1.uv = math::vec2f(0.0f, 0.0f);

    p2.color = RGBA(0, 255, 0, 255);
    p2.uv = math::vec2f(1.0f, 0.0f);

    p3.color = RGBA(0, 0, 255, 255);
    p3.uv = math::vec2f(0.0f, 1.0f);

    p4.color = RGBA(0, 0, 255, 255);
    p4.uv = math::vec2f(1.0f, 1.0f);

    pos1 = { -2.0f, -1.0f, 0.0f, 1.0f }; //左下
    pos2 = { 2.0f, -1.0f, 0.0f, 1.0f };  //右下
    pos3 = { -2.0f, 1.0f, 0.0f, 1.0f };  //左上
    pos4 = { 2.0f, 1.0f, 0.0f, 1.0f };   //右上

    perspectiveMatrix = math::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    screenMatrix = math::screenMatrix<float>(WIDTH, HEIGHT);
}

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,            //本应用程序实例句柄，唯一指代当前程序
    _In_opt_ HINSTANCE hPrevInstance,    //本程序前一个实例，一般是null
    _In_ LPWSTR lpCmdLine,               //应用程序运行参数
    _In_ int nCmdShow)                   //窗口如何显示(最大化、最小化、隐藏)，不需理会
{
    if (!app->initApplication(hInstance, WIDTH, HEIGHT)) {
        return -1;
    }

    //将bmp指向的内存配置到sgl当中
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
