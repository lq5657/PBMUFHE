/*
 * =====================================================================================
 *
 *       Filename:  matrix_fhe.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月22日 12时21分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "matrix_fhe.h"
#include <NTL/matrix.h>
#include <NTL/ZZ_p.h>
#include <NTL/mat_ZZ_p.h>

NTL_CLIENT
using namespace NTL;

Mat<ZZ_p> BitDecomp(Mat<ZZ_p> old) {
  long m = old.NumRows();
  long l = std::floor(log2(to_long(ZZ_p::modulus()))) + 1;
  long n = old.NumCols() - 1;
  long N = (n+1) * l;

  Mat<ZZ_p> decomp;
  decomp.SetDims(m, N);
  for (long i = 0; i < m; i++) {
    Vec<ZZ_p> new_row = BitDecomp(old[i]);
    for (long j = 0; j < N; j++) {
      decomp[i][j] = new_row[j];
    }
  }
  return decomp;
}

Vec<ZZ_p> BitDecomp(Vec<ZZ_p> old) {
  long l = std::floor(log2(to_long(ZZ_p::modulus()))) + 1;
  long k = old.length();

  Vec<ZZ_p> decomp;
  decomp.SetLength(l*k);
  for (long i = 0; i < k; i++) {
    long old_elem = conv<long>(old[i]);
    // Ordered with least significant first
    for (long j = 0; j < l; j++) {
      decomp[i*l + j] = old_elem % 2;
      old_elem >>= 1;
    }
  }
  return decomp;
}

Mat<ZZ_p> BitDecompInv(Mat<ZZ_p> old) {
  long m = old.NumRows();
  long l = std::floor(log2(to_long(ZZ_p::modulus()))) + 1;
  long n = old.NumCols();
  long N = n/l;

  Mat<ZZ_p> decomp;
  decomp.SetDims(m, N);
  for (long i = 0; i < m; i++) {
    Vec<ZZ_p> new_row = BitDecompInv(old[i]);
    for (long j = 0; j < N; j++) {
      decomp[i][j] = new_row[j];
    }
  }
  return decomp;
}

Vec<ZZ_p> BitDecompInv(Vec<ZZ_p> old) {
  long l = std::floor(log2(to_long(ZZ_p::modulus()))) + 1;
  long k = old.length();

  Vec<ZZ_p> decomp;
  decomp.SetLength(k/l);
  for (long i = 0; i < k/l; i++) {
    long current = 0;
    for (long j = 0; j < l; j++) {
      current += conv<long>(old[i*l + j]) * (1 << j);
    }
    decomp[i] = current;
  }
  return decomp;
}

Mat<ZZ_p> Flatten(Mat<ZZ_p> old) {
  return BitDecomp(BitDecompInv(old));
}
// Last Update:2015-04-24 17:42:08
/**
 * @file matrix_fhe.cpp
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-22
 */

