#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"
#include "application/image.h"

#include <time.h>

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:windows /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:console /entry:mainCRTStartup")

void printFps() {
    static int frameCount = 0;  // 静态变量，保持帧数
    static clock_t startTime = 0; // 静态变量，保持开始时间
    clock_t currentTime;

    // 初始化开始时间
    if (startTime == 0) {
        startTime = clock();
    }

    // 模拟渲染帧
    frameCount++;

    // 计算时间
    currentTime = clock();
    double elapsedTime = (double)(currentTime - startTime) / CLOCKS_PER_SEC;

    // 每一秒计算一次FPS
    if (elapsedTime >= 1.0) {
        printf("FPS: %d\n", frameCount);
        frameCount = 0; // 重置帧计数
        startTime = clock(); // 重置开始时间
    }
}

void render() {
    sgl->clear();

    printFps();
    //sample1: 绘制水平直线
    /*for (uint32_t i = 0; i < app->getWidth(); i++)
    {
        sgl->drawPoint(i, 100, RGBA(255, 0, 0));
    }*/

    //sample2: 绘制雪花点
    /*for (uint32_t i = 0; i < app->getWidth(); i++)
    {
        for (uint32_t j = 0; j < app->getHeight(); j++)
        {
            uint32_t v = std::rand() % 255;
            RGBA color(v, v, v, v);
            sgl->drawPoint(i, j, color);
        }
    }*/

    //sample3: 绘制渐变斜线
    /*static Point p1{ 200, 200, RGBA(255, 0 ,0, 255) };
    static Point p2{ 201, 400, RGBA(0, 255 ,0, 255) };
    sgl->drawLine(p1, p2);*/


    //sample4: 绘制渐变放射圆
    /*static float step = 0.1f;
    static int r = 200;
    static Point c{ 400, 300, RGBA(255, 0 ,0, 255) };
    for (float i = 0; i < 360; i+=10)
    {
        float radian = DEG2RAD(i + step);
        int x = r * sin(radian) + c.x;
        int y = r * cos(radian) + c.y;

        Point pt{ x, y, RGBA(0, 255 ,0, 255) };
        sgl->drawLine(c, pt);
    }
    step += 0.1f;*/

    //sample5: 绘制三角形
    /*static Point p1{ 100,100, RGBA(255, 0 ,0, 255) };
    static Point p2{ 500,300, RGBA(0, 255 ,0, 255) };
    static Point p3{ 250,500, RGBA(0, 0 ,255, 255) };
    sgl->drawTriangle(p1, p2, p3);*/

    //sample6: 绘制图片
    /*static Image* image = Image::createImage("assets/textures/goku.jpg");
    sgl->drawImage(image);*/

    //sample6: 半透明图片绘制
    /*static Image* image1 = Image::createImage("assets/textures/grass.jpg");
    static Image* image2 = Image::createImage("assets/textures/goku.jpg");
    sgl->setBlending(true);
    sgl->drawImage(image1);
    sgl->drawImageWithAlpha(image2, 100);*/

    //sample7: 绘制uv三角形
    static Point p1{ 100,100, RGBA(), math::vec2f(0.0f,0.0f)};
    static Point p2{ 400,100, RGBA(), math::vec2f(1.0f,0.0f) };
    static Point p3{ 400,400, RGBA(), math::vec2f(1.0f,1.0f) };
    static Point p4{ 100,400, RGBA(), math::vec2f(0.0f,1.0f) };
    static Image* image = Image::createImage("assets/textures/goku.jpg");
    sgl->setTexture(image);
    sgl->setBilinear(false);
    sgl->drawTriangle(p1, p2, p3);
    sgl->drawTriangle(p3, p4, p1);

    static Point p5{ 400,100, RGBA(), math::vec2f(0.0f,0.0f) };
    static Point p6{ 700,100, RGBA(), math::vec2f(1.0f,0.0f) };
    static Point p7{ 700,400, RGBA(), math::vec2f(1.0f,1.0f) };
    static Point p8{ 400,400, RGBA(), math::vec2f(0.0f,1.0f) };
    sgl->setBilinear(true);
    sgl->drawTriangle(p5, p6, p7);
    sgl->drawTriangle(p7, p8, p5);
}

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,            //本应用程序实例句柄，唯一指代当前程序
    _In_opt_ HINSTANCE hPrevInstance,    //本程序前一个实例，一般是null
    _In_ LPWSTR lpCmdLine,               //应用程序运行参数
    _In_ int nCmdShow)                   //窗口如何显示(最大化、最小化、隐藏)，不需理会
{
    if (!app->initApplication(hInstance, 800, 600)) {
        return -1;
    }

    //将bmp指向的内存配置到sgl当中
    sgl->initSurface(app->getWidth(), app->getHeight(), app->getCanvas());

    bool alive = true;
    while (alive) {
        alive = app->peekMessage();
        render();
        app->show();
    }

    return 0;
}

int main() {
    std::cout << "Hello World" << std::endl;
    return 0;
}