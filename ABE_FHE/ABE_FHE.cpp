/*
 * =====================================================================================
 *
 *       Filename:  ABE_FHE.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月21日 11时17分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <cstdlib>
#include <NTL/ZZ_p.h>
#include <NTL/matrix.h>
#include <NTL/mat_ZZ_p.h>
#include <iostream>

#include "timing.h"
#include "abe_core.h"
#include "ABE_mkey.h"
#include "ABE_FHE.h"
#include "matrix_fhe.h"

using namespace NTL;

// ABE_FHE private key implementation
/*ABE_FHESecKey::ABE_FHESecKey( const char* params )
  {
  this->mKey = new ABE_mkey(params);
  }*/
void ABE_FHESecKey::genSeckey( char** attributes)
{
    FHE_TIMER_START;
    genPolicyPrvKey(attributes);
    genFHESK();
    FHE_TIMER_STOP;
}

void ABE_FHESecKey::genPolicyPrvKey( char** attributes )
{
    //FHE_TIMER_START;
    this->abe_prv = this->mKey.prvKeyGen(attributes);
    //FHE_TIMER_STOP;
}

void ABE_FHESecKey::genFHESK( )
{
    //FHE_TIMER_START;
    //fhe_t.SetLength(n);
    long i, j;

    /*  for (i = 0; i < n; i++)
        {
        fhe_t[i] = random_ZZ_p();
        }*/

    fhe_sk.SetLength(n + 1);
    fhe_sk[0] = 1;
    for (i = 1; i <= n; i++)
    {
        fhe_sk[i] = -fhe_t[i - 1];
    }

    long l = std::floor(log2(to_long(ZZ_p::modulus()))) + 1;
    long N = l * (n + 1);

    fhe_v.SetLength(N);
    for (i = 0; i < (n+1); i++)
    {
        for (j = 0; j < l; j++)
        {
            fhe_v[l * i + j] = fhe_sk[i] * (1<<j);
        }
    }
    //FHE_TIMER_STOP;
}
ZZ_p ABE_FHESecKey::regevDecrypt( Mat<ZZ_p> cipher, Vec<ZZ_p> vfhe)
{
    FHE_TIMER_START;

    Mat<ZZ_p> mat = cipher;
    long q = to_long(ZZ_p::modulus());
    long l = std::floor(log2(q)) + 1;

    long i = 0;
    for(i = 0; i < l; i++)
    {
        if(1 << i > q >> 2)
            break;
    }

    Vec<ZZ_p> row = mat[i];
    ZZ_p x = row*vfhe;
    long x_i = conv<long>(x);
    ZZ_p result_bit = conv<ZZ_p>(((x_i + (1 << (i-1))) / (1 << i))%2);

    FHE_TIMER_STOP;
    return result_bit;
}
ZZ_p ABE_FHESecKey::MPdecrypt( Mat<ZZ_p> cipher, Vec<ZZ_p> vfhe)
{
    //FHE_TIMER_START;

    long q = to_long(ZZ_p::modulus());
    long l = std::floor(log2(q)) + 1;
    Mat<ZZ_p> C = cipher;

    bool bit;
    ZZ_p x, result = conv<ZZ_p>(0);
    long x_long;
    for (long i = l-2; i >= 0; i--)
    {
        x = C[i]*vfhe;
        x -= conv<ZZ_p>(result * (1 << i));
        x_long = conv<long>(x);
        bit = min(x_long, q-x_long) >= std::abs(x_long - (q/2));
        result += conv<ZZ_p>(bit << (l-2 - i));
    }

    //FHE_TIMER_STOP;
    return result; 
}
bool ABE_FHESecKey::dec_policy(unsigned long& r, abe_cph_t* policy_cp, 
                               abe_pub_t* mpub, abe_prv_t* abe_key)
{
    abe_cph_t* cph = policy_cp;
    abe_prv_t* prv = abe_key;
    abe_pub_t* pub = mpub;

    element_t m_r;
    element_t t;

    element_init_GT(m_r, pub->p);
    element_init_GT(t, pub->p);

    check_sat(cph->p, prv);
    if( !cph->p->satisfiable )
    {
        std::cout << "cannot decrypt, attributes in key do not satisfy policy" << std::endl;
        //raise_error("cannot decrypt, attributes in key do not satisfy policy\n");
        //return conv<ZZ_p>(0);
        return false;
    }
    pick_sat_min_leaves(cph->p, prv);
    dec_flatten(t, cph->p, prv, pub);
    element_mul(m_r, cph->cs, t);
    pairing_apply(t, cph->c, prv->d, pub->p);
    element_invert(t, t);
    element_mul(m_r, m_r, t);

    mpz_t z;
    mpz_init(z);
    element_to_mpz(z, m_r);
    r = mpz_get_ui(z);
    mpz_clear(z);

    //std::cout << "decrypt: r = " << r << std::endl;

    return true;
}
ZZ_p ABE_FHESecKey::decrypt( abe_cph_t* policy_cp, Mat<ZZ_p> cipher,
                             abe_pub_t* mpub, abe_prv_t* abe_key, 
                             Vec<ZZ_p> vfhe, Mat<ZZ_p> m_R )
{
    FHE_TIMER_START;

    //element_t m_r;
    unsigned long r;
    bool isSatified = dec_policy(r, policy_cp, mpub, abe_key);
    if( !isSatified )
        return conv<ZZ_p>(0);

    //long r;
    //mpz_t z;
    //element_set_mpz(m_r, z);
    //r = mpz_get_ui(z);

    ZZ_p result;
    //result = regevDecrypt(cipher, vfhe);
    Mat<ZZ_p> mat_flatten = Flatten(m_R * r);
    result = MPdecrypt((cipher - mat_flatten), vfhe);

    FHE_TIMER_STOP;
    return result;
}
ZZ_p ABE_FHESecKey::Decrypt( const Ctxt &ciphertxt ) const
{
    FHE_TIMER_START;

    abe_cph_t* cph = ciphertxt.pCp;
    abe_prv_t* prv = this->abe_prv;
    //abe_pub_t* pub = this->mKey.abe_mpub;

    check_sat(cph->p, prv);
    if( !cph->p->satisfiable )
    {
        std::cout << "cannot decrypt, attributes in key do not satisfy policy" << std::endl;
        //raise_error("cannot decrypt, attributes in key do not satisfy policy\n");
        return conv<ZZ_p>(0);
    }

    Mat<ZZ_p> mat = ciphertxt.A;
    long q = to_long(ZZ_p::modulus());
    long l = std::floor(log2(q)) + 1;

    long i = 0;
    for(i = 0; i < l; i++)
    {
        if(1 << i > q >> 2)
            break;
    }

    Vec<ZZ_p> row = mat[i];
    ZZ_p x = row*fhe_v;
    long x_i = conv<long>(x);
    ZZ_p result_bit = conv<ZZ_p>(((x_i + (1 << (i-1))) / (1 << i))%2);

    FHE_TIMER_STOP;
    return result_bit;
}

