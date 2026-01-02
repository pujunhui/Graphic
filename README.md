## 计算机图形学学习项目

通过 CPU 光栅化管线，逐步实现从“画点”到“三维渲染/光照/模型导入”的完整渲染流程。项目基于 Win32 窗口 + 纯 C++ 实现，不依赖 OpenGL/DirectX 的固定管线。

本项目参考教程《计算机图形学—从0开始构建一个OpenGL软光栅》的内容实现，并在此基础上修复与改进部分问题，包括但不限于：
- 直线插值算法中权重的求解
- Texture Wrap中，Mirror的uv计算
- 光照着色器（`LightShader`）中法线矩阵正确使用模型矩阵的逆转置

### 功能概览
- 画点/画线（Bresenham）
- 三角形绘制与重心插值（颜色、UV、法线）
- 纹理采样、UV 包裹与双线性插值
- 变换矩阵（Model/View/Projection）、透视投影与屏幕映射
- VAO/VBO 抽象与基本渲染管线
- 深度测试、背面剔除、混合
- 相机控制（键鼠）
- 基于 Lambert 的简单光照（平行光 + 环境光）
- 模型加载（Assimp），多网格层级绘制

### 目录结构（节选）
- `application/`：应用框架、相机、图片与窗口管理
- `gpu/`：GPU 抽象（raster、shader、texture、vao、bufferObject、frameBuffer）
- `math/`：向量/矩阵/数学函数与常用变换
- `assets/`：纹理与模型资源
- `docs/`：算法与实现文档（Bresenham、重心插值、双线性插值、CMake 等）
- 根目录若干示例可执行：`01_drawPoint` ~ `19_model`

### 依赖
- CMake 3.12+
- Visual Studio 2022（MSVC）或兼容的 C++17 编译器（Windows 平台）
- Assimp（已在 `thirdParty/assimp/` 提供并由 CMake 集成）

### 构建（Windows/MSVC）
推荐方式（无需命令行）：
- 直接用 Visual Studio 2022 打开仓库根目录（VS 会识别为 CMake 项目）
- 在 VS 的“CMake Targets”/“启动项”中选择要运行的目标（如 `19_model`）
- 直接“本地 Windows 调试器”运行即可

### 运行与示例
本项目每个示例对应一个单独的可执行程序：

- `01_drawPoint`：基础画点
- `02_drawLine`：Bresenham 直线
- `03_testMath`：数学与矩阵测试
- `04_drawTriangle`：基础三角形栅格化
- `05_drawImage`：加载与显示图像
- `06_uvTexture`：UV 纹理映射
- `07_bilinear`：双线性插值纹理采样
- `08_textureWrap`：纹理包裹
- `09_transform`：模型变换（平移/旋转/缩放）与投影
- `10_vao`：VAO/VBO 抽象
- `11_renderTriangle`：渲染流程示例
- `12_clipSpace`：裁剪空间
- `13_cullFace`：背面剔除
- `14_depthTest`：深度测试
- `15_blending`：混合
- `16_texture`：纹理综合示例
- `17_camera`：相机与交互
- `18_light`：光照（Lambert）
- `19_model`：模型加载与光照渲染（Assimp）

### 相机与交互
- 鼠标左键拖拽：旋转视角（偏航/俯仰）
- 键盘：`W/A/S/D` 前/左/后/右 平移（详见 `application/camera.h`）
- 调用链：Application 捕获 Win32 消息 → 转发到 `Camera` → 每帧 `camera->update()` 更新视图矩阵

### 许可与致谢
- 学习与研究用途；第三方库版权遵循其各自许可（例如 Assimp）。
