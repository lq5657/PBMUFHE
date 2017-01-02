/*
 * =====================================================================================
 *
 *       Filename:  ABE_FHE.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月20日 11时17分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
// Last Update:2015-05-15 17:09:13
/**
 * @file ABE_FHE.h
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-20
 */

#ifndef _A_B_E__F_H_E_H
#define _A_B_E__F_H_E_H

#include <NTL/ZZ_p.h>
#include <NTL/matrix.h>

#include "abe_core.h"
#include "ABE_mkey.h"
#include "Ctxt.h"

using namespace NTL;

class ABE_FHESecKey
{
public:
    long n;
    Vec<ZZ_p> fhe_sk;      // sk = [1 -t[0] -t[1] ... -t[-1]]
    Vec<ZZ_p> fhe_t;       // t = 取自Z_q长度为n的随机向量
    Vec<ZZ_p> fhe_v;       // v = PowersOfTwo(s)
    abe_prv_t* abe_prv;    //属性密钥
    ABE_mkey mKey;         //属性主密钥

    ABE_FHESecKey( long n, const char* params ):mKey(ABE_mkey(params, n))    //初始化mKey
    {
        this->n = n;
        ZZ_p::init(conv<ZZ>(1 << n));
        fhe_t = mKey.t;
        abe_prv = NULL;
    }
    ABE_FHESecKey( long n, abe_pub_t* pub, abe_msk_t* msk, Vec<ZZ_p> t, Mat<ZZ_p> mR):fhe_t(t), mKey(ABE_mkey(pub, msk, t, mR))
    {
        this->n = n;
        ZZ_p::init(conv<ZZ>(1 << n));
        abe_prv = NULL;
    }
    //生成密钥
    void genSeckey( char** attributes);
    //解密
    ZZ_p Decrypt( const Ctxt &ciphertxt ) const;
    static ZZ_p decrypt( abe_cph_t* policy_cp, Mat<ZZ_p> cipher, 
                         abe_pub_t* mpub, abe_prv_t* abe_key, 
                         Vec<ZZ_p> vfhe, Mat<ZZ_p> m_R );
    static bool dec_policy(unsigned long& r, abe_cph_t* policy_cp, 
                           abe_pub_t* mpub, abe_prv_t* abe_key);
private:
    void genPolicyPrvKey( char** attributes );               //生成属性密钥
    void genFHESK( );                //生成密文隐私密钥
    static ZZ_p MPdecrypt( Mat<ZZ_p> cipher, Vec<ZZ_p> vfhe);
    static ZZ_p regevDecrypt( Mat<ZZ_p> cipher, Vec<ZZ_p> vfhe);
};

class ABE_FHEPubKey
{
public :
    long n;
    long m;
    Mat<ZZ_p> A;                     //公钥矩阵
    abe_pub_t* pub;
    Vec<ZZ_p> fhe_t;       // t = 取自Z_q长度为n的随机向量
    Mat<ZZ_p> m_R;
    //ABE_mkey mKey;                  //属性主密钥
    
   // ABE_FHEPubKey( const char* params );      //初始化mKey
    ABE_FHEPubKey( long n, long m, ABE_mkey& masterKey );
    ABE_FHEPubKey( long n, long m, abe_pub_t* pub, Vec<ZZ_p> t, Mat<ZZ_p> mR);
   
    //加密
    static abe_cph_t* sta_policy_enc( char* policy, abe_pub_t* mpub, 
                                      unsigned long& r);
    Ctxt Encrypt( ZZ_p& plaintxt, char* policy );
private:
    void genFHEPK( );
    abe_cph_t* policy_enc( element_t& m_r, char* policy );
};





#endif  /*_A_B_E__F_H_E_H*/
