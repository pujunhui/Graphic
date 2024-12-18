#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:windows /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:console /entry:mainCRTStartup")

float step = 0.1f;
void render() {
    sgl->clear();

    //for (uint32_t i = 0; i < app->getWidth(); i++)
    //{
    //    sgl->drawPoint(i, 100, RGBA(255, 0, 0));
    //}
     
    /*for (uint32_t i = 0; i < app->getWidth(); i++)
    {
        for (uint32_t j = 0; j < app->getHeight(); j++)
        {
            uint32_t v = std::rand() % 255;
            RGBA color(v, v, v, v);
            sgl->drawPoint(i, j, color);
        }
    }*/

    /*Point p1{ 200, 400, RGBA(255, 0 ,0, 255) };
    Point p2{ 200, 200, RGBA(0, 255 ,0, 255) };
    sgl->drawLine(p1, p2);*/

    int r = 200;
    Point c{ 400, 300, RGBA(255, 0 ,0, 255) };

    for (float i = 0; i < 360; i+=10)
    {
        float radian = DEG2RAD(i + step);
        int x = r * sin(radian) + c.x;
        int y = r * cos(radian) + c.y;

        Point pt{ x, y, RGBA(0, 255 ,0, 255) };
        sgl->drawLine(c, pt);
    }
    step += 0.01f;
}

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,            //��Ӧ�ó���ʵ�������Ψһָ����ǰ����
    _In_opt_ HINSTANCE hPrevInstance,    //������ǰһ��ʵ����һ����null
    _In_ LPWSTR lpCmdLine,               //Ӧ�ó������в���
    _In_ int nCmdShow)                   //���������ʾ(��󻯡���С��������)���������
{
    if (!app->initApplication(hInstance, 800, 600)) {
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

int main() {
	std::cout << "Hello World" << std::endl;
    return 0;
}