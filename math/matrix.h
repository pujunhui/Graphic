#pragma once

#include<iostream>
#include<iomanip>
#include<assert.h>
#include"vector.h"

namespace math {
    template<typename T>
    class Matrix44;

    /*
    * m0 m3 m6
    * m1 m4 m7
    * m2 m5 m8
    */
    template<typename T>
    class Matrix33 {
    public:
        Matrix33() {}
        Matrix33(T v) {
            m[0] = m[4] = m[8] = v;
        }
        Matrix33(const Matrix33<T>& src) {
            memcpy((void*)m, (void*)src.m, sizeof(T) * 9);
        }

        Matrix33(const Matrix44<T>& src) {
            m[0] = src.m[0]; m[3] = src.m[4]; m[6] = src.m[8];
            m[1] = src.m[1]; m[4] = src.m[5]; m[7] = src.m[9];
            m[2] = src.m[2]; m[5] = src.m[6]; m[8] = src.m[10];
        }

        Matrix33<T> operator*(const T& s) {
            Matrix33<T> result;

            auto col0 = this->getColumn(0) * s;
            auto col1 = this->getColumn(1) * s;
            auto col2 = this->getColumn(2) * s;

            result.setColumn(0, col0);
            result.setColumn(1, col1);
            result.setColumn(2, col2);

            return result;
        }

        Vector3<T> operator*(const Vector3<T>& v) {
            return Vector3<T>(
                v.x * m[0] + v.y * m[3], v.z * m[6],
                v.x * m[1] + v.y * m[4], v.z * m[7],
                v.x * m[2] + v.y * m[5], v.z * m[8],
                );
        }

        T get(const uint32_t& row, const uint32_t& col) const {
            assert(row >= 0 && row < 3 && col >= 0 && col < 3);
            return m[col * 3 + row];
        }

        void set(const uint32_t& row, const uint32_t& col, T t) {
            assert(row >= 0 && row < 3 && col >= 0 && col < 3);
            m[col * 3 + row] = t;
        }

        void set(
            const T& m00, const T& m01, const T& m02,
            const T& m10, const T& m11, const T& m12,
            const T& m20, const T& m21, const T& m22
        ) {
            m[0] = m00; m[3] = m01; m[6] = m02;
            m[1] = m10; m[4] = m11; m[7] = m12;
            m[2] = m20; m[5] = m21; m[8] = m22;
        }

        Matrix33<T>& indentity() {
            set(
                1, 0, 0,
                0, 1, 0,
                0, 0, 1
            );
            return *this;
        }

        Vector3<T> getColumn(const uint32_t& col) const {
            assert(col >= 0 && col < 3);
            return Vector3<T>(m[col * 3], m[col * 3 + 1], m[col * 3 + 2]);
        }

        void setColumn(const uint32_t& col, Vector3<T>& cvalue) {
            assert(col >= 0 && col < 3);
            m[col * 3] = cvalue.x;
            m[col * 3 + 1] = cvalue.y;
            m[col * 3 + 2] = cvalue.z;
        }

        friend std::ostream& operator<<(std::ostream& os, const Matrix33& mat) {
            os << "Matrix33 is:" << std::endl;
            for (size_t row = 0; row < 3; row++) {
                for (size_t col = 0; col < 3; col++) {
                    os << std::setw(8) << std::setprecision(2) << std::fixed << mat.get(row, col);
                    if (col == 2) {
                        os << ", ";
                    }
                    else {
                        os << ", ";
                    }
                }
                os << std::endl;
            }
            return os;
        }

    public:
        T m[9] = {
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
        };
    };


    /*
    * m0 m4 m8  m12
    * m1 m5 m9  m13
    * m2 m6 m10 m14
    * m3 m7 m11 m15
    */
    template<typename T>
    class Matrix44 {
    public:
        Matrix44() {}
        Matrix44(T v) {
            m[0] = m[5] = m[10] = m[15] = v;
        }
        Matrix44(const Matrix44<T>& src) {
            memcpy((void*)m, (void*)src.m, sizeof(T) * 16);
        }

        Matrix44<T> operator*(const T& s) {
            Matrix44<T> result;

            auto col0 = this->getColumn(0) * s;
            auto col1 = this->getColumn(1) * s;
            auto col2 = this->getColumn(2) * s;
            auto col3 = this->getColumn(3) * s;

            result.setColumn(0, col0);
            result.setColumn(1, col1);
            result.setColumn(2, col2);
            result.setColumn(3, col3);

            return result;
        }

        Vector4<T> operator*(const Vector4<T>& v) {
            return Vector4<T>(
                v.x * m[0] + v.y * m[4] + v.z * m[8] + v.w * m[12],
                v.x * m[1] + v.y * m[5] + v.z * m[9] + v.w * m[13],
                v.x * m[2] + v.y * m[6] + v.z * m[10] + v.w * m[14],
                v.x * m[3] + v.y * m[7] + v.z * m[11] + v.w * m[15]
                );
        }

        T get(const uint32_t& row, const uint32_t& col) const {
            assert(row >= 0 && row < 4 && col >= 0 && col < 4);
            return m[col * 4 + row];
        }

        void set(const uint32_t& row, const uint32_t& col, T t) {
            assert(row >= 0 && row < 4 && col >= 0 && col < 4);
            m[col * 4 + row] = t;
        }

        void set(
            const T& m00, const T& m01, const T& m02, const T& m03,
            const T& m10, const T& m11, const T& m12, const T& m13,
            const T& m20, const T& m21, const T& m22, const T& m23,
            const T& m30, const T& m31, const T& m32, const T& m33
        ) {
            m[0] = m00; m[4] = m01; m[8] = m02; m[12] = m03;
            m[1] = m10; m[5] = m11; m[9] = m12; m[13] = m13;
            m[2] = m20; m[6] = m21; m[10] = m22; m[14] = m23;
            m[3] = m30; m[7] = m31; m[11] = m32; m[15] = m33;
        }

        Matrix33<T>& indentity() {
            set(
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1,
                );
            return *this;
        }

        Vector4<T> getColumn(const uint32_t& col) const {
            assert(col >= 0 && col < 4);
            return Vector4<T>(
                m[col * 4],
                m[col * 4 + 1],
                m[col * 4 + 2],
                m[col * 4 + 3]
            );
        }

        void setColumn(const uint32_t& col, Vector4<T>& cvalue) {
            assert(col >= 0 && col < 4);
            m[col * 4] = cvalue.x;
            m[col * 4 + 1] = cvalue.y;
            m[col * 4 + 2] = cvalue.z;
            m[col * 4 + 3] = cvalue.w;
        }

        friend std::ostream& operator<<(std::ostream& os, const Matrix44& mat) {
            os << "Matrix44 is:" << std::endl;
            for (size_t row = 0; row < 4; row++) {
                for (size_t col = 0; col < 4; col++) {
                    os << std::setw(8) << std::setprecision(2) << std::fixed << mat.get(row, col);
                    if (col == 3) {
                        os << ", ";
                    }
                    else {
                        os << ", ";
                    }
                }
                os << std::endl;
            }
            return os;
        }

    public:
        T m[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };
    };

    using mat3f = Matrix33<float>;
    using mat4f = Matrix44<float>;
}

