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
    static int frameCount = 0;  // 帧数
    static clock_t startTime = 0; // 开始时间
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

//sample1: 绘制水平直线
void drawLine() {
    for (uint32_t i = 0; i < app->getWidth(); i++)
    {
        sgl->drawPoint(i, 100, RGBA(255, 0, 0));
    }
}

//sample2: 绘制雪花点
void drawNoise() {
    for (uint32_t i = 0; i < app->getWidth(); i++)
    {
        for (uint32_t j = 0; j < app->getHeight(); j++)
        {
            uint32_t v = std::rand() % 255;
            RGBA color(v, v, v, v);
            sgl->drawPoint(i, j, color);
        }
    }
}

//sample3: 绘制渐变斜线
void drawGradientLine() {
    static Point p1{ 200, 200, RGBA(255, 0 ,0, 255) };
    static Point p2{ 201, 400, RGBA(0, 255 ,0, 255) };
    sgl->drawLine(p1, p2);
}

//sample4: 绘制渐变放射圆
void drawRadiationLine() {
    static float step = 0.1f;
    static int r = 200;
    static Point c{ 400, 300, RGBA(255, 0 ,0, 255) };
    for (float i = 0; i < 360; i += 10)
    {
        float radian = DEG2RAD(i + step);
        int x = r * sin(radian) + c.x;
        int y = r * cos(radian) + c.y;

        Point pt{ x, y, RGBA(0, 255, 0, 255) };
        sgl->drawLine(c, pt);
    }
    step += 0.1f;
}

//sample5: 绘制彩色三角形
void drawTriangle() {
    static Point p1{ 100,100, RGBA(255, 0 ,0, 255) };
    static Point p2{ 500,300, RGBA(0, 255 ,0, 255) };
    static Point p3{ 250,500, RGBA(0, 0 ,255, 255) };
    sgl->drawTriangle(p1, p2, p3);
}

//sample6: 绘制图片
void drawImage() {
    static Image* image = Image::createImage("assets/textures/goku.jpg");
    sgl->drawImage(image);
}

//sample7: 混合图片绘制
void drawBlendingImage() {
    static Image* image1 = Image::createImage("assets/textures/grass.jpg");
    static Image* image2 = Image::createImage("assets/textures/goku.jpg");
    sgl->setBlending(true);
    sgl->drawImage(image1);
    sgl->drawImageWithAlpha(image2, 128);
}

//sample8: 绘制uv三角形(存在bug，三角形之间有间隙)
void drawTexture() {
    static Point p1{ 100, 100, RGBA(), math::vec2f(0.0f, 0.0f) };
    static Point p2{ 400, 100, RGBA(), math::vec2f(2.0f, 0.0f) };
    static Point p3{ 400, 400, RGBA(), math::vec2f(2.0f, 2.0f) };
    static Point p4{ 100, 400, RGBA(), math::vec2f(0.0f, 2.0f) };
    static Image* image = Image::createImage("assets/textures/goku.jpg");
    sgl->setTexture(image);
    sgl->setTextureWrap(TEXTURE_WRAP_REPEAT);
    sgl->setBilinear(false);
    sgl->drawTriangle(p1, p2, p3);
    sgl->drawTriangle(p3, p4, p1);

    static Point p5{ 400, 100, RGBA(), math::vec2f(0.0f, 0.0f) };
    static Point p6{ 700, 100, RGBA(), math::vec2f(2.0f, 0.0f) };
    static Point p7{ 700, 400, RGBA(), math::vec2f(2.0f, 2.0f) };
    static Point p8{ 400, 400, RGBA(), math::vec2f(0.0f, 2.0f) };
    sgl->setTextureWrap(TEXTURE_WRAP_MIRROR);
    sgl->setBilinear(true);
    sgl->drawTriangle(p5, p6, p7);
    sgl->drawTriangle(p7, p8, p5);
}

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

Point p1;
Point p2;
Point p3;

math::vec4f pos1;
math::vec4f pos2;
math::vec4f pos3;

math::mat4f modelMatrix;
math::mat4f viewMatrix;
math::mat4f perspectiveMatrix;
math::mat4f screenMatrix;

float angle = 0.0f;
float cameraPos = 5.0f;

void transform() {
    angle += 0.01f;
    //cameraPos += 0.01f;

    //模型变换
    modelMatrix = math::rotate(math::mat4f(1.0f), angle, math::vec3f{ 0.0f, 1.0f, 0.0f });

    //模拟摄像头往后退
    auto cameraModelMatrix = math::translate(math::mat4f(1.0f), math::vec3f{ 0.0f ,0.0f ,cameraPos });
    viewMatrix = math::inverse(cameraModelMatrix);

    //mvp矩阵相乘
    auto sp1 = perspectiveMatrix * viewMatrix * modelMatrix * pos1;
    auto sp2 = perspectiveMatrix * viewMatrix * modelMatrix * pos2;
    auto sp3 = perspectiveMatrix * viewMatrix * modelMatrix * pos3;

    //透视除法（此处故意设计z!=0）
    sp1 /= sp1.w;
    sp2 /= sp2.w;
    sp3 /= sp3.w;

    //屏幕空间
    sp1 = screenMatrix * sp1;
    sp2 = screenMatrix * sp2;
    sp3 = screenMatrix * sp3;

    p1.x = sp1.x;
    p1.y = sp1.y;
    p2.x = sp2.x;
    p2.y = sp2.y;
    p3.x = sp3.x;
    p3.y = sp3.y;
}

void prepare() {
    Image* texture = Image::createImage("assets/textures/goku.jpg");
    sgl->setTexture(texture);

    p1.color = RGBA(255, 0, 0, 255);
    p1.uv = math::vec2f(0.0f, 0.0f);

    p2.color = RGBA(0, 255, 0, 255);
    p2.uv = math::vec2f(1.0f, 1.0f);

    p3.color = RGBA(0, 0, 255, 255);
    p3.uv = math::vec2f(1.0f, 0.0f);

    pos1 = { -1.5f, 0.0f, 0.0f, 1.0f };
    pos2 = { 1.5f, 0.0f, 0.0f, 1.0f };
    pos3 = { 0.0f, 2.0f, 0.0f, 1.0f };

    perspectiveMatrix = math::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    screenMatrix = math::screenMatrix<float>(WIDTH, HEIGHT);
}

void render() {
    sgl->clear();

    printFps();

    //drawLine();

    //drawNoise();

    //drawGradientLine();

    //drawRadiationLine();

    //drawTriangle();

    //drawImage();

    //drawBlendingImage();

    //drawTexture();

    transform();
    sgl->drawTriangle(p1, p2, p3);
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

    prepare();

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