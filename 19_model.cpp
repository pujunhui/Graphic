#include <Windows.h>
#include <iostream>
#include <time.h>

//这里需要先引入model.h,再引入application.h,避免app宏定义影响assimp库
#include "application/model.h"
#include "application/application.h"
#include "application/camera.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/lightShader.h"

#include "util/helper.h"


#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:windows /entry:wWinMainCRTStartup")
//#pragma comment(linker, "/subsystem:console /entry:mainCRTStartup")

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

Camera* camera = nullptr;

//使用的Shader
LightShader* shader = nullptr;
DirectionalLight directionalLight;
math::vec3f envLight;

//使用的模型
Model* model = nullptr;

float angle = 0.0f;

void transform() {
    angle -= 0.02f;

    auto rotateMatrix = math::rotate(math::mat4f(), angle, { 0.0f,1.0f,0.0f });
    auto translateMatrix = math::translate(math::mat4f(), 0.0f, -1.5f, -5.0f);
    auto m = translateMatrix * rotateMatrix;

    m = math::scale(m, 0.5f, 0.5f, 0.5f);
    model->setModelMatrix(m);
}

void render() {
    printFps();

    transform();

    shader->mViewMatrix = camera->getViewMatrix();
    shader->mProjectionMatrix = camera->getProjectionMatrix();

    shader->mDirectionalLight = directionalLight;
    shader->mEnvLight = envLight;

    sgl->clear();
    sgl->useProgram(shader);
    model->draw(shader);
}

void prepare() {
    camera = new Camera(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, math::vec3f(0.0f, 1.0f, 0.0f));
    app->setCamera(camera);

    shader = new LightShader();
    directionalLight.color = { 0.8f, 0.8f, 1.0f };
    directionalLight.direction = { -1.0f ,-0.5f, -0.7f };
    envLight = { 0.2f, 0.2f, 0.2f };

    sgl->enable(CULL_FACE);
    sgl->frontFace(FRONT_FACE_CCW);
    sgl->cullFace(BACK_FACE);

    model = new Model();
    model->read("assets/model/dinosaur/Rampaging T-Rex.glb");
    //model->read("assets/model/Fist Fight B.fbx");
    //model->read("assets/model/bag/backpack.obj");
    //model->read("assets/model/poCity/scene.gltf");
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
    delete camera;

    return 0;
}

int main() {
    std::cout << "Hello World" << std::endl;
    return 0;
}