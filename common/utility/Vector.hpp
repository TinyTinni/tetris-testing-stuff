#pragma once
#ifndef __COMMON_VECTOR_HPP__
#define __COMMON_VECTOR_HPP__

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#ifdef _DEBUG
#include <boost/numeric/ublas/io.hpp>
#endif

typedef boost::numeric::ublas::bounded_vector<int,2> int2;
typedef boost::numeric::ublas::bounded_vector<int,3> int3;
typedef boost::numeric::ublas::bounded_vector<int,4> int4;

typedef boost::numeric::ublas::bounded_vector<float,2> float2;
typedef boost::numeric::ublas::bounded_vector<float,3> float3;
typedef boost::numeric::ublas::bounded_vector<float,4> float4;

typedef boost::numeric::ublas::bounded_vector<double,2> double2;
typedef boost::numeric::ublas::bounded_vector<double,3> double3;
typedef boost::numeric::ublas::bounded_vector<double,4> double4;

typedef boost::numeric::ublas::bounded_matrix<float, 2, 2> float2x2;
typedef boost::numeric::ublas::bounded_matrix<float, 3, 3> float3x3;
typedef boost::numeric::ublas::bounded_matrix<float, 4, 4> float4x4;

namespace matrix
{

    template<typename Matrix>
    Matrix identity()
    {
        return Matrix(boost::numeric::ublas::identity_matrix<typename Matrix::value_type>(Matrix::max_size1, Matrix::max_size2));
    }

    template<typename Matrix, typename Vector>
    Matrix translate(Matrix matrix, const Vector& vec)
    {
        static_assert(Matrix::max_size2 == Vector::max_size || Matrix::max_size2 == Vector::max_size + 1, "cannot translate");
        matrix(3, 0) += vec(0);
        matrix(3, 1) += vec(1);
        matrix(3, 2) += vec(2);
        return matrix;
    }

    template<typename Matrix, typename T>
    Matrix rotateY(Matrix matrix, const T angleInRad)
    {
        typename Matrix::value_type angle = static_cast<typename Matrix::value_type>(angleInRad);
        matrix(0, 0) = cosf(angle);
        matrix(2, 0) = -sinf(angle);
        matrix(0, 2) = -matrix(2, 0);
        matrix(2, 2) = matrix(0, 0);
        return matrix;
    }

}

#endif //__COMMON_VECTOR_HPP__
