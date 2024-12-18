#include "application.h"

//��ʼ��Application�ľ�̬����
Application* Application::mInstance = nullptr;
Application* Application::getInstance() {
	//���mInstance�Ѿ�ʵ�����ˣ�new�����ˣ�����ֱ�ӷ���
	//������Ҫ��new�������ٷ���
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
    HWND hWnd,       //������
    UINT message,    //��Ϣ����
    WPARAM wParam,   //��Ϣ����
    LPARAM lParam)   //��Ϣ����
{
    Application::getInstance()->handleMessage(hWnd, message, wParam, lParam);
    return(DefWindowProc(hWnd, message, wParam, lParam));
}

bool Application::initApplication(HINSTANCE hInstance, const uint32_t& width, const uint32_t& height) {
	mWidth = width;
	mHeight = height;

    //��ʼ���������ͣ�����ע��
    registerWindowClass(hInstance);

    //����һ�����壬������ʾ
    if (!createWindow(hInstance)) {
        return false;
    }

    //��ʼ������
    /*
    * DC��Device Context �豸��������������
    * ÿ�����ڶ����Լ���Ӧ���豸����ӳ�䣬��mhDc
    * ���ﴴ��һ���뱾���ڼ��ݵ�DC��mCanvasDC
    * ���Դ�mCanvasDC��mhDC������ͼ��������
    */
    //��ȡ��ǰ����HDC
    mhDC = GetDC(mHwnd);
    //�����뵱ǰ������ݵ�HDC2(�ڴ��ʽ/�ֱ��ʵ�)
    mCanvasDC = CreateCompatibleDC(mhDC);

    BITMAPINFO bmpInfo{};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = mWidth;
    bmpInfo.bmiHeader.biHeight = mHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB; //ʵ���ϴ洢��ʽΪbgra

    //������mhMem���ݵ�λͼ����ʵ����mhMemָ�����豸�ϻ�����һ���ڴ棬��mCanvasBufferָ����
    HBITMAP bmp = CreateDIBSection(
        mCanvasDC,
        &bmpInfo,
        DIB_RGB_COLORS,
        (void**)&mCanvasBuffer,
        0, 0);

    //ÿ��HDC�����豸�����Է���������λͼ/��ˢ����Դ���������ǽ�bmp��Ϊ��ǰhDC2�Ĳ������󣬽������ж�hDC2�Ŀ������������ٿ���bmp������
    //һ���豸���Դ������λͼ�����豸ʹ��mhBmp��Ϊ����λͼ����mCanvasDc���ڴ濽������ʵ���ǿ�����mhBmp������
    SelectObject(mCanvasDC, bmp);

    memset(mCanvasBuffer, 0, mWidth * mHeight * 4); //���bufferΪ0

	return true;
}

ATOM Application::registerWindowClass(HINSTANCE hInstance) {
    WNDCLASSEXW wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;                       //ˮƽ/��ֱ��С�����仯ʱ�ػ洰��
    wndClass.lpfnWndProc = Wndproc;                                 //�����¼��ص�����
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;                                 //Ӧ�ó�����
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);               //Ӧ�ó���ͼ��,���������Ĵ�ͼ��
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);                 //���ͼ��
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);   //���ڱ���ɫ
    wndClass.lpszMenuName = NULL;                                   //����Ҫ�˵�
    wndClass.lpszClassName = mWindowClassName;                      //��������
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);                 //���ڱ���ͼ��

    return RegisterClassExW(&wndClass);
}

BOOL Application::createWindow(HINSTANCE hInstance) {
    mWindowInst = hInstance;

    /*
    * WS_POPUP : ����Ҫ������������Ҫ�߿�
    * WS_OVERLAPPEDWINDOW : ӵ����ͨ���������ڵ������ص㣬�����б������б߿�
    * 
    * WS_CLIPSIBLINGS : ���ֵܴ��ڵ�ס���򲻻���
    * WS_CLIPCHILDREN : ���Ӵ����ڵ�ס�����򲻻���
    */
    auto dwExStyle = WS_EX_APPWINDOW;
    auto dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
     
    //���ڴ��ڱ������ȣ�������Ҫ�����м���ʾ����Ĵ�С,����PopUp�Ĵ��壬��û�б��������򲻻�ı�
    RECT windowRect;
    windowRect.left = 0L;
    windowRect.top = 0L;
    windowRect.right = (long)mWidth;
    windowRect.bottom = (long)mHeight;
    AdjustWindowRectEx(
        &windowRect,
        dwStyle,
        FALSE,       //�Ƿ�ʹ��Menu
        dwExStyle);

    mHwnd = CreateWindowW(
        mWindowClassName,
        L"GraphicLearning",   //�������
        dwStyle,
        200,             //xλ�ã�������Ͻ�
        100,             //yλ�ã�������Ͻ�
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,         //������
        nullptr,         //�˵���
        hInstance,       //����ʵ��
        nullptr);        //�������

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
    case WM_CLOSE: {
        DestroyWindow(hWnd);//�˴����ٴ���,���Զ�����WM DESTROY
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);//�����߳���ֹ����
        mAlive = false;
        break;
    }
    }
}

bool Application::peekMessage() {
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { //���Բ�����Ϣ
        TranslateMessage(&msg);      //������Ϣ��
        DispatchMessage(&msg);       //�ַ���Ϣ
    }
    
    return mAlive;
}

void Application::show() {
    //��hDC2��Ӧ�ڴ�����ݿ�����hDC
    BitBlt(mhDC, 0, 0, mWidth, mHeight, mCanvasDC, 0, 0, SRCCOPY);
}
