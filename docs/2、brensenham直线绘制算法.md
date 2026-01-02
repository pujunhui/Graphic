# Bresenham直线绘制算法

## 问题背景

### 光栅化直线绘制的挑战

在计算机图形学中，我们需要在由离散像素点组成的屏幕上绘制连续的几何图形。对于直线绘制，主要面临以下挑战：

1. **离散化问题**：连续的直线必须用离散的像素点来近似表示
2. **效率要求**：需要快速确定哪些像素点最接近理论直线
3. **精度限制**：避免使用浮点运算，减少舍入误差

### 朴素方法的问题

最直观的方法是对每个x坐标计算对应的y值：
```
for x from x0 to x1:
    y = k * x + b
    plot(x, round(y))
```

**问题**：

- 需要浮点乘法和除法运算（效率低）
- round()函数调用开销大
- 浮点精度可能导致累积误差

## Bresenham算法的核心思想

Bresenham算法的天才之处在于：

1. **增量计算**：每步只需要简单的加减法
2. **整数运算**：完全避免浮点数运算
3. **误差跟踪**：通过跟踪累积误差来做出最优选择

**基本策略**：从起点开始，每次沿x方向前进一个像素，通过比较误差来决定y方向是否需要前进。

## 数学推导（以第一象限为例）

我们首先考虑最简单的情况：从 $(x_0, y_0)$ 到 $(x_1, y_1)$ 的直线，其中 $0 \leq \Delta y \leq \Delta x$。

### 第一步：建立直线方程

直线的标准方程为：
$$y = kx + b$$

其中斜率为：
$$k = \frac{\Delta y}{\Delta x}, \quad \Delta x = x_1 - x_0, \quad \Delta y = y_1 - y_0$$

将起点 $(x_0, y_0)$ 代入求截距：
$$b = y_0 - kx_0$$

因此，直线方程为：
$$y = \frac{\Delta y}{\Delta x}(x - x_0) + y_0$$

### 第二步：理解像素选择问题

对于每个整数x坐标 $x_i$，我们需要在 $y_i$ 和 $y_i + 1$ 之间选择一个像素点。

**关键观察**：当前像素为 $(x_i, y_i)$，下一个x位置为 $x_{i+1} = x_i + 1$。

理论直线在 $x_{i+1}$ 处的y坐标为：
$$y_{理论} = y_0 + k(x_{i+1} - x_0) = y_0 + k(x_i + 1 - x_0)$$

### 第三步：建立误差函数

**定义误差**：当前像素点与理论直线的竖直距离
$$\epsilon_i = y_{理论} - y_i$$

为了避免浮点运算，我们定义**放大误差**：
$$E_i = \Delta x \cdot \epsilon_i = \Delta x(y_{理论} - y_i)$$

**为什么要放大？** 因为 $\Delta x$ 是整数，这样可以将所有计算转换为整数运算。

### 第四步：推导误差的递推关系

当$x$从 $x_i$ 增加到 $x_{i+1} = x_i + 1$ 时，理论y值的增量为：
$$\Delta y_{理论} = k \cdot 1 = \frac{\Delta y}{\Delta x}$$

因此放大误差的增量为：
$$\Delta E = \Delta x \cdot \Delta y_{理论} = \Delta x \cdot \frac{\Delta y}{\Delta x} = \Delta y$$

**递推关系**：
$$E_{i+1} = E_i + \Delta y$$

### 第五步：确定像素选择规则

我们需要在候选像素 $(x_{i+1}, y_i)$ 和 $(x_{i+1}, y_i + 1)$ 之间选择。

**选择标准**：选择距离理论直线更近的像素点。

理论直线到两个候选点的距离：

- 到 $(x_{i+1}, y_i)$ 的距离：$|\epsilon_{i+1}|$
- 到 $(x_{i+1}, y_i + 1)$ 的距离：$|\epsilon_{i+1} - 1|$

**临界条件**：当 $\epsilon_{i+1} = 0.5$ 时，两个距离相等。

**选择规则**：

