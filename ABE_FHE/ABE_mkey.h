/*
 * =====================================================================================
 *
 *       Filename:  ABE_mkey.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月20日 16时55分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
// Last Update:2015-05-14 21:25:58
/**
 * @file ABE_mkey.h
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-20
 */

#ifndef _A_B_E_MKEY_H
#define _A_B_E_MKEY_H
#include <NTL/ZZ_p.h>
#include <NTL/matrix.h>
#include "abe_core.h"
using namespace NTL;

class ABE_mkey
{
public:
    abe_pub_t* abe_mpub;
    abe_msk_t* abe_msec;
    Vec<ZZ_p> t;
    Mat<ZZ_p> mR;

    ABE_mkey(const char* params, long n);
    ABE_mkey(abe_pub_t* pub, abe_msk_t* msk, Vec<ZZ_p> t, Mat<ZZ_p> mr):abe_mpub(pub), abe_msec(msk)
    {
        this->t = t;
        this->mR = mR;
    }
    ABE_mkey(const ABE_mkey& mKey);              //拷贝构造函数
    ABE_mkey& operator=(const ABE_mkey& mKey);    //赋值重载构造函数
    void mkey_free();

    abe_prv_t* prvKeyGen(char** attr);          //根据属性生成密钥
};


#endif  /*_A_B_E_MKEY_H*/
