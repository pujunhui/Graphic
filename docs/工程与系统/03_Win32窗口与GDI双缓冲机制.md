# Win32 窗口系统与 GDI 双缓冲机制

在不借助 OpenGL/DirectX/Vulkan 等硬件图形 API 的纯 CPU 软光栅渲染器中，必须直接面对操作系统底层的显示接口。本项目基于 Windows 原生 Win32 API 与 GDI（Graphics Device Interface）构建窗口与帧缓冲。

---

## 1. 应用程序入口与子系统选择

### 1.1 链接器入口指令
在示例代码顶部常见以下链接指令：
```cpp
#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup")
```
- **Windows 子系统（Subsystem）**：
  - `/subsystem:windows`：标准 GUI 程序，系统启动时不创建命令行控制台。如果使用 `std::cout` 或 `printf` 输出调试日志，信息将直接丢失。
  - `/subsystem:console`：控制台程序，系统自动分配并弹出一个命令提示符黑框。
- **入口函数（Entry Point）**：
  - `mainCRTStartup` 对应标准 C/C++ 的 `int main()` 入口。
  - `wWinMainCRTStartup` 对应 Windows 宽字符窗口入口 `int APIENTRY wWinMain(...)`。
- **组合意义**：
  通过将子系统设为 `console`，同时将入口重定向至 `wWinMainCRTStartup`，实现了**保留终端窗口实时查看 `std::cout` 调试信息与 FPS 输出，同时能够原生获取 `HINSTANCE` 句柄并调用 Win32 窗口函数**的最佳开发体验。

---

## 2. 窗口创建与尺寸校正

### 2.1 窗口注册与创建
创建窗口分为三个标准步骤：
1. **定义与注册窗口类（`registerWindowClass`）**：
   填充 `WNDCLASSEXW`，指定窗口事件处理回调函数指针 `lpfnWndProc = Wndproc`，设置类名 `mWindowClassName`，调用 `RegisterClassExW` 注册。
2. **窗口样式声明**：

```cpp
   auto dwExStyle = WS_EX_APPWINDOW;
   auto dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
```
   `WS_OVERLAPPEDWINDOW` 包含标准窗口所需的主标题栏、系统菜单、可拖拽边框与最大化/最小化按钮。

### 2.2 客户区（Client Area）精确尺寸换算
若直接在 `CreateWindowW` 中指定传入宽高为 $800 \times 600$ ，系统会将其作为**包含标题栏与四周边框的窗口外部总尺寸**，导致实际可供像素绘制的**客户区**尺寸小于 $800 \times 600$ 。
```cpp
RECT windowRect;
windowRect.left = 0L;
windowRect.top = 0L;
windowRect.right = (long)mWidth;
windowRect.bottom = (long)mHeight;

AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

int actualWindowWidth  = windowRect.right - windowRect.left;
int actualWindowHeight = windowRect.bottom - windowRect.top;
```
调用 `AdjustWindowRectEx` 会根据窗口样式（`dwStyle`），自动在设定的客户区矩形外周追加标题栏厚度与边框像素宽度，从而确保后续创建的窗口内部客户区严格等于设定的 $800 \times 600$ 像素。

---

## 3. 非阻塞主循环与消息泵机制

### 3.1 阻塞 `GetMessage` 与非阻塞 `PeekMessage` 对比
- 传统的 GUI 办公软件通常使用 `GetMessage`：当没有键盘、鼠标等输入消息进入消息队列时，线程进入休眠阻塞状态以节省 CPU 算力。
- 渲染引擎需要**在没有外部输入时也能持续以最高帧率进行场景更新与画面重绘**，因此必须使用 `PeekMessage`：
```cpp
bool Application::peekMessage() {
    MSG msg;
    // 第五个参数 PM_REMOVE 表示若取到消息则将其从队列中弹出
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg); // 转换虚拟按键（如字符输入）
        DispatchMessage(&msg);  // 分发消息至窗口过程回调 Wndproc
    }
    return mAlive; // 窗口未关闭则返回 true 继续主循环
}
```

