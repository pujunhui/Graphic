#pragma once

#include "../global/base.h"

namespace math
{
    template <typename T>
    class Vector3;

    template <typename T>
    class Vector4;

    template <typename T>
    class Vector2
    {
    public:
        Vector2() { x = y = 0; }
        Vector2(T x, T y) : x(x), y(y) {}
        Vector2(const Vector2<T>& v) : x(v.x), y(v.y) {}
        Vector2(const Vector3<T>& v) : x(v.x), y(v.y) {}
        Vector2(const Vector4<T>& v) : x(v.x), y(v.y) {}

        /*
         * 取得向量某个元素
         * int a = v[1]
         */
        T operator[](int i) const
        {
            assert(i >= 0 && i < 2);
            if (i == 0) return x;
            return y;
        }

        /*
         * 给向量某个元素赋值
         * v[1] = 6
         */
        T& operator[](int i)
        {
            assert(i >= 0 && i < 2);
            if (i == 0) return x;
            return y;
        }

        /*
         * 等号运算符重载
         */
        Vector2<T> operator=(const Vector3<T>& v)
        {
            x = v.x;
            y = v.y;
            return *this;
        }

        Vector2<T> operator=(const Vector4<T>& v)
        {
            x = v.x;
            y = v.y;
            return *this;
        }

        /*
         * 加法
         * v = v1 + v2
         */
        Vector2<T> operator+(const Vector2<T>& v) const
        {
            return Vector2(x + v.x, y + v.y);
        }

        /*
         * 加法并赋值
         * v += v1
         */
        Vector2<T> operator+=(const Vector2<T>& v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }

        /*
         * 减法
         * v = v1 - v2
         */
        Vector2<T> operator-(const Vector2<T>& v) const
        {
            return Vector2(x - v.x, y - v.y);
        }

        /*
         * 减法并赋值
         * v -= v1
         */
        Vector2<T> operator-=(const Vector2<T>& v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        /*
         * 乘法
         * v = v * s
         */
        Vector2<T> operator*(T s) const
        {
            return Vector2(x * s, y * s);
        }

        /*
         * 乘法并赋值
         * v *= s
         */
        Vector2<T> operator*=(T s)
        {
            x *= s;
            y *= s;
            return *this;
        }

        /*
         * 除法
         * v = v / f
         */
        Vector2<T> operator/(T f) const
        {
            assert(f != 0);
            float inv = static_cast<T>(1) / f;
            return Vector2(x * inv, y * inv);
        }

        /*
         * 除法并赋值
         * v /= f
         */
        Vector2<T> operator/=(T f)
        {
            assert(f != 0);
            float inv = static_cast<T>(1) / f;
            x *= inv;
            y *= inv;
            return *this;
        }

        /*
         * 负号操作
         * v = -v1
         */
        Vector2<T> operator-()
        {
            return Vector2(-x, -y);
        }

        // 重载<<运算符
        friend std::ostream& operator<<(std::ostream& os, const Vector2& v) {
            os << "Vector2(" << x << ", " << y << ")" << std::endl;
            return os;
        }

    public:
        T x, y;
    };

    template <typename T>
    class Vector3
    {
    public:
        Vector3() { x = y = z = 0; }
        Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
        Vector3(const Vector3<T>& v) : x(v.x), y(v.y), z(v.z) {}
        Vector3(const Vector4<T>& v) : x(v.x), y(v.y), z(v.z) {}

