#include <iostream>

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
