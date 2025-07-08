#include <iostream>

void printFps() {
    static int frameCount = 0;  // ֡��
    static clock_t startTime = 0; // ��ʼʱ��
    clock_t currentTime;

    // ��ʼ����ʼʱ��
    if (startTime == 0) {
        startTime = clock();
    }

    // ģ����Ⱦ֡
    frameCount++;

    // ����ʱ��
    currentTime = clock();
    double elapsedTime = (double)(currentTime - startTime) / CLOCKS_PER_SEC;

    // ÿһ�����һ��FPS
    if (elapsedTime >= 1.0) {
        printf("FPS: %d\n", frameCount);
        frameCount = 0; // ����֡����
        startTime = clock(); // ���ÿ�ʼʱ��
    }
}