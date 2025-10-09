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

//�������Զ�Ӧ��vbo
uint32_t positionVbo = 0;
uint32_t colorVbo = 0;
uint32_t uvVbo = 0;

//�����ε�indices
uint32_t ebo = 0;

//2�������ε�vao
uint32_t vao0, vao1 = 0;

//ʹ�õ�Shader
DefaultShader* shader = nullptr;

//mvp�任����
math::mat4f modelMatrix;
math::mat4f viewMatrix;
math::mat4f perspectiveMatrix;

float angle = 0.01f;

void transform() {
    angle += 0.01f;

    //ģ�ͱ任
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
    
    //��Ⱦ��һ��������
    sgl->bindVertexArray(vao0);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->drawElement(DRAW_TRIANGLES, 0, 3);

    //��Ⱦ�ڶ���������
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

    //����indices��Ӧ��ebo
    ebo = sgl->genBuffer();
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

    //���ɵ�һ�������ε�vao���Ұ�
    vao0 = sgl->genVertexArray();
    sgl->bindVertexArray(vao0);

    //���ɵ�һ�������ε�vbo���󶨺���������ID����ȡ����
    auto triangle0Vbo = sgl->genBuffer();
    sgl->bindBuffer(ARRAY_BUFFER, triangle0Vbo);
    sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 24, vertices0);
    sgl->vertexAttributePointer(0, 3, 8 * sizeof(float), 0 * sizeof(float));
    sgl->vertexAttributePointer(1, 3, 8 * sizeof(float), 3 * sizeof(float));
    sgl->vertexAttributePointer(2, 2, 8 * sizeof(float), 6 * sizeof(float));

    //���ɵڶ��������ε�vao���Ұ�
    vao1 = sgl->genVertexArray();
    sgl->bindVertexArray(vao1);

    //���ɵڶ��������ε�vbo���󶨺���������ID����ȡ����
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
