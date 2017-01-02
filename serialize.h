/*
 * =====================================================================================
 *
 *       Filename:  serialize.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月24日 21时00分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
// Last Update:2015-04-24 21:04:28
/**
 * @file serialize.h
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-24
 */

#ifndef SERIALIZE_H
#define SERIALIZE_H
#include <NTL/ZZ_p.h>
#include <NTL/matrix.h>
#include <NTL/mat_ZZ_p.h>
#include <glib.h>
using namespace NTL;

void vec_ZZ_p_serialize( GByteArray* b, Vec<ZZ_p> oldVec );
Vec<ZZ_p> unserialize_vec_ZZ_p( GByteArray* b );
Vec<ZZ_p> unserialize_vec_ZZ_p( GByteArray* b, guint* offset );
void mat_ZZ_p_serialize( GByteArray* b,Mat<ZZ_p> oldMat );
Mat<ZZ_p> unserialize_mat_ZZ_p( GByteArray* b );

void read_cph_file( char* file, GByteArray** policy_buf, GByteArray** mat_buf );
void read_prv_file( char* file, GByteArray** abe_buf,GByteArray** v_buf );

#endif  /*SERIALIZE_H*/
