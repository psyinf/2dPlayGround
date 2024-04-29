#pragma once
#include <pgEngine/math/Vec.hpp>

namespace pg {
template <typename T>
struct Matrix
{
    static constexpr uint8_t  rows = 3;
    static constexpr uint8_t  cols = 3;
    std::array<T, rows* cols> data = {0, 0, 0, 0, 1, 0, 0, 0, 1};

    T& operator()(uint8_t row, uint8_t col) { return data[row * cols + col]; }

    const T& operator()(uint8_t row, uint8_t col) const { return data[row * cols + col]; }

    // filled with zeros
    static Matrix<T> makeZero()
    {
        Matrix<T> result;
        result.data.fill(0);
        return result;
    }

    // unity matrix
    static Matrix<T> makeIdentity()
    {
        Matrix<T> result;
        return result;
    }

    // from translation vector
    static Matrix<T> makeTrans(const Vec2<T>& v)
    {
        Matrix<T> result;
        result(0, 2) = v[0];
        result(1, 2) = v[1];
        return result;
    }

    // from scale vector
    static Matrix<T> makeScale(const Vec2<T>& v)
    {
        Matrix<T> result;
        result(0, 0) = v[0];
        result(1, 1) = v[1];
        return result;
    }

    // from rotation angle
    static Matrix<T> makeRot(T angle)
    {
        Matrix<T> result;
        result(0, 0) = std::cos(angle);
        result(0, 1) = -std::sin(angle);
        result(1, 0) = std::sin(angle);
        result(1, 1) = std::cos(angle);
        return result;
    }

    // projection matrix
    static Matrix<T> makeProjection(T left, T right, T bottom, T top, T near, T far)
    {
        Matrix<T> result;
        result(0, 0) = 2 / (right - left);
        result(1, 1) = 2 / (top - bottom);
        result(2, 2) = -2 / (far - near);
        result(0, 3) = -(right + left) / (right - left);
        result(1, 3) = -(top + bottom) / (top - bottom);
        result(2, 3) = -(far + near) / (far - near);
        result(3, 3) = 1;
        return result;
    }

    // matrix multiplication
    Matrix<T> operator*(const Matrix<T>& other) const
    {
        Matrix<T> result;
        for (auto i = 0u; i < rows; ++i)
        {
            for (auto j = 0u; j < cols; ++j)
            {
                for (uint8_t k = 0; k < cols; ++k)
                {
                    result(i, j) += (*this)(i, k) * other(k, j);
                }
            }
        }
        return result;
    }

    // matrix vector multiplication
    Vec2<T> operator*(const Vec2<T>& v) const
    {
        Vec2<T> result;
        for (auto i = 0u; i < rows; ++i)
        {
            for (auto j = 0u; j < cols; ++j)
            {
                result[i] += (*this)(i, j) * v[j];
            }
        }
        return result;
    }

    // transpose matrix

    Matrix<T> transpose() const
    {
        Matrix<T> result;
        for (auto i = 0u; i < rows; ++i)
        {
            for (auto j = 0u; j < cols; ++j)
            {
                result(i, j) = (*this)(j, i);
            }
        }
        return result;
    }

    // matrix determinant
    T determinant() const
    {
        return (*this)(0, 0) * (*this)(1, 1) * (*this)(2, 2) + (*this)(0, 1) * (*this)(1, 2) * (*this)(2, 0) +
               (*this)(0, 2) * (*this)(1, 0) * (*this)(2, 1) - (*this)(0, 2) * (*this)(1, 1) * (*this)(2, 0) -
               (*this)(0, 1) * (*this)(1, 0) * (*this)(2, 2) - (*this)(0, 0) * (*this)(1, 2) * (*this)(2, 1);
    }

    // matrix inverse
    Matrix<T> inverse() const
    {
        Matrix<T> result;
        T         det = determinant();
        if (det == 0) { return result; }
        result(0, 0) = ((*this)(1, 1) * (*this)(2, 2) - (*this)(1, 2) * (*this)(2, 1)) / det;
        result(0, 1) = ((*this)(0, 2) * (*this)(2, 1) - (*this)(0, 1) * (*this)(2, 2)) / det;
        result(0, 2) = ((*this)(0, 1) * (*this)(1, 2) - (*this)(0, 2) * (*this)(1, 1)) / det;
        result(1, 0) = ((*this)(1, 2) * (*this)(2, 0) - (*this)(1, 0) * (*this)(2, 2)) / det;
        result(1, 1) = ((*this)(0, 0) * (*this)(2, 2) - (*this)(0, 2) * (*this)(2, 0)) / det;
        result(1, 2) = ((*this)(0, 2) * (*this)(1, 0) - (*this)(0, 0) * (*this)(1, 2)) / det;
        result(2, 0) = ((*this)(1, 0) * (*this)(2, 1) - (*this)(1, 1) * (*this)(2, 0)) / det;
        result(2, 1) = ((*this)(0, 1) * (*this)(2, 0) - (*this)(0, 0) * (*this)(2, 1)) / det;
        result(2, 2) = ((*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0)) / det;
        return result;
    }

    void rotate(T angle) { *this = rotation(angle) * *this; }

    void scale(const Vec2<T>& v) { *this = scale(v) * *this; }
};

} // namespace pg