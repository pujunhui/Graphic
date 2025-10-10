#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"
#include "util/helper.h"
#include "application/image.h"
#include "gpu/shader/textureShader.h"
#include "application/camera.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:windows /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:console /entry:mainCRTStartup")

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

Camera* camera = nullptr;

uint32_t vao = 0;
uint32_t interleavedVbo = 0;

//三角形的indices
uint32_t ebo = 0;

//使用的Shader
TextureShader* shader = nullptr;

//纹理
uint32_t texture = 0;

//纹理图片
Image* image = nullptr;

//mvp变换矩阵
math::mat4f modelMatrix;

float angle = 0.0f;

void transform() {
    //angle += 0.01f;

    //模型变换
    modelMatrix = math::rotate(math::mat4f(1.0f), angle, math::vec3f{ 0.0f, 1.0f, 0.0f });
}

void render() {
    printFps();

    transform();
    shader->mModelMatrix = modelMatrix;
    shader->mViewMatrix = camera->getViewMatrix();
    shader->mProjectionMatrix = camera->getProjectionMatrix();
    shader->mDiffuseTexture = texture;

    sgl->clear();
    sgl->useProgram(shader);

    sgl->bindVertexArray(vao);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->drawElement(DRAW_TRIANGLES, 0, 3);
}

void prepare() {
    camera = new Camera(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, { 0.0f, 1.0f, 0.0f });
    app->setCamera(camera);

    shader = new TextureShader();

    //制造纹理
    image = Image::createImage("assets/textures/goku.jpg");
    texture = sgl->genTexture();
    sgl->bindTexture(texture);
    sgl->texImage2D(image->mWidth, image->mHeight, image->mData);
    sgl->texParameter(TEXTURE_FILTER, TEXTURE_FILTER_NEAREST);
    sgl->texParameter(TEXTURE_WRAP_U, TEXTURE_WRAP_REPEAT);
    sgl->texParameter(TEXTURE_WRAP_V, TEXTURE_WRAP_REPEAT);
    sgl->bindTexture(0);

    sgl->enable(CULL_FACE);
    sgl->frontFace(FRONT_FACE_CW);
    sgl->cullFace(BACK_FACE);

    float vertices[] = {
        //|== position ==|  |======= color =======|  |== uv ==|
        -0.5f, -0.5f, -1.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.0f,  0.5f, -1.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f,
         0.5f, -0.5f, -1.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    };

    uint32_t indices[] = { 0, 1, 2 };

    //生成indices对应的ebo
    ebo = sgl->genBuffer();
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

    //生成vao并且绑定
    vao = sgl->genVertexArray();
    sgl->bindVertexArray(vao);

    //生成vbo，绑定后，设置属性ID及读取参数
    auto interleavedVbo = sgl->genBuffer();
    sgl->bindBuffer(ARRAY_BUFFER, interleavedVbo);
    sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 27, vertices);
    sgl->vertexAttributePointer(0, 3, 9 * sizeof(float), 0 * sizeof(float));
    sgl->vertexAttributePointer(1, 4, 9 * sizeof(float), 3 * sizeof(float));
    sgl->vertexAttributePointer(2, 2, 9 * sizeof(float), 7 * sizeof(float));

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
        camera->update();
        render();
        app->show();
    }

    delete shader;
    Image::destroyImage(image);
    sgl->deleteTexture(texture);
    delete camera;

    return 0;
}
