/*
 * =====================================================================================
 *
 *       Filename:  serialize.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月24日 15时45分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <cstdlib>
//#include <fstream>
#include <NTL/ZZ_p.h>
#include <NTL/matrix.h>
#include <NTL/mat_ZZ_p.h>
#include <glib.h>

#include "common.h"
#include "serialize.h"
//using namespace std;
using namespace NTL;

void vec_ZZ_p_serialize( GByteArray* b, Vec<ZZ_p> oldVec )
{
    //long l = std::floor(log2(to_long(ZZ_p::modulus()))) + 1;
    long k = oldVec.length();
    guint8 byte;
    int i,j;
    for( j = 3; j >= 0; j--)  //将Vec的长度存在字节序列的前4字节
    {
        byte = (k & 0xff << (j*8)) >> (j*8);
        g_byte_array_append(b, &byte, 1);
    }
    for(i = 0; i < k; i++)
    {
        long vec_elem = conv<long>(oldVec[i]);
        for( j = 3; j >= 0; j--)
        {
            byte = (vec_elem & 0xff << (j*8)) >> (j*8);
            g_byte_array_append(b, &byte, 1);
        }
    }
}

Vec<ZZ_p> unserialize_vec_ZZ_p( GByteArray* b )
{
    Vec<ZZ_p> newVec;
    int i,j;
    guint offset = 0;
    guint b_length = b->len;
    long vec_len = 0;
    for( i = 3; i >=0; i--)
    {
        vec_len |= (b->data[offset++])<<(i*8);
    }
    newVec.SetLength(vec_len);
    long elem_Vec;
    for( j = 0; offset < b_length; )
    {
        elem_Vec = 0;
        for( i = 3; i >= 0; i--)
        {
            elem_Vec |= (b->data[offset++])<<(i*8);
        }
        newVec[j++] = conv<ZZ_p>(elem_Vec);
    }
    return newVec;
}

Vec<ZZ_p> unserialize_vec_ZZ_p( GByteArray* b, guint* offset )
{
    Vec<ZZ_p> newVec;
    int i,j;
    //guint curOffset;
    //guint b_length = b->len;
    long vec_len = 0;
    for( i = 3; i >=0; i--)
    {
        vec_len |= (b->data[(*offset)++])<<(i*8);
    }
    newVec.SetLength(vec_len);
    long elem_vec;
    for( j = 0; /**offset < b_length*/j < vec_len; j++ )    //调试说这里有错
    {
        elem_vec = 0;
        for( i = 3; i >= 0; i--)
        {
            elem_vec |= (b->data[(*offset)++])<<(i*8);
        }
        newVec[j] = conv<ZZ_p>(elem_vec);
    }
    return newVec;
}


void mat_ZZ_p_serialize( GByteArray* b,Mat<ZZ_p> oldMat )
{
    long m = oldMat.NumRows();
    long n = oldMat.NumCols();
    long i,j;
    guint8 byte;
    for( j = 3; j >= 0; j--)
    {
        byte = (m & 0xff << (j*8)) >> (j*8);
        g_byte_array_append(b, &byte, 1);
    }
    for( j = 3; j >= 0; j--) 
    {
        byte = (n & 0xff << (j*8)) >> (j*8);
        g_byte_array_append(b, &byte, 1);
    }
    for(i = 0; i < m; i++)
    {
        vec_ZZ_p_serialize(b, oldMat[i]);  
    }
}

Mat<ZZ_p> unserialize_mat_ZZ_p( GByteArray* b )
{
    Mat<ZZ_p> newMat;
    long m = 0;
    long n = 0;
    long i,j;
    guint offset = 0;
    for( i = 3; i >= 0; i-- )
    {
        m |= (b->data[offset++])<<(i*8);
    }
    for( i = 3; i >=0; i-- )
    {
        n |= (b->data[offset++])<<(i*8);
    }
    newMat.SetDims(m, n);
    guint length = b->len;
    for( j = 0; j < m; j++)
    {
        Vec<ZZ_p> rowVec;
        rowVec.SetLength(n);
        //offset += 4 * (n+1) * j;
        if(offset < length)
            rowVec = unserialize_vec_ZZ_p(b, &offset);
        newMat[j] = rowVec;
    }
    return newMat; 
}

void read_cph_file( char* file, GByteArray** policy_buf, GByteArray** mat_buf )
{
    FILE* f;
    int i;
    long buf_len;
    long policy_buf_len;
    long mat_buf_len;

    *policy_buf = g_byte_array_new();
    *mat_buf = g_byte_array_new();

    f = fopen_read_or_die(file);
    buf_len = 0;
    for( i = 3; i >= 0; i--)
        buf_len |= fgetc(f)<<(i*8);
    policy_buf_len = 0;
    for( i = 3; i >= 0; i-- )
        policy_buf_len |= fgetc(f)<<(i*8);
    g_byte_array_set_size(*policy_buf, policy_buf_len);
    fread((*policy_buf)->data, 1, policy_buf_len, f);
    mat_buf_len = buf_len - policy_buf_len;
    g_byte_array_set_size(*mat_buf, mat_buf_len);
    fread((*mat_buf)->data, 1, mat_buf_len, f);

    fclose(f);
}

void read_prv_file( char* file, GByteArray** abe_buf,GByteArray** v_buf )
{
    FILE* f;
    int i;
    long buf_len;
    long abe_buf_len;
    long v_buf_len;

    *abe_buf = g_byte_array_new();
    *v_buf = g_byte_array_new();

    f = fopen_read_or_die(file);
    buf_len = 0;
    for( i = 3; i >= 0; i--)
        buf_len |= fgetc(f)<<(i*8);
    abe_buf_len = 0;
    for( i = 3; i >= 0; i-- )
        abe_buf_len |= fgetc(f)<<(i*8);
    g_byte_array_set_size(*abe_buf, abe_buf_len);
    fread((*abe_buf)->data, 1, abe_buf_len, f);
    v_buf_len = buf_len - abe_buf_len;
    g_byte_array_set_size(*v_buf, v_buf_len);
    fread((*v_buf)->data, 1, v_buf_len, f);

    fclose(f);
}


// Last Update:2015-04-30 11:07:06
/**
 * @file serialize.cpp
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-24
 */