//ABE_FHE public key implementation

/*ABE_FHEPubKey::ABE_FHEPubKey(ABE_mkey& masterKey)
  {
  this->mKey = masterKey;
  }*/

ABE_FHEPubKey::ABE_FHEPubKey( long n, long m, ABE_mkey& masterKey )
{
    this->n = n;
    this->m = m;
    ZZ_p::init(conv<ZZ>(1 << n));

    pub = masterKey.abe_mpub;
    fhe_t = masterKey.t;
    m_R = masterKey.mR;

    /*fhe_t.SetLength(n);
      long i, j;

      for (i = 0; i < n; i++)
      {
      fhe_t[i] = random_ZZ_p();
      }*/
    genFHEPK();
}
ABE_FHEPubKey::ABE_FHEPubKey( long n, long m, abe_pub_t* pub, Vec<ZZ_p> t,Mat<ZZ_p> mR)
{
    this->n = n;
    this->m = m;
    ZZ_p::init(conv<ZZ>(1 << n));
    this->pub = pub;
    this->fhe_t = t;
    this->m_R = mR;

    genFHEPK();
}


void ABE_FHEPubKey::genFHEPK( )
{
    Mat<ZZ_p> B;
    B.SetDims(m, n);

    long i, j;
    for (i = 0; i < m; i++)
    {
        for(j = 0; j < n; j++)
        {
            B[i][j] = random_ZZ_p();
        }
    }

    //创建error vector
    Vec<ZZ_p> e;
    long stdev = 8;
    e.SetLength(m);
    for (i = 0; i < m; i++)
    {
        static double const Pi = 4.0*atan(1.0);
        static long const bignum = 0xfffffff;

        //使用Box-Muller算法获取正太分布（0,stdev^2）变量
        double r1 = (1+RandomBnd(bignum))/((double)bignum+1);
        double r2 = (1+RandomBnd(bignum))/((double)bignum+1);
        double theta = 2*Pi*r1;
        double rr = sqrt(-2.0*log(r2))*stdev;
        //生成两个Gaussians RV's
        long x = (long) floor(rr*cos(theta) + 0.5);
        e[i] = x;
    }
    Vec<ZZ_p> t = fhe_t;

    A.SetDims(m, n+1);
    //用向量b填充矩阵A的第一列，向量b由矩阵B乘以向量t并加上error向量e
    for (i = 0; i < m; i++)
    {
        long actual_sum = 0;
        for (j = 0; j < n; j++)
        {
            actual_sum += conv<long>(B[i][j] * t[j]);
        }
        A[i][0] = actual_sum + conv<long>(e[i]);
    }
    //用矩阵B填充A的其余部分
    for (i = 0; i < m; i++)
    {
        for (j = 1; j <= n; j++)
        {
            A[i][j] = B[i][j - 1];
        }
    }
}