### 3.2 游戏主循环结构
在各个示例的 `wWinMain` 中，驱动软光栅运行的主循环结构统一为：
```cpp
bool alive = true;
while (alive) {
    alive = app->peekMessage(); // 1. 轮询并分发 Windows 消息
    camera->update();           // 2. 更新相机逻辑与视图矩阵
    render();                   // 3. CPU 软光栅执行渲染管线
    app->show();                // 4. 将离屏内存位图拷贝上屏
}
```

---

## 4. GDI 双缓冲与 DIBSection 显存模拟

如果每次画点都直接通过 GDI 函数（如 `SetPixel`）在屏幕窗口 DC 上绘制，每次函数调用都会产生用户态与内核态的上下文切换，导致绘制一帧需要数秒并产生严重的画面闪烁。本项目采用 **DIBSection 双缓冲机制**：

```
[ CPU 软光栅管线 ]
       ↓ 逐像素直接写入连续内存
[ mCanvasBuffer (DIBSection 像素数组) ] 
       ↓ 选入内存活动设备
[ mCanvasDC (兼容内存 DC) ]
       ↓ BitBlt (单次块传输，毫秒级拷贝)
[ mhDC (窗口屏幕设备上下文) ]
```

### 4.1 设备无关位图（DIB）创建
1. `GetDC(mHwnd)`：获取当前窗口客户区的屏幕设备上下文 `mhDC`。
2. `CreateCompatibleDC(mhDC)`：在内存中创建一个与屏幕 DC 兼容的离屏内存 DC `mCanvasDC`。
3. 配置 `BITMAPINFO`：

```cpp
   BITMAPINFO bmpInfo{};
   bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   bmpInfo.bmiHeader.biWidth = mWidth;
   bmpInfo.bmiHeader.biHeight = mHeight;     // 正数：自底向上（Bottom-Up）
   bmpInfo.bmiHeader.biPlanes = 1;
   bmpInfo.bmiHeader.biBitCount = 32;        // 32 位真彩色（BGRA，每通道 8 位）
   bmpInfo.bmiHeader.biCompression = BI_RGB; // 未压缩 RGB 格式
```
   > **关键知识点**：`biHeight` 为正数时，位图在内存中的存储顺序为**自底向上**——即数组下标 0 对应图像的**左下角**第一行，Y 轴向上生长。这正好与 OpenGL 的标准屏幕空间坐标系一致！
4. `CreateDIBSection(...)`：系统分配一块物理连续的堆内存，并将首地址写入指针 `mCanvasBuffer`。
5. `SelectObject(mCanvasDC, mhBmp)`：将位图绑定到内存 DC，使其成为内存 DC 的绘制目标。

### 4.2 像素内存布局与 BGRA 字节序
Windows GDI 32 位位图在 x86/x64 小端体系结构下，内存存储字节序为 `[Blue, Green, Red, Alpha]`：
```cpp
struct RGBA {
    byte mB; // 低地址
    byte mG;
    byte mR;
    byte mA; // 高地址
};
```
在二维屏幕空间中，已知左下角为原点的坐标 $(x, y)$ ，其在内存中的一维线性偏移量为：

$$
\text{Offset} = (y \times \text{Width} + x) \times 4 \quad \text{Bytes}
$$

通过强转 `RGBA* colorBuffer = (RGBA*)mCanvasBuffer`，画点操作简化为极速的单次指针赋值：
```cpp
mFrameBuffer->mColorBuffer[y * mWidth + x] = color;
```

### 4.3 `BitBlt` 单帧上屏（Present）
在帧末调用 `Application::show()`，使用系统级的内存块传输 API 将整块离屏缓冲一次性翻转至屏幕：
```cpp
void Application::show() {
    BitBlt(mhDC, 0, 0, mWidth, mHeight, mCanvasDC, 0, 0, SRCCOPY);
}
```
`BitBlt` 拥有操作系统显卡驱动底层硬件加速，全屏拷贝仅需几十微秒，彻底消除了画面撕裂与闪烁。