- 当 $\epsilon_{i+1} < 0.5$ 时，选择 $(x_{i+1}, y_i)$
- 当 $\epsilon_{i+1} \geq 0.5$ 时，选择 $(x_{i+1}, y_i + 1)$

转换为放大误差：

- 当 $E_{i+1} < \frac{\Delta x}{2}$ 时，选择 $(x_{i+1}, y_i)$
- 当 $E_{i+1} \geq \frac{\Delta x}{2}$ 时，选择 $(x_{i+1}, y_i + 1)$

### 第六步：设计判别式

为了完全使用整数运算，我们引入**判别式** $d$：

$$d = 2E - \Delta x$$

**为什么这样定义？**

- 当 $E \geq \frac{\Delta x}{2}$ 时，$d \geq 0$
- 当 $E < \frac{\Delta x}{2}$ 时，$d < 0$
- 乘以2是为了避免分数运算

**判别规则转换**：

- 当 $d \geq 0$ 时，选择上方像素：$y_{i+1} = y_i + 1$
- 当 $d < 0$ 时，选择当前行像素：$y_{i+1} = y_i$

### 第七步：推导判别式的递推关系

**情况1：选择当前行像素** $(y_{i+1} = y_i)$

误差更新：$E_{i+1} = E_i + \Delta y$
判别式更新：
$$d_{i+1} = 2E_{i+1} - \Delta x = 2(E_i + \Delta y) - \Delta x = d_i + 2\Delta y$$

**情况2：选择上方像素** $(y_{i+1} = y_i + 1)$

此时实际像素y坐标增加了1，所以实际误差需要修正：
$$E_{实际} = E_i + \Delta y - \Delta x$$

（因为理论y增加了 $\frac{\Delta y}{\Delta x}$，实际y增加了1，差值为 $1 - \frac{\Delta y}{\Delta x} = \frac{\Delta x - \Delta y}{\Delta x}$，放大后为 $\Delta x - \Delta y$）

判别式更新：
$$d_{i+1} = 2E_{实际} - \Delta x = 2(E_i + \Delta y - \Delta x) - \Delta x = d_i + 2(\Delta y - \Delta x)$$

### 第八步：确定初始判别式

当 $i = 0$（即在起点 $(x_0, y_0)$）时：
$$E_0 = \Delta x(y_{理论} - y_0) = \Delta x \cdot 0 = 0$$

向前看一步，到达 $(x_0 + 1, y_0)$ 时：
$$E_1 = E_0 + \Delta y = \Delta y$$

因此：
$$d_0 = 2E_1 - \Delta x = 2\Delta y - \Delta x$$

### 算法总结

**初始化**：

- $x = x_0, y = y_0$
- $d = 2\Delta y - \Delta x$

**迭代过程**：

```
while x < x1:
    plot(x, y)
    if d >= 0:
        y = y + 1
        d = d + 2(Δy - Δx)
    else:
        d = d + 2Δy
    x = x + 1
```

**算法特点**：

1. 只使用整数运算
2. 每步只需要2-3次加法运算
3. 没有乘法、除法或浮点运算

## 算法验证示例

让我们通过一个具体例子来验证算法：

**绘制从 $(0,0)$ 到 $(6,3)$ 的直线**

- $\Delta x = 6, \Delta y = 3$
- 斜率 $k = \frac{3}{6} = 0.5$
- 初始判别式：$d_0 = 2 \times 3 - 6 = 0$

| 步骤 | x | y | d | 选择 | 新d |
|------|---|---|---|------|-----|
| 0 | 0 | 0 | 0 | d≥0, y++ | 0+2(3-6)=-6 |
| 1 | 1 | 1 | -6 | d<0, y不变 | -6+2×3=0 |
| 2 | 2 | 1 | 0 | d≥0, y++ | 0+2(3-6)=-6 |
| 3 | 3 | 2 | -6 | d<0, y不变 | -6+2×3=0 |
| 4 | 4 | 2 | 0 | d≥0, y++ | 0+2(3-6)=-6 |
| 5 | 5 | 3 | -6 | d<0, y不变 | -6+2×3=0 |

