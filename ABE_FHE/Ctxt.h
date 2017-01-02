/*
 * =====================================================================================
 *
 *       Filename:  Ctxt.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月20日 18时25分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
// Last Update:2015-04-30 10:04:40
/**
 * @file Ctxt.h
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-20
 */

#ifndef _CTXT_H
#define _CTXT_H
#include <NTL/xdouble.h>
#include <NTL/ZZ_p.h>
#include <NTL/matrix.h>

//#include "ABE_mkey.h"
#include "abe_core.h"

using namespace NTL;

class ABE_FHEPubKey;
class ABE_FHESecKey;

class Ctxt
{
public:
    abe_cph_t* pCp;          //策略密文
    Mat<ZZ_p> A;             //密文矩阵

    Ctxt(abe_cph_t* policy_cp, Mat<ZZ_p> cipher);
    Ctxt& operator=(const Ctxt& other);
    
    //密文计算
    void addCtxt(const Ctxt& other);
    Ctxt& operator+=(const Ctxt& other) { addCtxt(other); return *this; }

    void multiplyBy(const Ctxt& other);
    Ctxt& operator*=(const Ctxt& other) { multiplyBy(other); return *this; }
};


#endif  /*_CTXT_H*/
