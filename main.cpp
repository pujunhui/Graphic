#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"

#include <time.h>

#include "gpu/shader/defaultShader.h"
#include "gpu/shader/textureShader.h"
#include "application/camera.h"

#include "util/helper.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:windows /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:console /entry:mainCRTStartup")

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

Camera* camera = nullptr;

//两个三角形专属vao
uint32_t vao0 = 0;
uint32_t vao1 = 0;

//三个属性对应的vbo
uint32_t vbo0 = 0;
uint32_t vbo1 = 0;

//三角形的indices
uint32_t ebo;

//使用Shader
TextureShader* shader = nullptr;
uint32_t texture = 0;

//mvp变换矩阵
math::mat4f modelMatrix;

float angle = 0.0f;

void transform() {
    angle += 0.01f;

    //模型变换
    modelMatrix = math::rotate(math::mat4f(1.0f), angle, math::vec3f{ 0.0f, 1.0f, 0.0f });
}

void render() {
    //printFps();

    transform();
    shader->mModelMatrix = modelMatrix;
    shader->mViewMatrix = camera->getViewMatrix();
    shader->mProjectionMatrix = camera->getProjectionMatrix();
    shader->mDiffuseTexture = texture;

    sgl->clear();
    sgl->useProgram(shader);

    sgl->bindVertexArray(vao0);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->drawElement(DRAW_TRIANGLES, 0, 3);

    sgl->bindVertexArray(vao1);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->drawElement(DRAW_TRIANGLES, 0, 3);
}

void prepare() {
    camera = new Camera(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, math::vec3f(0.0f, 1.0f, 0.0f));
    app->setCamera(camera);

    shader = new TextureShader();

    //制造纹理
    texture = sgl->genTexture();
    sgl->bindTexture(texture);

    Image* image = Image::createImage("assets/textures/goku.jpg");
    sgl->texImage2D(image->mWidth, image->mHeight, image->mData);
    Image::destroyImage(image);

    sgl->texParameter(TEXTURE_FILTER, TEXTURE_FILTER_LINEAR);
    sgl->texParameter(TEXTURE_WRAP_U, TEXTURE_WRAP_REPEAT);
    sgl->texParameter(TEXTURE_WRAP_V, TEXTURE_WRAP_REPEAT);
    sgl->bindTexture(0);

    sgl->disable(CULL_FACE);
    sgl->frontFace(FRONT_FACE_CCW);
    sgl->cullFace(BACK_FACE);

    //sgl->disable(DEPTH_TEST);

    sgl->enable(BLENDING);

    //第一个三角形
    float vertices0[] = {
        //  position               color                 uv
        -0.5f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f, 0.5f,  0.0f, 0.0f,
         0.5f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f, 0.5f,  1.0f, 0.0f,
        0.25f, 0.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.5f,  0.5f, 1.0f,
    };

    //第二个三角形
    float vertices1[] = {
        //  position               color                 uv
        0.3f, 0.0f, -0.3f,   1.0f, 1.0f, 0.0f, 0.5f,  0.0f, 0.0f,
        0.8f, 0.0f, -0.3f,   1.0f, 1.0f, 0.0f, 0.5f,  1.0f, 0.0f,
       0.45f, 0.5f, -0.3f,   1.0f, 1.0f, 0.0f, 0.5f,  0.5f, 1.0f,
    };

    uint32_t indices[] = { 0, 1, 2 };

    //生成indices对应的ebo
    ebo = sgl->genBuffer();
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
    sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

    //生成vao并绑定
    vao0 = sgl->genVertexArray();
    sgl->bindVertexArray(vao0);

    //生成每个vbo，绑定后，设置属性ID及读取参数
    vbo0 = sgl->sgl->genBuffer();
    sgl->bindBuffer(ARRAY_BUFFER, vbo0);
    sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 9 * 3, vertices0);

    //position
    sgl->vertexAttributePointer(0, 3, 9 * sizeof(float), 0);
    //color
    sgl->vertexAttributePointer(1, 4, 9 * sizeof(float), 3 * sizeof(float));
    //uv
    sgl->vertexAttributePointer(2, 2, 9 * sizeof(float), 7 * sizeof(float));


    //生成vao并绑定
    vao1 = sgl->genVertexArray();
    sgl->bindVertexArray(vao1);

    //生成每个vbo，绑定后，设置属性ID及读取参数
    vbo1 = sgl->sgl->genBuffer();
    sgl->bindBuffer(ARRAY_BUFFER, vbo1);
    sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 9 * 3, vertices1);

    //position
    sgl->vertexAttributePointer(0, 3, 9 * sizeof(float), 0);
    //color
    sgl->vertexAttributePointer(1, 4, 9 * sizeof(float), 3 * sizeof(float));
    //uv
    sgl->vertexAttributePointer(2, 2, 9 * sizeof(float), 7 * sizeof(float));

    sgl->bindBuffer(ARRAY_BUFFER, 0);
    sgl->bindVertexArray(0);
    //sgl->printVAO(vao);
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
    sgl->deleteTexture(texture);
    delete camera;

    return 0;
}

int main() {
    std::cout << "Hello World" << std::endl;
    return 0;
}