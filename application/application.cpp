#include "application.h"
#include <windowsx.h>

//初始化Application的静态变量
Application* Application::mInstance = nullptr;
Application* Application::getInstance() {
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
    mWindowInst = hInstance;
    mWidth = width;
    mHeight = height;

    //初始化窗口类型，并且注册
    registerWindowClass(hInstance);

    //生成一个窗口
    mHwnd = createWindow(hInstance);
    if (!mHwnd) {
        return false;
    }

    //显示窗口
    ShowWindow(mHwnd, SW_SHOWNORMAL);
    //立即强制重绘
    UpdateWindow(mHwnd);

    //初始化画布
    /*
    * Windows GDI(Graphics Device Interface)图形设备接口，负责显示图形
    * DC：Device Context 设备上下文描述对象
    * 每个窗口都有自己对应的设备区域映射，即mhDc
    * 这里创建一个与本窗口兼容的DC，mCanvasDC
    * 可以从mCanvasDC向mhDC拷贝绘图数据内容
    */
    //获取当前窗口设备上下文（Device Context）。（屏幕画架）
    mhDC = GetDC(mHwnd);
    //创建与当前窗口兼容的HDC(内存格式/分辨率等)。（另一个画架）
    mCanvasDC = CreateCompatibleDC(mhDC);

    //设置位图信息结构
    BITMAPINFO bmpInfo{};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = mWidth;
    bmpInfo.bmiHeader.biHeight = mHeight;     // 正数表示自底向上，原点在左下角
    bmpInfo.bmiHeader.biPlanes = 1;           // 必须为1
    bmpInfo.bmiHeader.biBitCount = 32;        // 每像素32位（RGBA各8位）
    bmpInfo.bmiHeader.biCompression = BI_RGB; // 未压缩格式

    //从mCanvasDC中创建DIB段（设备无关位图），其实是在mCanvasDC指向的虚拟设备上分配了一块内存，让mCanvasBuffer指向它。（实际画布）
    mhBmp = CreateDIBSection(
        mCanvasDC,
        &bmpInfo,
        DIB_RGB_COLORS,
        (void**)&mCanvasBuffer,  // 获取指向像素数据的指针
        0, 0);

    if (!mhBmp) {
        return false;
    }

    //将GDI对象选入设备上下文，成为当前活动对象。（把画布装到画架上）
    //每个HDC虚拟设备都可以分配出来多个位图/画刷等资源，本操作是将mhBmp作为当前mCanvasDC的操作对象，将来所有对mCanvasDC的操作都是在操作mhBmp的数据
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

HWND Application::createWindow(HINSTANCE hInstance) {
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

    HWND hwnd = CreateWindowW(
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

    return hwnd;
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
    //把mCanvasDC对应内存的数据拷贝到mhDC，由于mCanvasDC的激活位图是mhBmp，也就是将mhBmp的内存数据拷贝到mhDC。（把画布内容复制到屏幕画架）
    BitBlt(mhDC, 0, 0, mWidth, mHeight, mCanvasDC, 0, 0, SRCCOPY);
}

void Application::setCamera(Camera* camera) {
    mCamera = camera;
}
