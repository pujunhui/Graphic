#include "camera.h"
#include<algorithm>

Camera::Camera(float fovy, float aspect, float n, float f, const math::vec3f& top) {
    mTop = top;
    mProjectionMatrix = math::perspective(fovy, aspect, n, f);
    update();
}

Camera::~Camera() {}

void Camera::onLMouseDown(const int& x, const int& y) {
    mMouseMoving = true;
    mCurrentMouseX = x;
    mCurrentMouseY = y;
}

void Camera::onLMouseUp(const int& x, const int& y) {
    mMouseMoving = false;
}

void Camera::onMouseMove(const int& x, const int& y) {
    if (mMouseMoving) {
        int xOffset = x - mCurrentMouseX;
        int yOffset = y - mCurrentMouseY;

        mCurrentMouseX = x;
        mCurrentMouseY = y;

        pitch(-yOffset);
        yaw(xOffset);
    }
}

void Camera::onKeyDown(const uint32_t& key) {
    switch (key)
    {
    case KEY_W:
        mMoveState |= MOVE_FRONT;
        break;
    case KEY_A:
        mMoveState |= MOVE_LEFT;
        break;
    case KEY_S:
        mMoveState |= MOVE_BACK;
        break;
    case KEY_D:
        mMoveState |= MOVE_RIGHT;
        break;
    default:
        break;
    }
}

void Camera::onKeyUp(const uint32_t& key) {
    switch (key)
    {
    case KEY_W:
        mMoveState &= ~MOVE_FRONT;
        break;
    case KEY_A:
        mMoveState &= ~MOVE_LEFT;
        break;
    case KEY_S:
        mMoveState &= ~MOVE_BACK;
        break;
    case KEY_D:
        mMoveState &= ~MOVE_RIGHT;
        break;
    default:
        break;
    }
}

void Camera::update() {
    //将各个方向的可能进行汇总，得到一个最终的移动方向
    math::vec3f moveDirection = { 0.0f,0.0f,0.0f };

    math::vec3f front = mFront;
    math::vec3f right = math::normalize(cross(mFront, mTop));

    if (mMoveState & MOVE_FRONT) {
        moveDirection += front;
    }

    if (mMoveState & MOVE_BACK) {
        moveDirection -= front;
    }

    if (mMoveState & MOVE_LEFT) {
        moveDirection -= right;
    }

    if (mMoveState & MOVE_RIGHT) {
        moveDirection += right;
    }

    if (math::lengthSquared(moveDirection) != 0) {
        moveDirection = math::normalize(moveDirection);
        mPosition += mSpeed * moveDirection;
    }

    mViewMatrix = math::lookAt<float>(mPosition, mPosition + mFront, mTop);
}


void Camera::setSpeed(const float& speed) {
    mSpeed = speed;
}

void Camera::pitch(int yoffset) {
    mPitch += yoffset * mSensitivity;

    mPitch = std::clamp(mPitch, -89.0f, 89.0f);

    mFront.y = sin(DEG2RAD(mPitch));
    mFront.x = cos(DEG2RAD(mYaw)) * cos(DEG2RAD(mPitch));
    mFront.z = sin(DEG2RAD(mYaw)) * cos(DEG2RAD(mPitch));

    mFront = math::normalize(mFront);
}

void Camera::yaw(int xoffset) {
    mYaw += xoffset * mSensitivity;

    mFront.y = sin(DEG2RAD(mPitch));
    mFront.x = cos(DEG2RAD(mYaw)) * cos(DEG2RAD(mPitch));
    mFront.z = sin(DEG2RAD(mYaw)) * cos(DEG2RAD(mPitch));

    mFront = math::normalize(mFront);
}