#pragma once 

#include "../global/base.h"
#include "../math/math.h"

#define NO_MOVE 0
#define MOVE_LEFT 0x001
#define MOVE_RIGHT 0x002
#define MOVE_FRONT 0x004
#define MOVE_BACK 0x008

//1 接收到外部操作(鼠标/键盘)
//2 根据操作更改当前状态(移动状态/front向量)
//3 每一帧由用户主动调用update，更新viewMatrix矩阵
class Camera {
public:
    Camera(float fovy, float aspect, float n, float f, const math::vec3f& top);

    ~Camera();

public:
    void onLMouseDown(const int& x, const int& y);

    void onLMouseUp(const int& x, const int& y);

    void onMouseMove(const int& x, const int& y);

    void onKeyDown(const uint32_t& key);

    void onKeyUp(const uint32_t& key);

    void update();

    math::mat4f getViewMatrix() const {
        return mViewMatrix;
    }

    math::mat4f getProjectionMatrix() const {
        return mProjectionMatrix;
    }

    void setSpeed(const float& speed);

private:
    void pitch(int yoffset);
    void yaw(int xoffset);

private:
    math::mat4f mViewMatrix;
    math::mat4f mProjectionMatrix;

    math::vec3f mPosition{ 0.0f,0.0f,0.0f };    // 当前位置
    math::vec3f mFront{ 0.0f,0.0f,-1.0f };      // 摄像机空间方向
    math::vec3f mTop{ 0.0f,1.0f,0.0f };         // 摄像机穹顶方向
    float       mSpeed{ 0.01f };                // 摄像机移到速度

    float       mPitch{ 0.0f };         // 摄像机当前仰角
    float       mYaw{ -90.0f };         // 摄像机绕y轴旋转角度
    float       mSensitivity{ 0.1f };   // 鼠标滑过屏幕距离对旋转角影响敏感度

    uint32_t    mMoveState{ NO_MOVE };
    bool        mMouseMoving{ false };
    int         mCurrentMouseX{ 0 };
    int         mCurrentMouseY{ 0 };
};