        /*
         * 取得向量某个元素
         * int a = v[1]
         */
        T operator[](int i) const
        {
            assert(i >= 0 && i < 3);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        /*
         * 给向量某个元素赋值
         * v[1] = 6
         */
        T& operator[](int i)
        {
            assert(i >= 0 && i < 3);
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        /*
         * 等号运算符重载
         */
        Vector3<T> operator=(const Vector2<T>& v)
        {
            x = v.x;
            y = v.y;
            return *this;
        }

        Vector3<T> operator=(const Vector4<T>& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }

        /*
         * 加法
         * v = v1 + v2
         */
        Vector3<T> operator+(const Vector3<T>& v) const
        {
            return Vector3(x + v.x, y + v.y, z + v.z);
        }

        /*
         * 加法并赋值
         * v += v1
         */
        Vector3<T> operator+=(const Vector3<T>& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        /*
         * 减法
         * v = v1 - v2
         */
        Vector3<T> operator-(const Vector3<T>& v) const
        {
            return Vector3(x - v.x, y - v.y, z - v.z);
        }

        /*
         * 减法并赋值
         * v -= v1
         */
        Vector3<T> operator-=(const Vector3<T>& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }
        /*
         * 乘法
         * v = v * s
         */
        Vector3<T> operator*(T s) const
        {
            return Vector3(x * s, y * s, z * s);
        }

        /*
         * 乘法并赋值
         * v *= s
         */
        Vector3<T> operator*=(T s)
        {
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        /*
         * 除法
         * v = v / f
         */
        Vector3<T> operator/(T f) const
        {
            assert(f != 0);
            float inv = static_cast<T>(1) / f;
            return Vector3(x * inv, y * inv, z * inv);
        }

        /*
         * 除法并赋值
         * v /= f
         */
        Vector3<T> operator/=(T f)
        {
            assert(f != 0);
            float inv = static_cast<T>(1) / f;
            x *= inv;
            y *= inv;
            z *= inv;
            return *this;
        }

        /*
         * 负号操作
         * v = -v1
         */
        Vector3<T> operator-()
        {
            return Vector3(-x, -y, -z);
        }

        friend std::ostream& operator<<(std::ostream& os, const Vector3& v) {
            os << "Vector3(" << x << ", " << y << ", " << z << ")" << std::endl;
            return os;
        }

    public:
        T x, y, z;
    };

    template <typename T>
    class Vector4
    {
    public:
        Vector4() { x = y = z = w = 0; }
        Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
        Vector4(const Vector4<T>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

        /*
         * 取得向量某个元素
         * int a = v[1]
         */
        T operator[](int i) const
        {
            assert(i >= 0 && i < 4);
            if (i == 0) return x;
            if (i == 1) return y;
            if (i == 2) return z;
            return w;
        }

        /*
         * 给向量某个元素赋值
         * v[1] = 6
         */
        T& operator[](int i)
        {
            assert(i >= 0 && i < 4);
            if (i == 0) return x;
            if (i == 1) return y;
            if (i == 2) return z;
            return w;
        }

        /*
         * 等号运算符重载
         */
        Vector4<T> operator=(const Vector2<T>& v)
        {
            x = v.x;
            y = v.y;
            return *this;
        }

        Vector4<T> operator=(const Vector3<T>& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }

        /*
         * 加法
         * v = v1 + v2
         */
        Vector4<T> operator+(const Vector4<T>& v) const
        {
            return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
        }

        /*
         * 加法并赋值
         * v += v1
         */
        Vector4<T> operator+=(const Vector4<T>& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }

        /*
         * 减法
         * v = v1 - v2
         */
        Vector4<T> operator-(const Vector4<T>& v) const
        {
            return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
        }

        /*
         * 减法并赋值
         * v -= v1
         */
        Vector4<T> operator-=(const Vector4<T>& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }
        /*
         * 乘法
         * v = v * s
         */
        Vector4<T> operator*(T s) const
        {
            return Vector4(x * s, y * s, z * s, w * s);
        }

        /*
         * 乘法并赋值
         * v *= s
         */
        Vector4<T> operator*=(T s)
        {
            x *= s;
            y *= s;
            z *= s;
            w *= s;
            return *this;
        }

        /*
         * 除法
         * v = v / f
         */
        Vector4<T> operator/(T f) const
        {
            assert(f != 0);
            float inv = static_cast<T>(1) / f;
            return Vector4(x * inv, y * inv, z * inv, w * inv);
        }

        /*
         * 除法并赋值
         * v /= f
         */
        Vector4<T> operator/=(T f)
        {
            assert(f != 0);
            float inv = static_cast<T>(1) / f;
            x *= inv;
            y *= inv;
            z *= inv;
            w *= inv;
            return *this;
        }

        /*
         * 负号操作
         * v = -v1
         */
        Vector4<T> operator-()
        {
            return Vector4(-x, -y, -z, -w);
        }

        friend std::ostream& operator<<(std::ostream& os, const Vector4& v) {
            os << "Vector4(" << x << ", " << y << ", " << z << ", " << w << ")" << std::endl;
            return os;
        }

    public:
        T x, y, z, w;
    };

    using vec2f = Vector2<float>;
    using vec2i = Vector2<int>;
    using vec3f = Vector3<float>;
    using vec3i = Vector3<int>;
    using vec4f = Vector4<float>;
    using vec4i = Vector4<int>;
}