**结果像素点**：$(0,0), (1,1), (2,1), (3,2), (4,2), (5,3), (6,3)$

**验证**：理论直线 $y = 0.5x$

- x=1: y=0.5 → 选择(1,1) ✓
- x=2: y=1.0 → 选择(2,1) ✓  
- x=3: y=1.5 → 选择(3,2) ✓
- x=4: y=2.0 → 选择(4,2) ✓
- x=5: y=2.5 → 选择(5,3) ✓

## 算法的几何直观

**误差的几何意义**：

- 误差 $E$ 表示当前像素与理论直线的"竖直距离"×$\Delta x$
- 判别式 $d$ 帮助我们比较这个距离与 $\frac{\Delta x}{2}$（即半个像素宽度）

**为什么算法有效**：

1. **最优性**：总是选择距离理论直线最近的像素
2. **连续性**：生成的像素序列连接成连续的路径
3. **对称性**：从A到B与从B到A生成相同的像素集合（方向相反）

## 算法实现

### 基础版本（第一象限，0 ≤ k ≤ 1）

基于前面的数学推导，我们可以直接实现算法：

```cpp
void bresenhamLineBasic(int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;  // Δx
    int dy = y1 - y0;  // Δy
    
    // 初始化
    int x = x0, y = y0;
    int d = 2 * dy - dx;  // 初始判别式
    
    // 绘制起点
    drawPixel(x, y);
    
    // 迭代绘制
    while (x < x1) {
        if (d >= 0) {
            // 选择上方像素
            y++;
            d += 2 * (dy - dx);
        } else {
            // 选择当前行像素
            d += 2 * dy;
        }
        x++;
        drawPixel(x, y);
    }
}
```

**代码说明**：

- `d >= 0` 对应 $d \geq 0$ 的判别条件
- `d += 2 * (dy - dx)` 对应选择上方像素时的更新：$d_{i+1} = d_i + 2(\Delta y - \Delta x)$
- `d += 2 * dy` 对应选择当前行像素时的更新：$d_{i+1} = d_i + 2\Delta y$

### 扩展到所有象限

前面我们只处理了第一象限且斜率 ≤ 1 的情况。要处理所有象限和所有斜率，需要考虑：

#### 1. 八个方向的处理策略

根据直线的方向，我们可以将所有情况分为8类：

```cpp
class BresenhamLineDrawer {
public:
    virtual void drawPixel(int x, int y) = 0;
    
    void drawLine(int x0, int y0, int x1, int y1) {
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        
        int sx = (x0 < x1) ? 1 : -1;  // x方向步长
        int sy = (y0 < y1) ? 1 : -1;  // y方向步长
        
        if (dx >= dy) {
            // 主要沿x方向移动（斜率 ≤ 1）
            drawLineX(x0, y0, x1, y1, dx, dy, sx, sy);
        } else {
            // 主要沿y方向移动（斜率 > 1）
            drawLineY(x0, y0, x1, y1, dx, dy, sx, sy);
        }
    }
    
private:
    void drawLineX(int x0, int y0, int x1, int y1, 
                   int dx, int dy, int sx, int sy) {
        int d = 2 * dy - dx;  // 判别式
        int x = x0, y = y0;
        
        for (int i = 0; i <= dx; i++) {
            drawPixel(x, y);
            
            if (d >= 0) {
                y += sy;
                d -= 2 * dx;  // 等价于 d += 2*(dy-dx)
            }
            d += 2 * dy;
            x += sx;
        }
    }
    
    void drawLineY(int x0, int y0, int x1, int y1, 
                   int dx, int dy, int sx, int sy) {
        int d = 2 * dx - dy;  // 判别式（角色互换）
        int x = x0, y = y0;
        
        for (int i = 0; i <= dy; i++) {
            drawPixel(x, y);
            
            if (d >= 0) {
                x += sx;
                d -= 2 * dy;  // 等价于 d += 2*(dx-dy)
            }
            d += 2 * dx;
            y += sy;
        }
    }
};
```

