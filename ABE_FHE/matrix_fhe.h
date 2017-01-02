/*
 * =====================================================================================
 *
 *       Filename:  matrix_fhe.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月22日 12时20分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
// Last Update:2015-04-22 12:20:58
/**
 * @file matrix_fhe.h
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-22
 */
#ifndef MATRIX_FHE_H
#define MATRIX_FHE_H
#include <vector>
#include <NTL/ZZ_p.h>
#include <NTL/matrix.h>

using namespace NTL;

Mat<ZZ_p> BitDecomp(Mat<ZZ_p> old);
Vec<ZZ_p> BitDecomp(Vec<ZZ_p> old);
Mat<ZZ_p> BitDecompInv(Mat<ZZ_p> old);
Vec<ZZ_p> BitDecompInv(Vec<ZZ_p> old);
Mat<ZZ_p> Flatten(Mat<ZZ_p> old);

#endif  /*MATRIX_FHE_H*/
