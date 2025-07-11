#include "application.h"
#include <windowsx.h>

//初始化Application的静态变量
Application* Application::mInstance = nullptr;
Application* Application::getInstance() {
    //如果mInstance已经实例化了（new出来了），就直接返回
    //否则需要先new出来，再返回
    if (mInstance == nullptr) {
        mInstance = new Application();
    }

    return mInstance;
}

Application::Application() {

}

Application::~Application() {

}

LRESULT CALLBACK Wndproc(
    HWND hWnd,       //窗体句柄
    UINT message,    //消息类型
    WPARAM wParam,   //消息参数
    LPARAM lParam)   //消息参数
{
    Application::getInstance()->handleMessage(hWnd, message, wParam, lParam);
    return DefWindowProc(hWnd, message, wParam, lParam);
}

bool Application::initApplication(HINSTANCE hInstance, const uint32_t& width, const uint32_t& height) {
    mWidth = width;
    mHeight = height;

    //初始化窗口类型，并且注册
    registerWindowClass(hInstance);

    //生成一个窗体，并且显示
    if (!createWindow(hInstance)) {
        return false;
    }

    //初始化画布
    /*
    * Windows GDI(Graphics Device Interface)图形设备接口，负责显示图形
    * DC：Device Context 设备上下文描述对象
    * 每个窗口都有自己对应的设备区域映射，即mhDc
    * 这里创建一个与本窗口兼容的DC，mCanvasDC
    * 可以从mCanvasDC向mhDC拷贝绘图数据内容
    */
    //获取当前窗体HDC
    mhDC = GetDC(mHwnd);
    //创建与当前窗体兼容的HDC2(内存格式/分辨率等)
    mCanvasDC = CreateCompatibleDC(mhDC);

    BITMAPINFO bmpInfo{};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = mWidth;
    bmpInfo.bmiHeader.biHeight = mHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB; //实际上存储方式为bgra

    //创建与mCanvasDC兼容的位图，其实是在mCanvasDC指代的设备上划拨了一块内存，让mCanvasBuffer指向它
    mhBmp = CreateDIBSection(
        mCanvasDC,
        &bmpInfo,
        DIB_RGB_COLORS,
        (void**)&mCanvasBuffer,
        0, 0);

    if (!mhBmp) {
        return false;
    }

    //每个HDC虚拟设备都可以分配出来多个位图/画刷等资源，本操作是将mhBmp作为当前hDC2的操作对象，将来所有对hDC2的拷贝操作都是在拷贝mhBmp的数据
    //一个设备可以创建多个位图，本设备使用mhBmp作为激活位图，对mCanvasDC的内存拷出，其实就是拷出了mhBmp的数据
    SelectObject(mCanvasDC, mhBmp);

    memset(mCanvasBuffer, 0, mWidth * mHeight * 4); //清空buffer为0

    return true;
}

ATOM Application::registerWindowClass(HINSTANCE hInstance) {
    WNDCLASSEXW wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;                       //水平/垂直大小发生变化时重绘窗口
    wndClass.lpfnWndProc = Wndproc;                                 //窗口事件回调函数
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;                                 //应用程序句柄
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);               //应用程序图标,即任务栏的大图标
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);                 //鼠标图标
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);   //窗口背景色
    wndClass.lpszMenuName = NULL;                                   //不需要菜单
    wndClass.lpszClassName = mWindowClassName;                      //窗口类名
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);                 //窗口标题图标

    return RegisterClassExW(&wndClass);
}

BOOL Application::createWindow(HINSTANCE hInstance) {
    mWindowInst = hInstance;

    /*
    * WS_POPUP : 不需要标题栏，则不需要边框
    * WS_OVERLAPPEDWINDOW : 拥有普通程序主窗口的所有特点，必须有标题且有边框
    *
    * WS_CLIPSIBLINGS : 被兄弟窗口挡住区域不绘制
    * WS_CLIPCHILDREN : 被子窗口遮挡住的区域不绘制
    */
    auto dwExStyle = WS_EX_APPWINDOW;
    auto dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    //由于存在标题栏等，所以需要计算中间显示区域的大小,比如PopUp的窗体，就没有标题栏，则不会改变
    RECT windowRect;
    windowRect.left = 0L;
    windowRect.top = 0L;
    windowRect.right = (long)mWidth;
    windowRect.bottom = (long)mHeight;
    AdjustWindowRectEx(
        &windowRect,
        dwStyle,
        FALSE,       //是否使用Menu
        dwExStyle);

    mHwnd = CreateWindowW(
        mWindowClassName,
        (LPCWSTR)"GraphicLearning",   //窗体标题
        dwStyle,
        200,             //x位置，相对左上角
        100,             //y位置，相对左上角
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,         //父窗体
        nullptr,         //菜单栏
        hInstance,       //程序实例
        nullptr);        //额外参数

    if (!mHwnd) {
        return FALSE;
    }

    ShowWindow(mHwnd, true);
    UpdateWindow(mHwnd);

    return TRUE;
}

void Application::handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
    case WM_SIZE: {
        // 获取新窗口大小
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        char buffer[100];
        sprintf(buffer, "Width: %d, Height: %d", width, height);
        std::cout << buffer << std::endl;
        break;
    }
    case WM_KEYDOWN: {
        if (mCamera) {
            mCamera->onKeyDown(wParam);
        }
        break;
    }
    case WM_KEYUP: {
        if (mCamera) {
            mCamera->onKeyUp(wParam);
        }
        break;
    }
    case WM_LBUTTONDOWN: {
        if (mCamera) {
            mCamera->onLMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        break;
    }
    case WM_LBUTTONUP: {
        if (mCamera) {
            mCamera->onLMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        break;
    }
    case WM_MOUSEMOVE: {
        if (mCamera) {
            mCamera->onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        break;
    }
    case WM_CLOSE: {
        DestroyWindow(hWnd);//此处销毁窗体,会自动发出WM DESTROY
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);//发出线程终止请求
        mAlive = false;
        break;
    }
    }
}

bool Application::peekMessage() {
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { //尝试捕获消息
        TranslateMessage(&msg);      //构建消息体
        DispatchMessage(&msg);       //分发消息
    }

    return mAlive;
}

void Application::show() {
    //把mCanvasDC对应内存的数据拷贝到mhDC
    BitBlt(mhDC, 0, 0, mWidth, mHeight, mCanvasDC, 0, 0, SRCCOPY);
}

void Application::setCamera(Camera* camera) {
    mCamera = camera;
}