//static element_t m_r;

abe_cph_t* ABE_FHEPubKey::policy_enc( element_t& m_r,char* policy )
{
    FHE_TIMER_START;

    abe_cph_t* cph;
    element_t s;

    cph = (abe_cph_t*)malloc(sizeof(abe_cph_t));
    element_init_Zr(s, pub->p);
    element_init_GT(m_r, pub->p);
    element_init_GT(cph->cs, pub->p);
    element_init_G1(cph->c, pub->p);

    cph->p = parse_policy_postfix(policy);

    element_random(m_r);
    //mpz_t z;
    //long r;
    //element_to_mpz(z, m_r);
    //r = mpz_get_ui(z); 
    //std::cout << "m_r=" << r << std::endl;

    element_random(s);
    element_pow_zn(cph->cs, pub->g_hat_alpha, s);
    element_mul(cph->cs, cph->cs, m_r);

    element_pow_zn(cph->c, pub->h, s);

    enc_policy(cph->p, pub, s);

    FHE_TIMER_STOP;
    return cph;
}

abe_cph_t* ABE_FHEPubKey::sta_policy_enc( char* policy, abe_pub_t* mpub, 
                                          unsigned long& r)
{
    FHE_TIMER_START;

    abe_cph_t* cph;
    abe_pub_t* pub = mpub;
    element_t m_r;
    element_t s;

    cph = (abe_cph_t*)malloc(sizeof(abe_cph_t));
    element_init_Zr(s, pub->p);
    element_init_GT(m_r, pub->p);
    element_init_GT(cph->cs, pub->p);
    element_init_G1(cph->c, pub->p);

    cph->p = parse_policy_postfix(policy);

    element_random(m_r);
    element_random(s);
    element_pow_zn(cph->cs, pub->g_hat_alpha, s);
    element_mul(cph->cs, cph->cs, m_r);

    element_pow_zn(cph->c, pub->h, s);

    enc_policy(cph->p, pub, s);
    
    mpz_t z;
    mpz_init(z);
    element_to_mpz(z, m_r);
    r = mpz_get_ui(z);
    mpz_clear(z);
    //std::cout << "Encrypt:r = " << r << std::endl;

    FHE_TIMER_STOP;
    return cph;
}


Ctxt ABE_FHEPubKey::Encrypt( ZZ_p& plaintxt, char* policy )
{
    FHE_TIMER_START;

    abe_cph_t* cph;
    element_t m_r;
    //cph = policy_enc(policy);
    element_t s;

    cph = (abe_cph_t*)malloc(sizeof(abe_cph_t));
    element_init_Zr(s, pub->p);
    element_init_GT(m_r, pub->p);
    element_init_GT(cph->cs, pub->p);
    element_init_G1(cph->c, pub->p);

    cph->p = parse_policy_postfix(policy);

    element_random(m_r);
    element_random(s);
    element_pow_zn(cph->cs, pub->g_hat_alpha, s);
    element_mul(cph->cs, cph->cs, m_r);

    element_pow_zn(cph->c, pub->h, s);

    enc_policy(cph->p, pub, s);

    unsigned long r;
    mpz_t z;
    mpz_init(z);
    element_to_mpz(z, m_r);
    r = mpz_get_ui(z);
    mpz_clear(z);

    //加密明文
    long l = std::floor(log2(to_long(ZZ_p::modulus()))) + 1;
    long ncols = A.NumCols() - 1;
    long mrows = A.NumRows();
    long N = (ncols+1) * l;
    Mat<ZZ_p> R;
    R.SetDims(N, mrows);

    long i, j;
    for (i = 0; i < N; i++)
    {
        for(j = 0; j < mrows; j++)
            R[i][j] = RandomBnd(2);
    }
    Mat<ZZ_p> ident = ident_mat_ZZ_p(N);
    ident *= plaintxt;

    Mat<ZZ_p> cipher = Flatten(ident + BitDecomp(R*A) + r * m_R);

    Ctxt ctxt(cph, cipher);

    FHE_TIMER_STOP;
    return ctxt;
}
