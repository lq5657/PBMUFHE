/*
 * =====================================================================================
 *
 *       Filename:  ABE_mkey.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月20日 15时20分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <cstdlib>
#include <cstring>
#include <string>

#include "ABE_mkey.h"
#include "abe_core.h"

ABE_mkey::ABE_mkey(const char* params, long n)
{
    abe_mpub = (abe_pub_t*)malloc(sizeof(abe_pub_t));
    abe_msec = (abe_msk_t*)malloc(sizeof(abe_msk_t));
    abe_setup(&abe_mpub, &abe_msec, params);

    ZZ_p::init(conv<ZZ>(1 << n));
    t.SetLength(n);
    long i,j;
    for (i = 0; i < n; i++)
    {
        t[i] = random_ZZ_p();
    }

    long l = std::floor(log2(to_long(ZZ_p::modulus()))) + 1;
    long N = (n + 1) * l;
    mR.SetDims(N, N);
    
    for (i = 0; i < N; i++)
    {
        for(j = 0; j < N; j++)
            mR[i][j] = RandomBnd(2);
    }
}

ABE_mkey::ABE_mkey(const ABE_mkey& mKey)
{
   this->abe_mpub = mKey.abe_mpub;
   this->abe_msec = mKey.abe_msec;
   this->t = mKey.t;
   this->mR = mKey.mR;
}

ABE_mkey& ABE_mkey::operator=(const ABE_mkey& mKey)
{
    if(this == &mKey)
        return *this;
    this->abe_mpub = mKey.abe_mpub;
    this->abe_msec = mKey.abe_msec;
    this->t = mKey.t;
    this->mR = mKey.mR;
    return *this;
}

void ABE_mkey::mkey_free()
{
    if(abe_mpub)
        abe_pub_free(abe_mpub);
    if(abe_msec)
        abe_msk_free(abe_msec);
}

abe_prv_t* ABE_mkey::prvKeyGen( char** attributes )
{
    if(!abe_mpub || !abe_msec)
    {
        raise_error(const_cast<char*>(std::string("cannot prvKeyGen, abe_mpub or abe_msec is null!\n").c_str()));
        return NULL;
    }

    abe_prv_t* prv;
    element_t g_r;
    element_t r;
    element_t beta_inv;
    
    prv = (abe_prv_t*)malloc(sizeof(abe_prv_t));

    element_init_G2(prv->d, abe_mpub->p);
    element_init_G2(g_r, abe_mpub->p);
    element_init_Zr(r, abe_mpub->p);
    element_init_Zr(beta_inv, abe_mpub->p);
    
    prv->cmps = g_array_new(0, 1, sizeof(abe_prv_comp_t));

    element_random(r);
    element_pow_zn(g_r, abe_mpub->gp, r);
    element_mul(prv->d, abe_msec->g_alpha, g_r);
    element_invert(beta_inv, abe_msec->beta);
    element_pow_zn(prv->d, prv->d, beta_inv);

    while( *attributes )
    {
        abe_prv_comp_t c;
        element_t h_rp;
        element_t rp;

        c.attr = *(attributes++);

        element_init_G2(c.d, abe_mpub->p);
        element_init_G1(c.dp, abe_mpub->p);
        element_init_G2(h_rp, abe_mpub->p);
        element_init_Zr(rp, abe_mpub->p);

        element_from_string(h_rp, c.attr);
        element_random(rp);
        element_pow_zn(h_rp, h_rp, rp);

        element_mul(c.d, g_r, h_rp);
        element_pow_zn(c.dp, abe_mpub->g, rp);

        element_clear(h_rp);
        element_clear(rp);

        g_array_append_val(prv->cmps, c);
    }
    return prv;
}

