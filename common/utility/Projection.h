#pragma once

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/math/constants/constants.hpp>
#include "Vector.hpp"

template<typename ScalarT>
class Projection
{
public:
    typedef ScalarT Scalar;
    typedef boost::numeric::ublas::bounded_matrix<Scalar, 4, 4> Matrix;    
private:
    Matrix m_projectionMatrix;
    Matrix m_invMatrix;
    size_t m_width, m_height;
public:

    Projection(){}

    Projection(const Projection&) = delete;
    Projection& operator=(const Projection&) = delete;
    
    const Matrix& matrix()const{ return m_projectionMatrix; }
    void setPerspective(const Scalar fovInDegree, const size_t width, const size_t height, const Scalar zNear, const Scalar zFar);

    /** \brie unproject the given point into 3d space with depth = 0
    @param point 2D window space, so from (0,0) (top left) to (width,height) (bottom,right)
    @result point in 3D space with depth 0
    */
    float3 unproject(float2 point);
    /** \brief project a given 3d point into window coordinates
    */
    float2 project(float3 point) const;

    /// returns the width limit of the current projection
    size_t width() const { return m_width; }
    /// returns the height limit of the current projection
    size_t height() const { return m_height; }
};


inline Projection<float>& globalProjection()
{
    static Projection<float> p;
    return p;
}

template<typename Scalar>
void Projection<Scalar>::setPerspective(const Scalar fovInDegree, const size_t width, const size_t height, const Scalar zNear, const Scalar zFar)
{
    m_width = width;
    m_height = height;
    m_projectionMatrix.clear();
    float fovInRad = fovInDegree * boost::math::constants::pi<Scalar>()* (1.f / 180.f);
    float tanHalfFovy = tan(fovInRad * 0.5f);

    Scalar scale = Scalar(1) / tan(fovInRad * Scalar(0.5));
    float aspectRatio = static_cast<Scalar>(width) / static_cast<Scalar>(height);
    m_projectionMatrix(0, 0) = 1.f / (aspectRatio * tanHalfFovy);
    m_projectionMatrix(1, 1) = 1.f / (tanHalfFovy);
    m_projectionMatrix(2, 2) = -(zFar + zNear) / (zFar - zNear);
    m_projectionMatrix(3, 2) = -1.f;
    m_projectionMatrix(2, 3) = -(1.f * zFar * zNear) / (zFar - zNear);

    //compute inverse
    Matrix A = m_projectionMatrix;
    boost::numeric::ublas::permutation_matrix<size_t> pm(A.size1());
    m_invMatrix.assign(boost::numeric::ublas::identity_matrix<Scalar>(A.size1()));
    boost::numeric::ublas::lu_factorize(A, pm);
    boost::numeric::ublas::lu_substitute(A, pm, m_invMatrix);
}

template<typename Scalar>
float2 Projection<Scalar>::project(float3 point) const
{
    float4 v;
    v[0] = point[0];
    v[1] = point[1];
    v[2] = point[2];
    v[3] = 1.f;
    v = boost::numeric::ublas::prod(m_projectionMatrix, v);
    float2 r;
    v[0] /= -v[2];
    v[1] /= -v[2];
    r[0] = v[0]*width();
    r[1] = v[1]*height();
    return r;
}


template<typename Scalar>
float3 Projection<Scalar>::unproject(float2 in)
{
    float4 b;
    b[0] = static_cast<Scalar>(in[0]) / static_cast<Scalar>(width());
    b[0] = static_cast<Scalar>(2.*(b[0] - 0.5));
    b[1] = static_cast<Scalar>(in[1]) / static_cast<Scalar>(height());
    b[1] = static_cast<Scalar>(-2.0*(b[1] - 0.5));
    b[2] = static_cast<Scalar>(-1.0);
    b[3] = static_cast<Scalar>(1.0);
    b = boost::numeric::ublas::prod(m_invMatrix, b);

    float3 result;
    result[0] = b[0];
    result[1] = b[1];
    result[2] = b[2];

    result /= b[3];
    result /= static_cast<Scalar>(-result[2] * 0.5);
    return result;
}