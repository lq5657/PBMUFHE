/*
 * =====================================================================================
 *
 *       Filename:  testSerialize.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月24日 21时05分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <cstdlib>
// Last Update:2015-04-24 21:35:02
/**
 * @file testSerialize.cpp
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-24
 */
#include <iostream>
#include <glib.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
#include <NTL/matrix.h>
#include <NTL/mat_ZZ_p.h>
#include "serialize.h"
using namespace NTL;
using namespace std;

int main()
{
    ZZ q;
    q = 1 << 16;
    ZZ_p::init(q);
    Mat<ZZ_p> A;
    long m = 5;
    long n = 6;
    A.SetDims(m, n);
    long i, j;
    cout << "矩阵序列化之前为：" << endl;
    for( i = 0; i < m; i++)
    {
        for(j = 0; j < n; j++)
        {
            A[i][j] = random_ZZ_p();
            cout << A[i][j] << " ";
        }
        cout << endl;
    }

    Vec<ZZ_p> s;
    s.SetLength(n);
    cout << "向量序列化之前为：" << endl;
    for( i = 0; i < n; i++)
    {
        s[i] = random_ZZ_p();
        cout << s[i] << " ";
    }
    cout << endl; 

    //序列化矩阵和向量
    GByteArray* b_vec;
    b_vec = g_byte_array_new();
    vec_ZZ_p_serialize(b_vec, s);
    Vec<ZZ_p> ss;
    ss.SetLength(n);
    ss = unserialize_vec_ZZ_p(b_vec);
    cout << "从序列化字节恢复向量：" << endl;
    for(i = 0; i < n; i++)
    {
        cout << ss[i] << " ";
    }
    cout << endl;

    GByteArray* b_mat;
    b_mat = g_byte_array_new();
    mat_ZZ_p_serialize(b_mat, A);
    Mat<ZZ_p> AA;
    AA.SetDims(m, n);
    AA = unserialize_Mat_ZZ_p(b_mat);
    cout << "从序列化字节恢复矩阵：" << endl;
    for( i = 0; i < m; i++)
    {
        for( j = 0; j < n; j++)
            cout << AA[i][j] << " ";
        cout << endl;
    }

    return 0;
}