#### 2. 统一的实现方法

也可以使用更简洁的统一实现：

```cpp
void drawLineUnified(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    
    int err = dx - dy;  // 综合误差项
    int x = x0, y = y0;
    
    while (true) {
        drawPixel(x, y);
        
        if (x == x1 && y == y1) break;
        
        int e2 = 2 * err;
        
        if (e2 > -dy) {  // x方向移动条件
            err -= dy;
            x += sx;
        }
        
        if (e2 < dx) {   // y方向移动条件
            err += dx;
            y += sy;
        }
    }
}
```

**统一实现的原理**：

- `err = dx - dy` 是一个综合的误差项
- 通过比较 `2*err` 与 `dx` 和 `-dy` 来决定移动方向
- 这个实现等价于在主要方向上应用标准Bresenham算法

### 性能优化版本

对于特殊情况可以进行优化：

```cpp
void drawOptimizedLine(int x0, int y0, int x1, int y1) {
    // 检查特殊情况
    if (x0 == x1) {
        // 垂直线
        int start_y = std::min(y0, y1);
        int end_y = std::max(y0, y1);
        for (int y = start_y; y <= end_y; y++) {
            drawPixel(x0, y);
        }
        return;
    }
    
    if (y0 == y1) {
        // 水平线
        int start_x = std::min(x0, x1);
        int end_x = std::max(x0, x1);
        for (int x = start_x; x <= end_x; x++) {
            drawPixel(x, y0);
        }
        return;
    }
    
    // 一般情况使用Bresenham算法
    drawLineUnified(x0, y0, x1, y1);
}
```

### 深入理解：为什么选择这些更新规则？

让我们回到数学本质来理解更新规则：

1. **基本更新** `d += 2*dy`：
   - 对应x增加1，理论y增加 $\frac{\Delta y}{\Delta x}$
   - 放大误差增加 $\Delta y$
   - 判别式增加 $2\Delta y$

2. **选择上方像素时的额外更新** `d -= 2*dx`：
   - 实际y增加了1，但理论y只增加了 $\frac{\Delta y}{\Delta x}$
   - 需要补偿 $1 - \frac{\Delta y}{\Delta x} = \frac{\Delta x - \Delta y}{\Delta x}$
   - 放大后为 $\Delta x - \Delta y$
   - 判别式需要减去 $2(\Delta x - \Delta y) = 2\Delta x - 2\Delta y$
   - 由于已经加了 $2\Delta y$，所以再减去 $2\Delta x$

## 算法特点

### 优点

1. **高效性**：只使用整数运算，避免浮点数计算
2. **精确性**：生成的直线在视觉上最接近理论直线
3. **通用性**：适用于所有斜率的直线
4. **内存友好**：不需要额外的存储空间

### 缺点

1. **像素化效果**：在低分辨率下可能出现锯齿
2. **不支持抗锯齿**：需要其他算法来实现平滑效果

## 应用场景

1. **计算机图形学**：基础的直线绘制
2. **游戏开发**：2D游戏中的线条绘制
3. **CAD软件**：工程图纸的直线绘制
4. **图像处理**：边缘检测和线段提取

## 扩展算法

### Wu's线算法（抗锯齿）

对于需要抗锯齿效果的应用，可以考虑使用Wu's线算法，它能够生成带有不同透明度的像素，创造出更平滑的视觉效果。

### DDA算法（数字微分分析器）

另一种直线绘制算法，使用浮点数运算，在某些情况下可能更直观，但效率较低。

## 总结

Bresenham算法是计算机图形学中的经典算法，其巧妙之处在于：

1. **数学严谨性**：基于误差分析的科学方法
2. **计算效率**：完全避免浮点运算，只使用整数加减法
3. **实现简洁**：核心逻辑仅需几行代码
4. **广泛适用性**：可以扩展到圆弧、椭圆等其他图形

通过理解其数学推导过程，我们不仅能正确实现算法，还能在此基础上进行优化和扩展。

---

**参考资料**：https://juejin.cn/post/7459005223174815781
