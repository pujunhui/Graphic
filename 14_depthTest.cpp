#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"
#include "util/helper.h"
#include "application/image.h"
#include "gpu/shader/defaultShader.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:windows /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:console /entry:mainCRTStartup")

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

//三个属性对应的vbo
uint32_t positionVbo = 0;
uint32_t colorVbo = 0;
uint32_t uvVbo = 0;

//三角形的indices
uint32_t ebo = 0;

//2个三角形的vao
uint32_t vao0, vao1 = 0;

//使用的Shader
DefaultShader* shader = nullptr;

//mvp变换矩阵
math::mat4f modelMatrix;
math::mat4f viewMatrix;
math::mat4f perspectiveMatrix;

float angle = 0.01f;

void transform() {
    angle += 0.01f;

    //模型变换
    modelMatrix = math::rotate(math::mat4f(1.0f), angle, math::vec3f{ 0.0f, 1.0f, 0.0f });
}

void render() {
    printFps();

    transform();
    shader->mModelMatrix = modelMatrix;
    shader->mViewMatrix = viewMatrix;
    shader->mProjectionMatrix = perspectiveMatrix;

    sgl->clear();
    sgl->useProgram(shader);
    //sgl->disable(DEPTH_TEST);
    
    //渲染第一个三角形
    sgl->bindVertexArray(vao0);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->drawElement(DRAW_TRIANGLES, 0, 3);

    //渲染第二个三角形
    shader->mModelMatrix = math::mat4f(1.0f);
    sgl->bindVertexArray(vao1);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->drawElement(DRAW_TRIANGLES, 0, 3);
}

void prepare() {
    shader = new DefaultShader();

    perspectiveMatrix = math::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    auto cameraMatrix = math::translate(math::mat4f(1.0f), math::vec3f{ 0.0f, 0.0f, 3.0f });
    viewMatrix = math::inverse(cameraMatrix);

    float vertices0[] = {
        //|== position ==|  |==== color ===|  |== uv ==|
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    };

    float vertices1[] = {
        //|== position ==|  |==== color ===|  |== uv ==|
        -0.5f, -0.5f, 0.1f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.1f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    };

    uint32_t indices[] = { 0, 1, 2 };

    //生成indices对应的ebo
    ebo = sgl->genBuffer();
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

    //生成第一个三角形的vao并且绑定
    vao0 = sgl->genVertexArray();
    sgl->bindVertexArray(vao0);

    //生成第一个三角形的vbo，绑定后，设置属性ID及读取参数
    auto triangle0Vbo = sgl->genBuffer();
    sgl->bindBuffer(ARRAY_BUFFER, triangle0Vbo);
    sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 24, vertices0);
    sgl->vertexAttributePointer(0, 3, 8 * sizeof(float), 0 * sizeof(float));
    sgl->vertexAttributePointer(1, 3, 8 * sizeof(float), 3 * sizeof(float));
    sgl->vertexAttributePointer(2, 2, 8 * sizeof(float), 6 * sizeof(float));

    //生成第二个三角形的vao并且绑定
    vao1 = sgl->genVertexArray();
    sgl->bindVertexArray(vao1);

    //生成第二个三角形的vbo，绑定后，设置属性ID及读取参数
    auto triangle1Vbo = sgl->genBuffer();
    sgl->bindBuffer(ARRAY_BUFFER, triangle1Vbo);
    sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 24, vertices1);
    sgl->vertexAttributePointer(0, 3, 8 * sizeof(float), 0 * sizeof(float));
    sgl->vertexAttributePointer(1, 3, 8 * sizeof(float), 3 * sizeof(float));
    sgl->vertexAttributePointer(2, 2, 8 * sizeof(float), 6 * sizeof(float));

    sgl->bindBuffer(ARRAY_BUFFER, 0);
    sgl->bindVertexArray(0);
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
