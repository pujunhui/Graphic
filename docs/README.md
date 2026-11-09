# 计算机图形学软光栅知识库导航

本项目通过纯 CPU C++ 实现了一个功能完备的现代软光栅渲染器。为了帮助系统化理解图形学底层原理，所有涉及的理论推导、底层算法、系统机制与工程实现均已按照**分类目录**进行系统拆分与重构。

---

## 知识库目录结构

```
docs/
├── README.md                                  # 本知识库导航与实例映射索引
├── cmake/                                     # CMake 构建系统与工程化实践
│   ├── 01_CMake现代语法与核心机制.md
│   └── 02_软光栅项目构建实战.md
├── 工程与系统/                                # Windows 原生系统接口与 C++ 底层机制
│   ├── 01_Win32窗口与GDI双缓冲机制.md
│   ├── 02_C++运算符重载与返回引用机制.md
│   └── 03_软光栅性能瓶颈与优化技术.md
├── 数学/                                      # 线性代数与三维几何推导
│   ├── 01_向量与列主序矩阵基础.md
│   ├── 02_三维空间变换与MVP矩阵推导.md
│   ├── 03_欧拉角与LookAt视图矩阵推导.md
│   └── 04_法线变换与逆转置矩阵推导.md
├── 算法/                                      # 核心光栅化、采样与多边形算法
│   ├── 01_Bresenham全象限直线光栅化算法.md
│   ├── 02_三角形光栅化与重心坐标插值.md
│   ├── 03_纹理采样与双线性插值算法.md
│   ├── 04_纹理坐标环绕与寻址算法.md
│   └── 05_Sutherland-Hodgman多边形裁剪与透视校正插值.md
└── 渲染管线/                                  # 现代软光栅可编程管线与状态机
    ├── 01_现代软光栅渲染管线架构总览.md
    ├── 02_显存抽象与顶点数组状态机(VAO_VBO_EBO).md
    ├── 03_背面剔除与深度测试(Z-Buffer).md
    ├── 04_Alpha透明度混合与渲染次序.md
    ├── 05_光照模型与Lambert漫反射着色.md
    └── 06_Assimp模型加载与场景层级树.md
```

---

## 19 个实例核心知识点与专题文档映射索引

| 示例编号与名称 | 所属阶段 | 核心涉及的图形学知识点 | 对应专题文档链接 |
| :--- | :--- | :--- | :--- |
| **`01_drawPoint`** | 阶段一 | Win32 窗口生命周期、`PeekMessage` 非阻塞消息泵、GDI `CreateDIBSection` 双缓冲、BGRA 内存字节序与画点寻址 | [01_Win32窗口与GDI双缓冲机制.md](file:///d:/Graphic/Graphic/docs/工程与系统/01_Win32窗口与GDI双缓冲机制.md) |
| **`02_drawLine`** | 阶段一 | 全象限 Bresenham 整数直线光栅化、斜率与象限规范化/逆序还原、主轴比例防除零颜色线性插值 | [01_Bresenham全象限直线光栅化算法.md](file:///d:/Graphic/Graphic/docs/算法/01_Bresenham全象限直线光栅化算法.md) |
| **`03_testMath`** | 阶段一 | 向量模板类设计、点积/叉积/Hadamard 积、列主序矩阵列视图乘法、拉普拉斯展开与伴随矩阵 $4\times 4$ 求逆 | [01_向量与列主序矩阵基础.md](file:///d:/Graphic/Graphic/docs/数学/01_向量与列主序矩阵基础.md)<br>[02_C++运算符重载与返回引用机制.md](file:///d:/Graphic/Graphic/docs/工程与系统/02_C++运算符重载与返回引用机制.md) |
| **`04_drawTriangle`** | 阶段一 | 屏幕空间 AABB 包围盒扫描、二维叉积同号内外测试（含共享边防漏光）、子三角形面积比重心坐标插值 | [02_三角形光栅化与重心坐标插值.md](file:///d:/Graphic/Graphic/docs/算法/02_三角形光栅化与重心坐标插值.md) |
| **`05_drawImage`** | 阶段一 | `stb_image` 图片解码、坐标系 Y 轴翻转、R/B 通道交换、2D 帧缓冲 Alpha 混合 | [01_Win32窗口与GDI双缓冲机制.md](file:///d:/Graphic/Graphic/docs/工程与系统/01_Win32窗口与GDI双缓冲机制.md)<br>[04_Alpha透明度混合与渲染次序.md](file:///d:/Graphic/Graphic/docs/渲染管线/04_Alpha透明度混合与渲染次序.md) |
| **`06_uvTexture`** | 阶段一 | UV 归一化纹理坐标系、重心插值 UV、最近邻（Nearest）采样与马赛克成因 | [03_纹理采样与双线性插值算法.md](file:///d:/Graphic/Graphic/docs/算法/03_纹理采样与双线性插值算法.md) |
| **`07_bilinear`** | 阶段一 | 双线性插值（Bilinear）滤波、四邻域纹素网格定位、除零保护与双轴二次 Lerp 平滑抗锯齿 | [03_纹理采样与双线性插值算法.md](file:///d:/Graphic/Graphic/docs/算法/03_纹理采样与双线性插值算法.md) |
| **`08_textureWrap`** | 阶段一 | 纹理寻址模式、负数安全二次取小数公式、重复平铺（Repeat）与镜像重复（Mirror）算法 | [04_纹理坐标环绕与寻址算法.md](file:///d:/Graphic/Graphic/docs/算法/04_纹理坐标环绕与寻址算法.md) |
| **`09_transform`** | 阶段一 | 仿射模型变换（平移/缩放/Rodrigues 旋转）、相机逆矩阵、透视投影矩阵标准推导、透视除法与视口映射 | [02_三维空间变换与MVP矩阵推导.md](file:///d:/Graphic/Graphic/docs/数学/02_三维空间变换与MVP矩阵推导.md) |
| **`10_vao`** | 阶段二 | 显存缓冲抽象 `BufferObject`（VBO/EBO）、顶点数组状态机 `VertexArrayObject`、分离式顶点布局 | [02_显存抽象与顶点数组状态机(VAO_VBO_EBO).md](file:///d:/Graphic/Graphic/docs/渲染管线/02_显存抽象与顶点数组状态机(VAO_VBO_EBO).md) |
| **`11_renderTriangle`** | 阶段二 | 可编程 `Shader` 基类与属性动态读取、`drawElement` 管线驱动、实心/线框图元装配、单次叉积光栅化复用优化 | [01_现代软光栅渲染管线架构总览.md](file:///d:/Graphic/Graphic/docs/渲染管线/01_现代软光栅渲染管线架构总览.md)<br>[02_三角形光栅化与重心坐标插值.md](file:///d:/Graphic/Graphic/docs/算法/02_三角形光栅化与重心坐标插值.md) |
| **`12_cullFace`** | 阶段二 | 背面剔除（Cull Face）、投影后左手系转换、顺逆时针（CCW/CW）与法线 Z 轴符号组合判据 | [03_背面剔除与深度测试(Z-Buffer).md](file:///d:/Graphic/Graphic/docs/渲染管线/03_背面剔除与深度测试(Z-Buffer).md) |
| **`13_clipSpace`** | 阶段二 | 齐次裁剪空间 7 平面定义、Sutherland-Hodgman 多边形裁剪与交点求取、凸多边形扇形缝补、透视校正插值（$1/w$）数学推导 | [05_Sutherland-Hodgman多边形裁剪与透视校正插值.md](file:///d:/Graphic/Graphic/docs/算法/05_Sutherland-Hodgman多边形裁剪与透视校正插值.md) |
| **`14_depthTest`** | 阶段二 | 交叉式顶点布局（Interleaved VBO Cache Locality 优化）、Z-Buffer 深度缓冲机制与 `DEPTH_LESS` 测试 | [02_显存抽象与顶点数组状态机(VAO_VBO_EBO).md](file:///d:/Graphic/Graphic/docs/渲染管线/02_显存抽象与顶点数组状态机(VAO_VBO_EBO).md)<br>[03_背面剔除与深度测试(Z-Buffer).md](file:///d:/Graphic/Graphic/docs/渲染管线/03_背面剔除与深度测试(Z-Buffer).md) |
| **`15_blending`** | 阶段二 | 3D 管线 Alpha 混合与 Z-Buffer 的机制冲突、先不透明后半透明（由远及近）标准渲染次序 | [04_Alpha透明度混合与渲染次序.md](file:///d:/Graphic/Graphic/docs/渲染管线/04_Alpha透明度混合与渲染次序.md) |
| **`16_texture`** | 阶段二 | GPU 纹理对象 `Texture` 状态机生命周期（`gen/bind/texImage2D/texParameter`）、`TextureShader` 着色流程 | [01_现代软光栅渲染管线架构总览.md](file:///d:/Graphic/Graphic/docs/渲染管线/01_现代软光栅渲染管线架构总览.md)<br>[03_纹理采样与双线性插值算法.md](file:///d:/Graphic/Graphic/docs/算法/03_纹理采样与双线性插值算法.md) |
| **`17_camera`** | 阶段二 | Win32 键鼠事件转发、位掩码并发移动状态机、欧拉角（Pitch/Yaw）球面视线推导、万向节锁死截断、`lookAt` 矩阵推导 | [03_欧拉角与LookAt视图矩阵推导.md](file:///d:/Graphic/Graphic/docs/数学/03_欧拉角与LookAt视图矩阵推导.md)<br>[02_三维空间变换与MVP矩阵推导.md](file:///d:/Graphic/Graphic/docs/数学/02_三维空间变换与MVP矩阵推导.md) |
| **`18_light`** | 阶段二 | 立方体 36 独立硬表面顶点法线设计、法线矩阵 $(M^{-1})^T$ 逆转置推导与 $w=0$ 向量变换、逐片元 Lambert 漫反射与环境光模型 | [04_法线变换与逆转置矩阵推导.md](file:///d:/Graphic/Graphic/docs/数学/04_法线变换与逆转置矩阵推导.md)<br>[05_光照模型与Lambert漫反射着色.md](file:///d:/Graphic/Graphic/docs/渲染管线/05_光照模型与Lambert漫反射着色.md) |
| **`19_model`** | 阶段二 | `app` 全局宏污染防范、Assimp 资产导入后处理、场景层级树（Scene Graph）构建与父子矩阵累乘传递、内嵌纹理（Embedded Texture）内存解码与纹理缓存池 | [06_Assimp模型加载与场景层级树.md](file:///d:/Graphic/Graphic/docs/渲染管线/06_Assimp模型加载与场景层级树.md) |
